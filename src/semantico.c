#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "semantico.h"
#include "simbolos.h"

/* ── Lista de errores semanticos ────────────────────────────── */
static ErrorSem *errores      = NULL;
static ErrorSem *errores_tail = NULL;

static void agregar_error(int linea, const char *fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    ErrorSem *e = (ErrorSem*)malloc(sizeof(ErrorSem));
    e->linea     = linea;
    e->mensaje   = strdup(buf);
    e->siguiente = NULL;
    if (errores_tail) errores_tail->siguiente = e;
    else              errores = e;
    errores_tail = e;
}

int hay_errores_sem(void) {
    return errores != NULL;
}

void imprimir_errores_sem(void) {
    printf("\n==================================================\n");
    printf("ANALISIS SEMANTICO\n");
    printf("==================================================\n");
    if (!errores) {
        printf("No se detectaron errores semanticos.\n");
    } else {
        for (ErrorSem *e = errores; e; e = e->siguiente)
            printf("[ERROR SEMANTICO] Linea %d: %s\n", e->linea, e->mensaje);
    }
    printf("==================================================\n");
}

void emitir_errores_gui(void) {
    for (ErrorSem *e = errores; e; e = e->siguiente)
        printf("semantico|%d|%s\n", e->linea, e->mensaje);
}

void liberar_errores_sem(void) {
    ErrorSem *e = errores;
    while (e) {
        ErrorSem *n = e->siguiente;
        free(e->mensaje);
        free(e);
        e = n;
    }
    errores = errores_tail = NULL;
}

/* ── Utilidades de tipos ────────────────────────────────────── */

static int es_numerico(const char *t) {
    return t && (strcmp(t, "num") == 0 || strcmp(t, "dec") == 0);
}

/* Compatibilidad de asignacion: dest recibe src.
   num <-> dec son compatibles (widening/narrowing permitido).
   text solo con text; booleano solo con booleano. */
static int tipos_compatibles(const char *dest, const char *src) {
    if (!dest || !src) return 0;
    if (strcmp(dest, src) == 0) return 1;
    if (es_numerico(dest) && es_numerico(src)) return 1;
    return 0;
}

/* ── Inferencia de tipos de una expresion ─────────────────────
   Recorre el subarbol de expresion, registra los errores que encuentre
   y devuelve el tipo inferido ("num","dec","text","booleano") o NULL. */
static const char* inferir_tipo(NodoAST *n) {
    if (!n) return NULL;

    switch (n->tipo) {
        case NODO_ENTERO:    return "num";
        case NODO_DECIMAL:   return "dec";
        case NODO_TEXTO:     return "text";
        case NODO_BOOLEANO:  return "booleano";

        case NODO_IDENTIFICADOR: {
            Simbolo *s = buscar(n->str_val);
            if (!s) {
                agregar_error(n->linea, "Variable '%s' no declarada.", n->str_val);
                return NULL;
            }
            return s->tipo;
        }

        case NODO_OP_BINARIO: {
            const char *op = n->str_val;
            const char *t1 = inferir_tipo(n->izq);
            const char *t2 = inferir_tipo(n->der);
            if (!t1 || !t2) return NULL;   /* error ya reportado */

            if (strcmp(op,"+")==0 || strcmp(op,"-")==0 ||
                strcmp(op,"*")==0 || strcmp(op,"/")==0 || strcmp(op,"%")==0) {
                /* operador aritmetico */
                if (!es_numerico(t1) || !es_numerico(t2)) {
                    agregar_error(n->linea,
                        "Operacion aritmetica '%s' invalida entre tipos '%s' y '%s'.",
                        op, t1, t2);
                    return NULL;
                }
                return (strcmp(t1,"dec")==0 || strcmp(t2,"dec")==0) ? "dec" : "num";
            }

            if (strcmp(op,">")==0 || strcmp(op,"<")==0 ||
                strcmp(op,">=")==0 || strcmp(op,"<=")==0 ||
                strcmp(op,"==")==0 || strcmp(op,"!=")==0) {
                /* operador relacional */
                if (!es_numerico(t1) || !es_numerico(t2)) {
                    agregar_error(n->linea,
                        "Comparacion '%s' invalida entre tipos '%s' y '%s'.",
                        op, t1, t2);
                    return NULL;
                }
                return "booleano";
            }

            if (strcmp(op,"&&")==0 || strcmp(op,"||")==0) {
                /* operador logico */
                if (strcmp(t1,"booleano")!=0 || strcmp(t2,"booleano")!=0) {
                    agregar_error(n->linea,
                        "Operacion logica '%s' requiere operandos booleanos (recibido '%s' y '%s').",
                        op, t1, t2);
                    return NULL;
                }
                return "booleano";
            }
            return NULL;
        }

        case NODO_LEER_TECLA:
            /* tecla() devuelve el codigo de la tecla leida (0 si ninguna). */
            return "num";

        case NODO_ALEATORIO: {
            /* aleatorio(n) exige un limite numerico y devuelve 'num'. */
            const char *t1 = inferir_tipo(n->izq);
            if (!t1) return NULL;
            if (!es_numerico(t1)) {
                agregar_error(n->linea,
                    "aleatorio(n) requiere un limite numerico (recibido '%s').", t1);
                return NULL;
            }
            return "num";
        }

        case NODO_OP_UNARIO: {
            const char *op = n->str_val;
            const char *t1 = inferir_tipo(n->izq);
            if (!t1) return NULL;

            if (strcmp(op,"!")==0) {
                if (strcmp(t1,"booleano")!=0) {
                    agregar_error(n->linea,
                        "Operador '!' requiere operando booleano (recibido '%s').", t1);
                    return NULL;
                }
                return "booleano";
            }
            if (strcmp(op,"-")==0) {
                if (!es_numerico(t1)) {
                    agregar_error(n->linea,
                        "Operador '-' unario requiere operando numerico (recibido '%s').", t1);
                    return NULL;
                }
                return t1;
            }
            return NULL;
        }

        default:
            return NULL;
    }
}

/* ── Recorrido del AST (sentencias) ─────────────────────────── */
static void visitar(NodoAST *n) {
    if (!n) return;

    switch (n->tipo) {
        case NODO_PROGRAMA:
            tabla_init();
            visitar(n->izq);
            break;

        case NODO_LISTA_SENT:
            visitar(n->izq);
            visitar(n->extra);
            break;

        case NODO_DECLARACION: {
            char *tipo   = n->str_val;        /* "num","dec","text" */
            char *nombre = n->izq->str_val;   /* nombre de variable */
            int   linea  = n->linea;

            if (declarar(nombre, tipo, linea, n->der ? 1 : 0) != 0) {
                agregar_error(linea,
                    "Doble declaracion de la variable '%s' en el mismo ambito.", nombre);
            }
            if (n->der) {
                const char *te = inferir_tipo(n->der);
                if (te && !tipos_compatibles(tipo, te)) {
                    agregar_error(n->der->linea,
                        "Tipo incompatible en declaracion: no se puede asignar '%s' a '%s' (variable '%s').",
                        te, tipo, nombre);
                }
            }
            break;
        }

        case NODO_ASIGNACION: {
            char    *nombre = n->izq->str_val;
            Simbolo *s      = buscar(nombre);
            if (!s) {
                agregar_error(n->linea,
                    "Asignacion a variable '%s' no declarada.", nombre);
                inferir_tipo(n->der);   /* igual recorre para detectar mas errores */
            } else {
                const char *te = inferir_tipo(n->der);
                if (te && !tipos_compatibles(s->tipo, te)) {
                    agregar_error(n->linea,
                        "Tipo incompatible: no se puede asignar '%s' a '%s' (variable '%s').",
                        te, s->tipo, nombre);
                }
                marcar_inicializado(nombre);
            }
            break;
        }

        case NODO_CONDICIONAL: {
            const char *tc = inferir_tipo(n->izq);
            if (tc && strcmp(tc, "booleano") != 0) {
                agregar_error(n->izq->linea,
                    "La condicion de 'cuando' debe ser booleana (recibido '%s').", tc);
            }
            entrar_scope();
            visitar(n->der);     /* bloque si */
            salir_scope();
            if (n->extra) {
                entrar_scope();
                visitar(n->extra);  /* bloque sino */
                salir_scope();
            }
            break;
        }

        case NODO_CICLO: {
            const char *tc = inferir_tipo(n->izq);
            if (tc && strcmp(tc, "booleano") != 0) {
                agregar_error(n->izq->linea,
                    "La condicion de 'loop' debe ser booleana (recibido '%s').", tc);
            }
            entrar_scope();
            visitar(n->der);     /* cuerpo */
            salir_scope();
            break;
        }

        case NODO_IMPRESION:
            inferir_tipo(n->izq);   /* valida que la expresion sea correcta */
            break;

        case NODO_RETORNO:
            if (n->izq) inferir_tipo(n->izq);
            break;

        /* ── Primitivas de juego ─────────────────────────────── */
        case NODO_LIMPIAR:
            /* limpiar() no lleva argumentos: nada que validar. */
            break;

        case NODO_POSICIONAR: {
            const char *tx = inferir_tipo(n->izq);
            const char *ty = inferir_tipo(n->der);
            if (tx && !es_numerico(tx))
                agregar_error(n->linea,
                    "posicionar(x, y): la columna debe ser numerica (recibido '%s').", tx);
            if (ty && !es_numerico(ty))
                agregar_error(n->linea,
                    "posicionar(x, y): la fila debe ser numerica (recibido '%s').", ty);
            break;
        }

        case NODO_DIBUJAR:
            inferir_tipo(n->izq);   /* imprime sin salto: acepta cualquier valor */
            break;

        case NODO_PINTAR: {
            const char *tc = inferir_tipo(n->izq);
            if (tc && !es_numerico(tc))
                agregar_error(n->linea,
                    "pintar(codigo): el codigo de caracter debe ser numerico (recibido '%s').", tc);
            break;
        }

        case NODO_ESPERAR: {
            const char *te = inferir_tipo(n->izq);
            if (te && !es_numerico(te))
                agregar_error(n->linea,
                    "esperar(n): el numero de cuadros debe ser numerico (recibido '%s').", te);
            break;
        }

        default:
            break;
    }
}

void analizar_semantico(NodoAST *raiz) {
    errores = errores_tail = NULL;
    visitar(raiz);
}

