#include <stdio.h>
#include <string.h>
#include "semantic.h"
#include "symbol_table.h"

typedef struct ContextoSemantico {
    TablaSimbolos tabla;
    int errores;
} ContextoSemantico;

static void analizar_nodo(ContextoSemantico *ctx, NodoAST *nodo);
static void analizar_bloque(ContextoSemantico *ctx, NodoAST *bloque);
static void analizar_expresion(ContextoSemantico *ctx, NodoAST *expr);

static int es_operador_relacional(const char *op) {
    return strcmp(op, ">") == 0 ||
           strcmp(op, "<") == 0 ||
           strcmp(op, ">=") == 0 ||
           strcmp(op, "<=") == 0 ||
           strcmp(op, "==") == 0 ||
           strcmp(op, "!=") == 0;
}

static int es_operador_logico(const char *op) {
    return strcmp(op, "&&") == 0 ||
           strcmp(op, "||") == 0 ||
           strcmp(op, "!") == 0;
}

static void reportar_error(ContextoSemantico *ctx, const char *mensaje, const char *nombre) {
    if (nombre) {
        printf("[ERROR SEMANTICO] %s '%s'.\n", mensaje, nombre);
    } else {
        printf("[ERROR SEMANTICO] %s.\n", mensaje);
    }
    ctx->errores++;
}

static void analizar_declaracion(ContextoSemantico *ctx, NodoAST *nodo) {
    const char *tipo = nodo->str_val;
    const char *nombre = nodo->izq ? nodo->izq->str_val : NULL;

    if (!nombre) {
        reportar_error(ctx, "Declaracion sin identificador", NULL);
        return;
    }

    if (!insertar_simbolo(&ctx->tabla, nombre, tipo)) {
        reportar_error(ctx, "Variable ya declarada en este scope", nombre);
    }

    analizar_expresion(ctx, nodo->der);
}

static void analizar_asignacion(ContextoSemantico *ctx, NodoAST *nodo) {
    const char *nombre = nodo->izq ? nodo->izq->str_val : NULL;

    if (!nombre || !buscar_simbolo(&ctx->tabla, nombre)) {
        reportar_error(ctx, "Variable no declarada", nombre);
    }

    analizar_expresion(ctx, nodo->der);
}

static void analizar_condicional(ContextoSemantico *ctx, NodoAST *nodo) {
    analizar_expresion(ctx, nodo->izq);

    entrar_scope(&ctx->tabla);
    analizar_bloque(ctx, nodo->der);
    salir_scope(&ctx->tabla);

    if (nodo->extra) {
        entrar_scope(&ctx->tabla);
        analizar_bloque(ctx, nodo->extra);
        salir_scope(&ctx->tabla);
    }
}

static void analizar_ciclo(ContextoSemantico *ctx, NodoAST *nodo) {
    analizar_expresion(ctx, nodo->izq);

    entrar_scope(&ctx->tabla);
    analizar_bloque(ctx, nodo->der);
    salir_scope(&ctx->tabla);
}

static void analizar_bloque(ContextoSemantico *ctx, NodoAST *bloque) {
    analizar_nodo(ctx, bloque);
}

static void analizar_expresion(ContextoSemantico *ctx, NodoAST *expr) {
    if (!expr) return;

    switch (expr->tipo) {
        case NODO_IDENTIFICADOR:
            if (!buscar_simbolo(&ctx->tabla, expr->str_val)) {
                reportar_error(ctx, "Variable no declarada", expr->str_val);
            }
            break;

        case NODO_OP_BINARIO:
            if (!es_operador_relacional(expr->str_val) && !es_operador_logico(expr->str_val)) {
                analizar_expresion(ctx, expr->izq);
                analizar_expresion(ctx, expr->der);
            } else {
                analizar_expresion(ctx, expr->izq);
                analizar_expresion(ctx, expr->der);
            }
            break;

        case NODO_OP_UNARIO:
            analizar_expresion(ctx, expr->izq);
            break;

        default:
            break;
    }
}

static void analizar_nodo(ContextoSemantico *ctx, NodoAST *nodo) {
    if (!nodo) return;

    switch (nodo->tipo) {
        case NODO_PROGRAMA:
            analizar_bloque(ctx, nodo->izq);
            break;

        case NODO_LISTA_SENT:
            analizar_nodo(ctx, nodo->izq);
            analizar_nodo(ctx, nodo->extra);
            break;

        case NODO_DECLARACION:
            analizar_declaracion(ctx, nodo);
            break;

        case NODO_ASIGNACION:
            analizar_asignacion(ctx, nodo);
            break;

        case NODO_CONDICIONAL:
            analizar_condicional(ctx, nodo);
            break;

        case NODO_CICLO:
            analizar_ciclo(ctx, nodo);
            break;

        case NODO_IMPRESION:
        case NODO_RETORNO:
            analizar_expresion(ctx, nodo->izq);
            break;

        default:
            analizar_expresion(ctx, nodo);
            break;
    }
}

int analizar_semantica(NodoAST *raiz) {
    ContextoSemantico ctx;
    inicializar_tabla(&ctx.tabla);
    ctx.errores = 0;

    printf("\n==================================================\n");
    printf("ANALISIS SEMANTICO\n");
    printf("==================================================\n\n");

    analizar_nodo(&ctx, raiz);
    imprimir_tabla(&ctx.tabla);

    if (ctx.errores == 0) {
        printf("\nAnalisis semantico completado sin errores.\n");
    } else {
        printf("\nAnalisis semantico concluido con %d error(es).\n", ctx.errores);
    }

    liberar_tabla(&ctx.tabla);
    return ctx.errores == 0;
}
