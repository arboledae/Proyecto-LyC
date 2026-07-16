#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* yylineno es definido por Flex (%option yylineno) y usado por Bison.
   Solo se referencia cuando ast.c se enlaza con el compilador completo. */
extern int yylineno;

/* ── Constructores ──────────────────────────────────────────── */

NodoAST* nuevo_nodo(TipoNodo tipo) {
    NodoAST *n = (NodoAST*)malloc(sizeof(NodoAST));
    n->tipo    = tipo;
    n->str_val = NULL;
    n->int_val = 0;
    n->linea   = yylineno;     /* linea donde se construye el nodo */
    n->izq     = NULL;
    n->der     = NULL;
    n->extra   = NULL;
    return n;
}

NodoAST* nodo_entero(int val) {
    NodoAST *n = nuevo_nodo(NODO_ENTERO);
    n->int_val = val;
    return n;
}

NodoAST* nodo_decimal(char *val) {
    NodoAST *n = nuevo_nodo(NODO_DECIMAL);
    n->str_val = val;
    return n;
}

NodoAST* nodo_texto(char *val) {
    NodoAST *n = nuevo_nodo(NODO_TEXTO);
    n->str_val = val;
    return n;
}

NodoAST* nodo_booleano(int val) {
    NodoAST *n = nuevo_nodo(NODO_BOOLEANO);
    n->int_val = val;
    return n;
}

NodoAST* nodo_id(char *nombre) {
    NodoAST *n = nuevo_nodo(NODO_IDENTIFICADOR);
    n->str_val = strdup(nombre);
    return n;
}

NodoAST* nodo_op_binario(char *op, NodoAST *izq, NodoAST *der) {
    NodoAST *n = nuevo_nodo(NODO_OP_BINARIO);
    n->str_val = strdup(op);
    n->izq     = izq;
    n->der     = der;
    return n;
}

NodoAST* nodo_op_unario(char *op, NodoAST *operando) {
    NodoAST *n = nuevo_nodo(NODO_OP_UNARIO);
    n->str_val = strdup(op);
    n->izq     = operando;
    return n;
}

NodoAST* nodo_declaracion(char *tipo, char *nombre, NodoAST *expr) {
    NodoAST *n = nuevo_nodo(NODO_DECLARACION);
    n->str_val = strdup(tipo);
    n->izq     = nodo_id(nombre);
    n->der     = expr;
    return n;
}

NodoAST* nodo_asignacion(char *nombre, NodoAST *expr) {
    NodoAST *n = nuevo_nodo(NODO_ASIGNACION);
    n->izq     = nodo_id(nombre);
    n->der     = expr;
    return n;
}

NodoAST* nodo_condicional(NodoAST *cond, NodoAST *bloque_si, NodoAST *bloque_sino) {
    NodoAST *n = nuevo_nodo(NODO_CONDICIONAL);
    n->izq   = cond;
    n->der   = bloque_si;
    n->extra = bloque_sino;
    return n;
}

NodoAST* nodo_ciclo(NodoAST *cond, NodoAST *cuerpo) {
    NodoAST *n = nuevo_nodo(NODO_CICLO);
    n->izq = cond;
    n->der = cuerpo;
    return n;
}

NodoAST* nodo_impresion(NodoAST *expr) {
    NodoAST *n = nuevo_nodo(NODO_IMPRESION);
    n->izq = expr;
    return n;
}

NodoAST* nodo_retorno(NodoAST *expr) {
    NodoAST *n = nuevo_nodo(NODO_RETORNO);
    n->izq = expr;
    return n;
}

NodoAST* nodo_lista(NodoAST *sentencia, NodoAST *siguiente) {
    NodoAST *n = nuevo_nodo(NODO_LISTA_SENT);
    n->izq   = sentencia;
    n->extra = siguiente;
    return n;
}

/* ── Constructores de las primitivas de juego ───────────────── */
NodoAST* nodo_limpiar(void) {
    return nuevo_nodo(NODO_LIMPIAR);
}

NodoAST* nodo_posicionar(NodoAST *x, NodoAST *y) {
    NodoAST *n = nuevo_nodo(NODO_POSICIONAR);
    n->izq = x;   /* columna */
    n->der = y;   /* fila    */
    return n;
}

NodoAST* nodo_dibujar(NodoAST *expr) {
    NodoAST *n = nuevo_nodo(NODO_DIBUJAR);
    n->izq = expr;
    return n;
}

NodoAST* nodo_pintar(NodoAST *expr) {
    NodoAST *n = nuevo_nodo(NODO_PINTAR);
    n->izq = expr;
    return n;
}

NodoAST* nodo_esperar(NodoAST *expr) {
    NodoAST *n = nuevo_nodo(NODO_ESPERAR);
    n->izq = expr;
    return n;
}

NodoAST* nodo_leer_tecla(void) {
    return nuevo_nodo(NODO_LEER_TECLA);
}

NodoAST* nodo_aleatorio(NodoAST *expr) {
    NodoAST *n = nuevo_nodo(NODO_ALEATORIO);
    n->izq = expr;
    return n;
}

NodoAST* nodo_sprites(void) {
    return nuevo_nodo(NODO_SPRITES);
}

/* ── Impresión ──────────────────────────────────────────────── */

static void sangria(int nivel) {
    for (int i = 0; i < nivel; i++) printf("  ");
}

void imprimir_ast(NodoAST *nodo, int nivel) {
    if (!nodo) return;

    /* El nodo lista es transparente: no imprime ni consume un nivel, asi
       cada sentencia del bloque queda a la misma profundidad. El resto de
       nodos imprime su sangria segun 'nivel' y sus hijos a nivel+1, de modo
       que la profundidad crece de forma estrictamente monotona. */
    if (nodo->tipo == NODO_LISTA_SENT) {
        imprimir_ast(nodo->izq,   nivel);
        imprimir_ast(nodo->extra, nivel);
        return;
    }

    sangria(nivel);

    switch (nodo->tipo) {
        case NODO_PROGRAMA:
            printf("[PROGRAMA]\n");
            imprimir_ast(nodo->izq, nivel + 1);
            break;

        case NODO_LISTA_SENT:   /* tratado arriba */
            break;

        case NODO_DECLARACION:
            printf("[DECLARACION] tipo=%s  var=%s\n",
                   nodo->str_val, nodo->izq->str_val);
            if (nodo->der) {
                sangria(nivel + 1);
                printf("[VALOR INICIAL]\n");
                imprimir_ast(nodo->der, nivel + 2);
            }
            break;

        case NODO_ASIGNACION:
            printf("[ASIGNACION] var=%s\n", nodo->izq->str_val);
            imprimir_ast(nodo->der, nivel + 1);
            break;

        case NODO_CONDICIONAL:
            printf("[CUANDO]\n");
            sangria(nivel + 1);
            printf("[CONDICION]\n");
            imprimir_ast(nodo->izq, nivel + 2);
            sangria(nivel + 1);
            printf("[BLOQUE_SI]\n");
            imprimir_ast(nodo->der, nivel + 2);
            if (nodo->extra) {
                sangria(nivel + 1);
                printf("[BLOQUE_SINO]\n");
                imprimir_ast(nodo->extra, nivel + 2);
            }
            break;

        case NODO_CICLO:
            printf("[LOOP]\n");
            sangria(nivel + 1);
            printf("[CONDICION]\n");
            imprimir_ast(nodo->izq, nivel + 2);
            sangria(nivel + 1);
            printf("[CUERPO]\n");
            imprimir_ast(nodo->der, nivel + 2);
            break;

        case NODO_IMPRESION:
            printf("[IMPRIMIR]\n");
            imprimir_ast(nodo->izq, nivel + 1);
            break;

        case NODO_RETORNO:
            printf("[RETORNAR]\n");
            if (nodo->izq) imprimir_ast(nodo->izq, nivel + 1);
            break;

        case NODO_OP_BINARIO:
            printf("[OP_BIN] %s\n", nodo->str_val);
            imprimir_ast(nodo->izq, nivel + 1);
            imprimir_ast(nodo->der, nivel + 1);
            break;

        case NODO_OP_UNARIO:
            printf("[OP_UN] %s\n", nodo->str_val);
            imprimir_ast(nodo->izq, nivel + 1);
            break;

        case NODO_ENTERO:
            printf("[LIT_ENTERO] %d\n", nodo->int_val);
            break;

        case NODO_DECIMAL:
            printf("[LIT_DECIMAL] %s\n", nodo->str_val);
            break;

        case NODO_TEXTO:
            printf("[LIT_TEXT] %s\n", nodo->str_val);
            break;

        case NODO_BOOLEANO:
            printf("[LIT_BOOL] %s\n", nodo->int_val ? "verdadero" : "falso");
            break;

        case NODO_IDENTIFICADOR:
            printf("[ID] %s\n", nodo->str_val);
            break;

        case NODO_LIMPIAR:
            printf("[LIMPIAR]\n");
            break;

        case NODO_POSICIONAR:
            printf("[POSICIONAR]\n");
            sangria(nivel + 1);
            printf("[COLUMNA]\n");
            imprimir_ast(nodo->izq, nivel + 2);
            sangria(nivel + 1);
            printf("[FILA]\n");
            imprimir_ast(nodo->der, nivel + 2);
            break;

        case NODO_DIBUJAR:
            printf("[DIBUJAR]\n");
            imprimir_ast(nodo->izq, nivel + 1);
            break;

        case NODO_PINTAR:
            printf("[PINTAR]\n");
            imprimir_ast(nodo->izq, nivel + 1);
            break;

        case NODO_ESPERAR:
            printf("[ESPERAR]\n");
            imprimir_ast(nodo->izq, nivel + 1);
            break;

        case NODO_LEER_TECLA:
            printf("[TECLA]\n");
            break;

        case NODO_ALEATORIO:
            printf("[ALEATORIO]\n");
            imprimir_ast(nodo->izq, nivel + 1);
            break;

        case NODO_SPRITES:
            printf("[SPRITES]\n");
            break;
    }
}

/* ── Liberación de memoria ──────────────────────────────────── */

void liberar_ast(NodoAST *nodo) {
    if (!nodo) return;
    liberar_ast(nodo->izq);
    liberar_ast(nodo->der);
    liberar_ast(nodo->extra);
    if (nodo->str_val) free(nodo->str_val);
    free(nodo);
}
