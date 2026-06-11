#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;

const char* obtener_nombre_token(int token) {
    switch (token) {
        case KW_INICIO:       return "KW_INICIO";
        case KW_SINO:         return "KW_SINO";
        case KW_CUANDO:       return "KW_CUANDO";
        case KW_LOOP:         return "KW_LOOP";
        case KW_IMPRIMIR:     return "KW_IMPRIMIR";
        case KW_RETORNAR:     return "KW_RETORNAR";
        case KW_NUM:          return "KW_NUM";
        case KW_DEC:          return "KW_DEC";
        case KW_TEXT:         return "KW_TEXT";
        case LIT_BOOLEANO:    return "LIT_BOOLEANO";
        case LIT_ENTERO:      return "LIT_ENTERO";
        case LIT_DECIMAL:     return "LIT_DECIMAL";
        case LIT_TEXT:        return "LIT_TEXT";
        case ID:              return "ID";
        case OP_ASIG:         return "OP_ASIG";
        case OP_SUMA:         return "OP_SUMA";
        case OP_RESTA:        return "OP_RESTA";
        case OP_MULT:         return "OP_MULT";
        case OP_DIV:          return "OP_DIV";
        case OP_MOD:          return "OP_MOD";
        case OP_IGUAL:        return "OP_IGUAL";
        case OP_DIFERENTE:    return "OP_DIFERENTE";
        case OP_MAYOR:        return "OP_MAYOR";
        case OP_MENOR:        return "OP_MENOR";
        case OP_MAYOR_IGUAL:  return "OP_MAYOR_IGUAL";
        case OP_MENOR_IGUAL:  return "OP_MENOR_IGUAL";
        case OP_AND:          return "OP_AND";
        case OP_OR:           return "OP_OR";
        case OP_NOT:          return "OP_NOT";
        case PUNTO_COMA:      return "PUNTO_COMA";
        case COMA:            return "COMA";
        case PARENTESIS_ABRIR:  return "PARENTESIS_ABRIR";
        case PARENTESIS_CERRAR: return "PARENTESIS_CERRAR";
        case LLAVE_ABRIR:       return "LLAVE_ABRIR";
        case LLAVE_CERRAR:      return "LLAVE_CERRAR";
        default:              return "TOKEN_DESCONOCIDO";
    }
}

int main(int argc, char** argv) {
    if (argc > 1) {
        FILE* file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "Error: No se pudo abrir el archivo '%s'\n", argv[1]);
            return 1;
        }
        yyin = file;
    } else {
        printf("Leyendo desde la entrada estándar...\n");
    }

    int token;
    printf("--- INICIANDO ANALISIS LEXICO DEL ARCHIVO ---\n\n");
    while ((token = yylex()) != 0) {
        printf("[Linea %d] Token: %s (%d) | Lexema: \"%s\"\n", yylineno, obtener_nombre_token(token), token, yytext);
    }
    printf("\n--- ANALISIS LEXICO FINALIZADO ---\n");

    if (argc > 1 && yyin) {
        fclose(yyin);
    }
    return 0;
}
