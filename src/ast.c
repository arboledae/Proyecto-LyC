#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* ── Constructores ──────────────────────────────────────────── */

NodoAST* nuevo_nodo(TipoNodo tipo) {
    NodoAST *n = (NodoAST*)malloc(sizeof(NodoAST));
    n->tipo    = tipo;
    n->str_val = NULL;
    n->int_val = 0;
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

/* ── Impresión ──────────────────────────────────────────────── */

void imprimir_ast(NodoAST *nodo, int nivel) {
    if (!nodo) return;

    for (int i = 0; i < nivel; i++) printf("  ");

    switch (nodo->tipo) {
        case NODO_PROGRAMA:
            printf("[PROGRAMA]\n");
            imprimir_ast(nodo->izq, nivel + 1);
            break;

        case NODO_LISTA_SENT:
            imprimir_ast(nodo->izq,   nivel);
            imprimir_ast(nodo->extra, nivel);
            break;

        case NODO_DECLARACION:
            printf("[DECLARACION] tipo=%s  var=%s\n",
                   nodo->str_val, nodo->izq->str_val);
            if (nodo->der) {
                for (int i = 0; i < nivel + 1; i++) printf("  ");
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
            for (int i = 0; i < nivel + 1; i++) printf("  ");
            printf("[CONDICION]\n");
            imprimir_ast(nodo->izq, nivel + 2);
            for (int i = 0; i < nivel + 1; i++) printf("  ");
            printf("[BLOQUE_SI]\n");
            imprimir_ast(nodo->der, nivel + 2);
            if (nodo->extra) {
                for (int i = 0; i < nivel + 1; i++) printf("  ");
                printf("[BLOQUE_SINO]\n");
                imprimir_ast(nodo->extra, nivel + 2);
            }
            break;

        case NODO_CICLO:
            printf("[LOOP]\n");
            for (int i = 0; i < nivel + 1; i++) printf("  ");
            printf("[CONDICION]\n");
            imprimir_ast(nodo->izq, nivel + 2);
            for (int i = 0; i < nivel + 1; i++) printf("  ");
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
