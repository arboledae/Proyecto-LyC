#include <stdio.h>
#include "gui_salida.h"
#include "tokens.h"

/* Simbolos del lexer/parser generados por Flex y Bison. */
extern int   yylex(void);
extern int   yylineno;
extern char *yytext;
extern FILE *yyin;
extern void  yyrestart(FILE *);

/* ── @@TOKENS ─────────────────────────────────────────────────
   Recorre todo el archivo emitiendo un token por linea y luego deja
   el lexer listo para que yyparse() lo lea de nuevo desde el inicio. */
void emitir_tokens_gui(void) {
    int token;
    while ((token = yylex()) != 0)
        printf("%d|%s|%s\n", yylineno, nombre_token(token), yytext);

    /* Rebobinar el archivo y reiniciar el lexer para la fase sintactica. */
    if (yyin) {
        rewind(yyin);
        yyrestart(yyin);
    }
    yylineno = 1;
}

/* ── @@AST ────────────────────────────────────────────────────
   Igual estructura que imprimir_ast pero emitiendo "profundidad|etiqueta".
   Las etiquetas sinteticas (CONDICION, BLOQUE_SI, ...) cuentan como un
   nivel real, garantizando profundidades estrictamente monotonas. */
static void emit_nodo(NodoAST *n, int d) {
    if (!n) return;

    switch (n->tipo) {
        case NODO_PROGRAMA:
            printf("%d|PROGRAMA\n", d);
            emit_nodo(n->izq, d + 1);
            break;

        case NODO_LISTA_SENT:               /* nodo transparente */
            emit_nodo(n->izq,   d);
            emit_nodo(n->extra, d);
            break;

        case NODO_DECLARACION:
            printf("%d|DECLARACION %s %s\n", d, n->str_val, n->izq->str_val);
            if (n->der) {
                printf("%d|VALOR_INICIAL\n", d + 1);
                emit_nodo(n->der, d + 2);
            }
            break;

        case NODO_ASIGNACION:
            printf("%d|ASIGNACION %s\n", d, n->izq->str_val);
            emit_nodo(n->der, d + 1);
            break;

        case NODO_CONDICIONAL:
            printf("%d|CUANDO\n", d);
            printf("%d|CONDICION\n", d + 1);
            emit_nodo(n->izq, d + 2);
            printf("%d|BLOQUE_SI\n", d + 1);
            emit_nodo(n->der, d + 2);
            if (n->extra) {
                printf("%d|BLOQUE_SINO\n", d + 1);
                emit_nodo(n->extra, d + 2);
            }
            break;

        case NODO_CICLO:
            printf("%d|LOOP\n", d);
            printf("%d|CONDICION\n", d + 1);
            emit_nodo(n->izq, d + 2);
            printf("%d|CUERPO\n", d + 1);
            emit_nodo(n->der, d + 2);
            break;

        case NODO_IMPRESION:
            printf("%d|IMPRIMIR\n", d);
            emit_nodo(n->izq, d + 1);
            break;

        case NODO_RETORNO:
            printf("%d|RETORNAR\n", d);
            if (n->izq) emit_nodo(n->izq, d + 1);
            break;

        case NODO_OP_BINARIO:
            printf("%d|OP_BIN %s\n", d, n->str_val);
            emit_nodo(n->izq, d + 1);
            emit_nodo(n->der, d + 1);
            break;

        case NODO_OP_UNARIO:
            printf("%d|OP_UN %s\n", d, n->str_val);
            emit_nodo(n->izq, d + 1);
            break;

        case NODO_ENTERO:        printf("%d|LIT_ENTERO %d\n", d, n->int_val); break;
        case NODO_DECIMAL:       printf("%d|LIT_DECIMAL %s\n", d, n->str_val); break;
        case NODO_TEXTO:         printf("%d|LIT_TEXT %s\n", d, n->str_val); break;
        case NODO_BOOLEANO:      printf("%d|LIT_BOOL %s\n", d, n->int_val ? "verdadero" : "falso"); break;
        case NODO_IDENTIFICADOR: printf("%d|ID %s\n", d, n->str_val); break;

        /* ── Primitivas de juego ─────────────────────────────── */
        case NODO_LIMPIAR:
            printf("%d|LIMPIAR\n", d);
            break;
        case NODO_POSICIONAR:
            printf("%d|POSICIONAR\n", d);
            printf("%d|COLUMNA\n", d + 1);
            emit_nodo(n->izq, d + 2);
            printf("%d|FILA\n", d + 1);
            emit_nodo(n->der, d + 2);
            break;
        case NODO_DIBUJAR:
            printf("%d|DIBUJAR\n", d);
            emit_nodo(n->izq, d + 1);
            break;
        case NODO_PINTAR:
            printf("%d|PINTAR\n", d);
            emit_nodo(n->izq, d + 1);
            break;
        case NODO_ESPERAR:
            printf("%d|ESPERAR\n", d);
            emit_nodo(n->izq, d + 1);
            break;
        case NODO_LEER_TECLA:
            printf("%d|TECLA\n", d);
            break;
        case NODO_ALEATORIO:
            printf("%d|ALEATORIO\n", d);
            emit_nodo(n->izq, d + 1);
            break;
    }
}

void emitir_ast_gui(NodoAST *raiz) {
    emit_nodo(raiz, 0);
}
