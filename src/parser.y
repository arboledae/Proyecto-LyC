%code requires {
    #include "ast.h"
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylex();
extern int yyparse();
extern FILE *yyin;
extern int yylineno;

void yyerror(const char *s);

int ends_with_g5z80(const char *filename) {
    int len = strlen(filename);
    if (len < 6) return 0;
    const char *ext = filename + len - 6;
    return (ext[0] == '.' &&
            (ext[1] == 'g' || ext[1] == 'G') &&
            (ext[2] == '5') &&
            (ext[3] == 'z' || ext[3] == 'Z') &&
            (ext[4] == '8') &&
            (ext[5] == '0'));
}
%}

/* ----- DECLARACION DE TIPOS EN YYLVAL ----- */
%union {
    int            entero;
    char          *str;
    struct NodoAST *nodo;
}

/* Tokens sin valor semantico */
%token KW_INICIO KW_CUANDO KW_SINO KW_LOOP
%token KW_NUM KW_DEC KW_TEXT
%token KW_IMPRIMIR KW_RETORNAR
%token OP_ASIGNACION OP_SUMA OP_RESTA OP_MULT OP_DIV OP_MOD
%token OP_IGUAL OP_DIFERENTE OP_MAYOR OP_MENOR
%token OP_MAYOR_IGUAL OP_MENOR_IGUAL
%token OP_AND OP_OR OP_NOT
%token DELIM_PAR_IZQ DELIM_PAR_DER
%token DELIM_LLAVE_IZQ DELIM_LLAVE_DER DELIM_PUNTO_COMA
%token TOKEN_COMENT TOKEN_ERROR

/* Tokens con valor semantico */
%token <str>    IDENTIFICADOR
%token <str>    LIT_TEXT
%token <str>    LIT_DECIMAL
%token <entero> LIT_ENTERO
%token <entero> LIT_BOOLEANO

/* Tipo de retorno de las reglas gramaticales que construyen nodos */
%type <nodo> sentencias sentencia declaracion asignacion
%type <nodo> condicional ciclo impresion retorno
%type <nodo> condicion expresion
%type <str>  tipo operador_rel

/* Precedencia de operadores (de menor a mayor) */
%left  OP_OR
%left  OP_AND
%left  OP_IGUAL OP_DIFERENTE OP_MAYOR OP_MENOR OP_MAYOR_IGUAL OP_MENOR_IGUAL
%left  OP_SUMA OP_RESTA
%left  OP_MULT OP_DIV OP_MOD
%right OP_NOT
%right UMINUS

%start programa

%%

/* ------ REGLAS GRAMATICALES ----- */

programa:
    KW_INICIO DELIM_LLAVE_IZQ sentencias DELIM_LLAVE_DER
    {
        NodoAST *raiz = nuevo_nodo(NODO_PROGRAMA);
        raiz->izq = $3;

        printf("\n==================================================\n");
        printf("ARBOL DE SINTAXIS ABSTRACTA (AST)\n");
        printf("==================================================\n\n");
        imprimir_ast(raiz, 0);
        printf("\n==================================================\n");
        printf("FIN DEL AST\n");
        printf("==================================================\n");

        liberar_ast(raiz);
    }
    ;

sentencias:
      sentencia sentencias
    {
        $$ = nodo_lista($1, $2);
    }
    | /* vacio */
    {
        $$ = NULL;
    }
    ;

sentencia:
      declaracion DELIM_PUNTO_COMA  { $$ = $1; }
    | asignacion  DELIM_PUNTO_COMA  { $$ = $1; }
    | condicional                   { $$ = $1; }
    | ciclo                         { $$ = $1; }
    | impresion   DELIM_PUNTO_COMA  { $$ = $1; }
    | retorno     DELIM_PUNTO_COMA  { $$ = $1; }
    ;

declaracion:
      tipo IDENTIFICADOR OP_ASIGNACION expresion
    {
        $$ = nodo_declaracion($1, $2, $4);
    }
    | tipo IDENTIFICADOR
    {
        $$ = nodo_declaracion($1, $2, NULL);
    }
    ;

tipo:
      KW_NUM  { $$ = "num";  }
    | KW_DEC  { $$ = "dec";  }
    | KW_TEXT { $$ = "text"; }
    ;

asignacion:
      IDENTIFICADOR OP_ASIGNACION expresion
    {
        $$ = nodo_asignacion($1, $3);
    }
    ;

condicional:
      KW_CUANDO DELIM_PAR_IZQ condicion DELIM_PAR_DER
      DELIM_LLAVE_IZQ sentencias DELIM_LLAVE_DER
    {
        $$ = nodo_condicional($3, $6, NULL);
    }
    | KW_CUANDO DELIM_PAR_IZQ condicion DELIM_PAR_DER
      DELIM_LLAVE_IZQ sentencias DELIM_LLAVE_DER
      KW_SINO DELIM_LLAVE_IZQ sentencias DELIM_LLAVE_DER
    {
        $$ = nodo_condicional($3, $6, $10);
    }
    ;

ciclo:
      KW_LOOP DELIM_PAR_IZQ condicion DELIM_PAR_DER
      DELIM_LLAVE_IZQ sentencias DELIM_LLAVE_DER
    {
        $$ = nodo_ciclo($3, $6);
    }
    ;

impresion:
      KW_IMPRIMIR DELIM_PAR_IZQ expresion DELIM_PAR_DER
    {
        $$ = nodo_impresion($3);
    }
    ;

retorno:
      KW_RETORNAR expresion
    {
        $$ = nodo_retorno($2);
    }
    | KW_RETORNAR
    {
        $$ = nodo_retorno(NULL);
    }
    ;

condicion:
      expresion operador_rel expresion
    {
        $$ = nodo_op_binario($2, $1, $3);
    }
    | OP_NOT condicion
    {
        $$ = nodo_op_unario("!", $2);
    }
    | condicion OP_AND condicion
    {
        $$ = nodo_op_binario("&&", $1, $3);
    }
    | condicion OP_OR condicion
    {
        $$ = nodo_op_binario("||", $1, $3);
    }
    | LIT_BOOLEANO
    {
        $$ = nodo_booleano($1);
    }
    ;

operador_rel:
      OP_MAYOR       { $$ = ">"; }
    | OP_MENOR       { $$ = "<"; }
    | OP_MAYOR_IGUAL { $$ = ">="; }
    | OP_MENOR_IGUAL { $$ = "<="; }
    | OP_IGUAL       { $$ = "=="; }
    | OP_DIFERENTE   { $$ = "!="; }
    ;

expresion:
      expresion OP_SUMA   expresion            { $$ = nodo_op_binario("+",  $1, $3); }
    | expresion OP_RESTA  expresion            { $$ = nodo_op_binario("-",  $1, $3); }
    | expresion OP_MULT   expresion            { $$ = nodo_op_binario("*",  $1, $3); }
    | expresion OP_DIV    expresion            { $$ = nodo_op_binario("/",  $1, $3); }
    | expresion OP_MOD    expresion            { $$ = nodo_op_binario("%",  $1, $3); }
    | OP_RESTA expresion %prec UMINUS          { $$ = nodo_op_unario("-",   $2);     }
    | DELIM_PAR_IZQ expresion DELIM_PAR_DER   { $$ = $2; }
    | LIT_ENTERO                               { $$ = nodo_entero($1);    }
    | LIT_DECIMAL                              { $$ = nodo_decimal($1);   }
    | LIT_TEXT                                 { $$ = nodo_texto($1);     }
    | LIT_BOOLEANO                             { $$ = nodo_booleano($1);  }
    | IDENTIFICADOR                            { $$ = nodo_id($1);        }
    ;

%%

/* ----- FUNCIONES DE SOPORTE ------ */

void yyerror(const char *s) {
    fprintf(stderr, "\n[ERROR SINTACTICO] Linea %d: %s\n\n", yylineno, s);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        if (!ends_with_g5z80(argv[1])) {
            fprintf(stderr, "\n[ERROR DE EXTENSION] El compilador solo acepta archivos con extension '.g5z80'.\n");
            fprintf(stderr, "Ejemplo de uso: .\\compilador.exe ejemplo1.g5z80\n\n");
            return 1;
        }

        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror(argv[1]);
            return 1;
        }
        printf("\n==================================================\n");
        printf("Iniciando analisis de: %s\n", argv[1]);
        printf("==================================================\n\n");
    } else {
        printf("Por favor ingresa un archivo de texto con extension '.g5z80'.\n");
        printf("Ejemplo: .\\compilador.exe ejemplo1.g5z80\n");
        return 1;
    }

    if (yyparse() == 0) {
        printf("\n==================================================\n");
        printf("ANALISIS SINTACTICO Y AST COMPLETADOS EXITOSAMENTE\n");
        printf("==================================================\n");
    } else {
        printf("\n==================================================\n");
        printf("ANALISIS CONCLUIDO CON ERRORES SINTACTICOS\n");
        printf("==================================================\n");
    }

    fclose(yyin);
    return 0;
}
