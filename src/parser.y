%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;

void yyerror(const char *s);
%}

%token KW_INICIO KW_SINO KW_CUANDO KW_LOOP KW_IMPRIMIR KW_RETORNAR
%token KW_NUM KW_DEC KW_TEXT
%token LIT_BOOLEANO LIT_ENTERO LIT_DECIMAL LIT_TEXT ID
%token OP_ASIG
%token OP_SUMA OP_RESTA OP_MULT OP_DIV OP_MOD
%token OP_IGUAL OP_DIFERENTE OP_MAYOR OP_MENOR OP_MAYOR_IGUAL OP_MENOR_IGUAL
%token OP_AND OP_OR OP_NOT
%token PUNTO_COMA COMA PARENTESIS_ABRIR PARENTESIS_CERRAR LLAVE_ABRIR LLAVE_CERRAR

/* Precedencia de operadores en Bison (de menor a mayor) */
%left OP_OR
%left OP_AND
%left OP_IGUAL OP_DIFERENTE OP_MAYOR OP_MENOR OP_MAYOR_IGUAL OP_MENOR_IGUAL
%left OP_SUMA OP_RESTA
%left OP_MULT OP_DIV OP_MOD
%right OP_NOT

%start programa

%%

programa
    : KW_INICIO LLAVE_ABRIR sentencias LLAVE_CERRAR
    ;

sentencias
    : sentencia sentencias
    | /* epsilon */
    ;

sentencia
    : declaracion PUNTO_COMA
    | asignacion PUNTO_COMA
    | condicional
    | ciclo
    | impresion PUNTO_COMA
    | retorno PUNTO_COMA
    ;

declaracion
    : tipo ID OP_ASIG expresion
    | tipo ID
    ;

asignacion
    : ID OP_ASIG expresion
    ;

tipo
    : KW_NUM
    | KW_DEC
    | KW_TEXT
    ;

condicional
    : KW_CUANDO PARENTESIS_ABRIR condicion PARENTESIS_CERRAR LLAVE_ABRIR sentencias LLAVE_CERRAR
    | KW_CUANDO PARENTESIS_ABRIR condicion PARENTESIS_CERRAR LLAVE_ABRIR sentencias LLAVE_CERRAR KW_SINO LLAVE_ABRIR sentencias LLAVE_CERRAR
    ;

ciclo
    : KW_LOOP PARENTESIS_ABRIR condicion PARENTESIS_CERRAR LLAVE_ABRIR sentencias LLAVE_CERRAR
    ;

impresion
    : KW_IMPRIMIR PARENTESIS_ABRIR expresion PARENTESIS_CERRAR
    ;

retorno
    : KW_RETORNAR expresion
    | KW_RETORNAR
    ;

condicion
    : expresion operador_rel expresion
    | OP_NOT condicion
    | condicion OP_AND condicion
    | condicion OP_OR condicion
    | LIT_BOOLEANO
    ;

operador_rel
    : OP_MAYOR
    | OP_MENOR
    | OP_MAYOR_IGUAL
    | OP_MENOR_IGUAL
    | OP_IGUAL
    | OP_DIFERENTE
    ;

expresion
    : expresion OP_SUMA expresion
    | expresion OP_RESTA expresion
    | expresion OP_MULT expresion
    | expresion OP_DIV expresion
    | expresion OP_MOD expresion
    | PARENTESIS_ABRIR expresion PARENTESIS_CERRAR
    | LIT_ENTERO
    | LIT_DECIMAL
    | LIT_TEXT
    | LIT_BOOLEANO
    | ID
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "[ERROR SINTACTICO] Linea %d: %s\n", yylineno, s);
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
        printf("Leyendo desde la entrada estandar...\n");
    }
    
    int result = yyparse();
    
    if (result == 0) {
        printf("ANALISIS SINTACTICO COMPLETADO EXITOSAMENTE\n");
    }
    
    if (argc > 1 && yyin) {
        fclose(yyin);
    }
    
    return result;
}
