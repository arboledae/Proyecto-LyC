#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"
#include "tokens.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;

/* Bandera de traza del lexer (definida en lex.yy.c). Este driver imprime
   los tokens por su cuenta, asi que apaga las trazas internas del lexer
   para no duplicar la salida. */
extern int lexer_verbose;

/* yylval lo define normalmente Bison (parser.tab.c). Para compilar el
   analizador lexico de forma independiente lo proveemos aqui. */
YYSTYPE yylval;

int main(int argc, char** argv) {
    lexer_verbose = 0;

    if (argc > 1) {
        FILE* file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "Error: No se pudo abrir el archivo '%s'\n", argv[1]);
            return 1;
        }
        yyin = file;
    } else {
        printf("Leyendo desde la entrada estandar...\n");
    }

    int token;
    printf("--- INICIANDO ANALISIS LEXICO DEL ARCHIVO ---\n\n");
    while ((token = yylex()) != 0) {
        printf("[Linea %d] Token: %s (%d) | Lexema: \"%s\"\n",
               yylineno, nombre_token(token), token, yytext);
    }
    printf("\n--- ANALISIS LEXICO FINALIZADO ---\n");

    if (argc > 1 && yyin) {
        fclose(yyin);
    }
    return 0;
}
