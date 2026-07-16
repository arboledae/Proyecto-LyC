#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codigo_intermedio.h"

/* ── Lista de cuadruplas ────────────────────────────────────── */
static Cuad *cuads      = NULL;
static Cuad *cuads_tail = NULL;
static int   n_cuads    = 0;
static int   n_temps    = 0;   /* contador de temporales t1..tn  */
static int   n_labels   = 0;   /* contador de etiquetas L1..Ln   */

/* ── Tabla de variables con nombre unico por ambito ───────────
   Replica la numeracion de ambitos del analisis semantico:
   inicio{} es el ambito 0 y cada bloque cuando/sino/loop abre
   uno nuevo con id creciente. Como el semantico ya valido el
   programa, aqui toda busqueda debe tener exito.               */
static TacSim *sims = NULL, *sims_tail = NULL;
static int scope_pila[128];
static int scope_top;          /* indice del tope de la pila     */
static int scope_contador;     /* ultimo id de ambito creado     */

static char* dupstr(const char *s) { return s ? strdup(s) : NULL; }

/* Nombres como t1 o L3 chocarian con temporales/etiquetas del TAC:
   esos casos siempre llevan sufijo de ambito, incluso en el global. */
static int parece_temp_o_etiqueta(const char *n) {
    if (n[0] != 't' && n[0] != 'L') return 0;
    if (!n[1]) return 0;
    for (const char *p = n + 1; *p; p++)
        if (*p < '0' || *p > '9') return 0;
    return 1;
}

static TacSim* sim_declarar(const char *nombre, const char *tipo) {
    TacSim *s = (TacSim*)malloc(sizeof(TacSim));
    int scope = scope_pila[scope_top];
    s->nombre = dupstr(nombre);
    s->tipo   = dupstr(tipo);
    s->scope  = scope;
    if (scope == 0 && !parece_temp_o_etiqueta(nombre)) {
        s->unico = dupstr(nombre);
    } else {
        s->unico = (char*)malloc(strlen(nombre) + 16);
        sprintf(s->unico, "%s.%d", nombre, scope);
    }
    s->sig = NULL;
    if (sims_tail) sims_tail->sig = s; else sims = s;
    sims_tail = s;
    return s;
}

static TacSim* sim_buscar(const char *nombre) {
    /* del ambito mas interno activo hacia el global */
    for (int i = scope_top; i >= 0; i--) {
        for (TacSim *s = sims; s; s = s->sig)
            if (s->scope == scope_pila[i] && strcmp(s->nombre, nombre) == 0)
                return s;
    }
    return NULL;
}

/* ── Emision de cuadruplas ──────────────────────────────────── */
static void emitir(const char *op, const char *a1, const char *a2, const char *res) {
    Cuad *c = (Cuad*)malloc(sizeof(Cuad));
    c->op   = dupstr(op);
    c->arg1 = dupstr(a1);
    c->arg2 = dupstr(a2);
    c->res  = dupstr(res);
    c->sig  = NULL;
    if (cuads_tail) cuads_tail->sig = c; else cuads = c;
    cuads_tail = c;
    n_cuads++;
}

static char* nuevo_temp(void) {
    char buf[16];
    sprintf(buf, "t%d", ++n_temps);
    return dupstr(buf);
}

static char* nueva_etiqueta(void) {
    char buf[16];
    sprintf(buf, "L%d", ++n_labels);
    return dupstr(buf);
}

/* ── Generacion de expresiones ────────────────────────────────
   Devuelve el operando (strdup) donde quedo el valor y, si
   tipo_out != NULL, el tipo inferido del subarbol.             */
static const char* tipo_de(NodoAST *n);

static char* gen_expr(NodoAST *n, const char **tipo_out) {
    if (!n) return NULL;
    if (tipo_out) *tipo_out = tipo_de(n);

    switch (n->tipo) {
        case NODO_ENTERO: {
            char buf[24];
            sprintf(buf, "%d", n->int_val);
            return dupstr(buf);
        }
        case NODO_DECIMAL:  return dupstr(n->str_val);
        case NODO_TEXTO:    return dupstr(n->str_val);   /* conserva comillas */
        case NODO_BOOLEANO: return dupstr(n->int_val ? "1" : "0");

        case NODO_IDENTIFICADOR: {
            TacSim *s = sim_buscar(n->str_val);
            return dupstr(s ? s->unico : n->str_val);
        }

        case NODO_OP_BINARIO: {
            char *a = gen_expr(n->izq, NULL);
            char *b = gen_expr(n->der, NULL);
            char *t = nuevo_temp();
            emitir(n->str_val, a, b, t);
            free(a); free(b);
            return t;
        }

        case NODO_OP_UNARIO: {
            char *a = gen_expr(n->izq, NULL);
            char *t = nuevo_temp();
            emitir(strcmp(n->str_val, "!") == 0 ? "not" : "neg", a, NULL, t);
            free(a);
            return t;
        }

        case NODO_LEER_TECLA: {
            char *t = nuevo_temp();
            emitir("tecla", NULL, NULL, t);
            return t;
        }

        case NODO_ALEATORIO: {
            char *a = gen_expr(n->izq, NULL);
            char *t = nuevo_temp();
            emitir("rand", a, NULL, t);
            free(a);
            return t;
        }

        default:
            return dupstr("0");
    }
}

/* Tipo inferido de una expresion ya validada por el semantico. */
static const char* tipo_de(NodoAST *n) {
    if (!n) return "num";
    switch (n->tipo) {
        case NODO_ENTERO:    return "num";
        case NODO_DECIMAL:   return "dec";
        case NODO_TEXTO:     return "text";
        case NODO_BOOLEANO:  return "booleano";
        case NODO_IDENTIFICADOR: {
            TacSim *s = sim_buscar(n->str_val);
            return s ? s->tipo : "num";
        }
        case NODO_OP_BINARIO: {
            const char *op = n->str_val;
            if (strcmp(op,"+")==0 || strcmp(op,"-")==0 || strcmp(op,"*")==0 ||
                strcmp(op,"/")==0 || strcmp(op,"%")==0) {
                const char *t1 = tipo_de(n->izq), *t2 = tipo_de(n->der);
                return (strcmp(t1,"dec")==0 || strcmp(t2,"dec")==0) ? "dec" : "num";
            }
            return "booleano";  /* relacionales y logicos */
        }
        case NODO_OP_UNARIO:
            return (strcmp(n->str_val,"!")==0) ? "booleano" : tipo_de(n->izq);
        case NODO_LEER_TECLA:
        case NODO_ALEATORIO:
            return "num";
        default:
            return "num";
    }
}

/* ── Recorrido de sentencias ────────────────────────────────── */
static void gen_sent(NodoAST *n) {
    if (!n) return;

    switch (n->tipo) {
        case NODO_PROGRAMA:
            gen_sent(n->izq);
            break;

        case NODO_LISTA_SENT:
            gen_sent(n->izq);
            gen_sent(n->extra);
            break;

        case NODO_DECLARACION: {
            TacSim *s = sim_declarar(n->izq->str_val, n->str_val);
            if (n->der) {
                char *v = gen_expr(n->der, NULL);
                emitir("=", v, NULL, s->unico);
                free(v);
            }
            break;
        }

        case NODO_ASIGNACION: {
            char *v = gen_expr(n->der, NULL);
            TacSim *s = sim_buscar(n->izq->str_val);
            emitir("=", v, NULL, s ? s->unico : n->izq->str_val);
            free(v);
            break;
        }

        case NODO_CONDICIONAL: {
            char *cond   = gen_expr(n->izq, NULL);
            char *l_fin  = nueva_etiqueta();
            char *l_sino = n->extra ? nueva_etiqueta() : NULL;

            emitir("ifFalse", cond, NULL, l_sino ? l_sino : l_fin);

            scope_pila[++scope_top] = ++scope_contador;   /* bloque si */
            gen_sent(n->der);
            scope_top--;

            if (n->extra) {
                emitir("goto", NULL, NULL, l_fin);
                emitir("label", NULL, NULL, l_sino);
                scope_pila[++scope_top] = ++scope_contador;  /* bloque sino */
                gen_sent(n->extra);
                scope_top--;
            }
            emitir("label", NULL, NULL, l_fin);
            free(cond); free(l_fin); if (l_sino) free(l_sino);
            break;
        }

        case NODO_CICLO: {
            char *l_ini = nueva_etiqueta();
            char *l_fin = nueva_etiqueta();

            emitir("label", NULL, NULL, l_ini);
            char *cond = gen_expr(n->izq, NULL);
            emitir("ifFalse", cond, NULL, l_fin);

            scope_pila[++scope_top] = ++scope_contador;      /* cuerpo */
            gen_sent(n->der);
            scope_top--;

            emitir("goto", NULL, NULL, l_ini);
            emitir("label", NULL, NULL, l_fin);
            free(l_ini); free(l_fin); free(cond);
            break;
        }

        case NODO_IMPRESION: {
            const char *tipo = "num";
            char *v = gen_expr(n->izq, &tipo);
            emitir("imprimir", v, tipo, NULL);
            free(v);
            break;
        }

        case NODO_RETORNO: {
            if (n->izq) {
                char *v = gen_expr(n->izq, NULL);
                emitir("retornar", v, NULL, NULL);
                free(v);
            } else {
                emitir("retornar", NULL, NULL, NULL);
            }
            break;
        }

        /* ── Primitivas de juego ─────────────────────────────── */
        case NODO_LIMPIAR:
            emitir("cls", NULL, NULL, NULL);
            break;

        case NODO_POSICIONAR: {
            char *x = gen_expr(n->izq, NULL);
            char *y = gen_expr(n->der, NULL);
            emitir("locate", x, y, NULL);
            free(x); free(y);
            break;
        }

        case NODO_DIBUJAR: {
            const char *tipo = "num";
            char *v = gen_expr(n->izq, &tipo);
            emitir("draw", v, tipo, NULL);
            free(v);
            break;
        }

        case NODO_PINTAR: {
            char *v = gen_expr(n->izq, NULL);
            emitir("putc", v, NULL, NULL);
            free(v);
            break;
        }

        case NODO_ESPERAR: {
            char *v = gen_expr(n->izq, NULL);
            emitir("wait", v, NULL, NULL);
            free(v);
            break;
        }

        default:
            break;
    }
}

/* ── API ────────────────────────────────────────────────────── */
int generar_tac(NodoAST *raiz) {
    liberar_tac();
    scope_top      = 0;
    scope_pila[0]  = 0;
    scope_contador = 0;
    gen_sent(raiz);
    emitir("fin", NULL, NULL, NULL);
    return n_cuads;
}

Cuad*   tac_cuadruplas(void)     { return cuads;   }
TacSim* tac_simbolos(void)       { return sims;    }
int     tac_num_temporales(void) { return n_temps; }

/* Version legible de una cuadrupla (estilo tres direcciones). */
void tac_formatear(const Cuad *c, char *buf, int tam) {
    const char *op = c->op;
    if (strcmp(op, "=") == 0)
        snprintf(buf, tam, "%s = %s", c->res, c->arg1);
    else if (strcmp(op, "neg") == 0)
        snprintf(buf, tam, "%s = - %s", c->res, c->arg1);
    else if (strcmp(op, "not") == 0)
        snprintf(buf, tam, "%s = ! %s", c->res, c->arg1);
    else if (strcmp(op, "label") == 0)
        snprintf(buf, tam, "%s:", c->res);
    else if (strcmp(op, "goto") == 0)
        snprintf(buf, tam, "goto %s", c->res);
    else if (strcmp(op, "ifFalse") == 0)
        snprintf(buf, tam, "ifFalse %s goto %s", c->arg1, c->res);
    else if (strcmp(op, "imprimir") == 0)
        snprintf(buf, tam, "imprimir %s", c->arg1);
    else if (strcmp(op, "retornar") == 0)
        snprintf(buf, tam, "retornar%s%s", c->arg1 ? " " : "", c->arg1 ? c->arg1 : "");
    else if (strcmp(op, "fin") == 0)
        snprintf(buf, tam, "fin");
    else if (strcmp(op, "cls") == 0)
        snprintf(buf, tam, "limpiar");
    else if (strcmp(op, "locate") == 0)
        snprintf(buf, tam, "posicionar %s, %s", c->arg1, c->arg2);
    else if (strcmp(op, "draw") == 0)
        snprintf(buf, tam, "dibujar %s", c->arg1);
    else if (strcmp(op, "putc") == 0)
        snprintf(buf, tam, "pintar %s", c->arg1);
    else if (strcmp(op, "wait") == 0)
        snprintf(buf, tam, "esperar %s", c->arg1);
    else if (strcmp(op, "tecla") == 0)
        snprintf(buf, tam, "%s = tecla()", c->res);
    else if (strcmp(op, "rand") == 0)
        snprintf(buf, tam, "%s = aleatorio(%s)", c->res, c->arg1);
    else   /* operacion binaria */
        snprintf(buf, tam, "%s = %s %s %s", c->res, c->arg1, op, c->arg2);
}

void imprimir_tac(void) {
    printf("\n==================================================\n");
    printf("CODIGO INTERMEDIO (TRES DIRECCIONES)\n");
    printf("==================================================\n\n");
    int i = 1;
    char buf[512];
    for (Cuad *c = cuads; c; c = c->sig, i++) {
        tac_formatear(c, buf, sizeof(buf));
        if (strcmp(c->op, "label") == 0)
            printf("%4d: %s\n", i, buf);
        else
            printf("%4d:     %s\n", i, buf);
    }
    printf("\n==================================================\n");
    printf("FIN DEL CODIGO INTERMEDIO (%d cuadruplas)\n", n_cuads);
    printf("==================================================\n");
}

/* Campo seguro para el flujo delimitado: '|' se sustituye por '¦'
   (emitido como UTF-8: 0xC2 0xA6) para no romper el separador.    */
static void campo_gui(const char *s) {
    if (!s) return;
    for (const char *p = s; *p; p++) {
        if (*p == '|') { putchar(0xC2); putchar(0xA6); }
        else           { putchar(*p); }
    }
}

void emitir_tac_gui(void) {
    int i = 1;
    char buf[512];
    for (Cuad *c = cuads; c; c = c->sig, i++) {
        tac_formatear(c, buf, sizeof(buf));
        printf("%d|", i);
        campo_gui(c->op);  putchar('|');
        campo_gui(c->arg1); putchar('|');
        campo_gui(c->arg2); putchar('|');
        campo_gui(c->res);  putchar('|');
        campo_gui(buf);
        putchar('\n');
    }
}

void liberar_tac(void) {
    Cuad *c = cuads;
    while (c) {
        Cuad *s = c->sig;
        free(c->op); free(c->arg1); free(c->arg2); free(c->res); free(c);
        c = s;
    }
    cuads = cuads_tail = NULL;
    n_cuads = n_temps = n_labels = 0;

    TacSim *t = sims;
    while (t) {
        TacSim *s = t->sig;
        free(t->nombre); free(t->unico); free(t->tipo); free(t);
        t = s;
    }
    sims = sims_tail = NULL;
}
