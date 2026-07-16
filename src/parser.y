%code requires {
    #include "ast.h"
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "simbolos.h"
#include "semantico.h"
#include "gui_salida.h"
#include "codigo_intermedio.h"
#include "generador_z80.h"

extern int yylex();
extern int yyparse();
extern FILE *yyin;
extern int yylineno;

void yyerror(const char *s);

/* ── Modo de salida ──────────────────────────────────────────
   modo_gui = 1  -> el compilador emite el formato delimitado (@@...)
                    que consume la interfaz grafica (sin JSON en C).
   modo_gui = 0  -> salida legible para consola (comportamiento normal). */
int modo_gui = 0;

/* lexer_verbose lo define el lexer (lex.yy.c). En modo --gui se apaga
   para que el flujo solo contenga las secciones @@. */
extern int lexer_verbose;

/* Estado del error sintactico capturado (para el modo --gui). */
static int  hubo_error_sint = 0;
static int  linea_error_sint = 0;
static char msg_error_sint[256] = "";

/* Resultado del semantico capturado antes de liberar las estructuras. */
static int sem_con_errores = 0;

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
%token KW_LIMPIAR KW_POSICIONAR KW_DIBUJAR KW_PINTAR KW_ESPERAR
%token KW_TECLA KW_ALEATORIO
%token OP_ASIGNACION OP_SUMA OP_RESTA OP_MULT OP_DIV OP_MOD
%token OP_IGUAL OP_DIFERENTE OP_MAYOR OP_MENOR
%token OP_MAYOR_IGUAL OP_MENOR_IGUAL
%token OP_AND OP_OR OP_NOT
%token DELIM_PAR_IZQ DELIM_PAR_DER
%token DELIM_LLAVE_IZQ DELIM_LLAVE_DER DELIM_PUNTO_COMA DELIM_COMA
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
%type <nodo> limpiar posicionar dibujar pintar esperar
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

/* 1 conflicto reduce/reduce esperado e inofensivo: dentro de un
   parentesis, un LIT_BOOLEANO aislado (p.ej. "(verdadero)") podria
   reducirse como "condicion" (regla 26, parentesis de condicion) o
   como "expresion" (que tambien acepta LIT_BOOLEANO like factor).
   Bison resuelve a favor de la regla declarada primero (condicion),
   que es la interpretacion sensata: un booleano solo entre parentesis
   siempre se usa como condicion completa, nunca como el operando de
   una comparacion relacional. (Bison no soporta %expect-rr fuera de
   parsers GLR, asi que el conflicto solo queda documentado aqui.) */

%%

/* ------ REGLAS GRAMATICALES ----- */

programa:
    KW_INICIO DELIM_LLAVE_IZQ sentencias DELIM_LLAVE_DER
    {
        NodoAST *raiz = nuevo_nodo(NODO_PROGRAMA);
        raiz->izq = $3;

        if (modo_gui) {
            /* Salida delimitada para la interfaz grafica. */
            printf("@@AST\n");
            emitir_ast_gui(raiz);

            analizar_semantico(raiz);
            sem_con_errores = hay_errores_sem();

            printf("@@SIMBOLOS\n");
            emitir_simbolos_gui();
            printf("@@ERRORES\n");
            emitir_errores_gui();

            /* Fases 4 y 5: codigo intermedio y Z80 (solo sin errores). */
            printf("@@INTERMEDIO\n");
            if (!sem_con_errores) {
                generar_tac(raiz);
                emitir_tac_gui();
            }
            printf("@@Z80\n");
            if (!sem_con_errores) {
                emitir_z80_gui();
                liberar_tac();
            }
        } else {
            /* Salida legible para consola. */
            printf("\n==================================================\n");
            printf("ARBOL DE SINTAXIS ABSTRACTA (AST)\n");
            printf("==================================================\n\n");
            imprimir_ast(raiz, 0);
            printf("\n==================================================\n");
            printf("FIN DEL AST\n");
            printf("==================================================\n");

            /* Analisis semantico: tabla de simbolos + verificacion de tipos */
            analizar_semantico(raiz);
            imprimir_tabla();
            imprimir_errores_sem();

            /* Fases 4 y 5: codigo intermedio (TAC) y ensamblador Z80 */
            if (!hay_errores_sem()) {
                generar_tac(raiz);
                imprimir_tac();
                imprimir_z80();
                liberar_tac();
            }
        }

        liberar_errores_sem();
        liberar_tabla();
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
    | limpiar     DELIM_PUNTO_COMA  { $$ = $1; }
    | posicionar  DELIM_PUNTO_COMA  { $$ = $1; }
    | dibujar     DELIM_PUNTO_COMA  { $$ = $1; }
    | pintar      DELIM_PUNTO_COMA  { $$ = $1; }
    | esperar     DELIM_PUNTO_COMA  { $$ = $1; }
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

/* ── Primitivas de juego (Amstrad CPC) ─────────────────────── */
limpiar:
      KW_LIMPIAR DELIM_PAR_IZQ DELIM_PAR_DER
    {
        $$ = nodo_limpiar();
    }
    ;

posicionar:
      KW_POSICIONAR DELIM_PAR_IZQ expresion DELIM_COMA expresion DELIM_PAR_DER
    {
        $$ = nodo_posicionar($3, $5);
    }
    ;

dibujar:
      KW_DIBUJAR DELIM_PAR_IZQ expresion DELIM_PAR_DER
    {
        $$ = nodo_dibujar($3);
    }
    ;

pintar:
      KW_PINTAR DELIM_PAR_IZQ expresion DELIM_PAR_DER
    {
        $$ = nodo_pintar($3);
    }
    ;

esperar:
      KW_ESPERAR DELIM_PAR_IZQ expresion DELIM_PAR_DER
    {
        $$ = nodo_esperar($3);
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
    | DELIM_PAR_IZQ condicion DELIM_PAR_DER
    {
        $$ = $2;
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
    | KW_TECLA DELIM_PAR_IZQ DELIM_PAR_DER     { $$ = nodo_leer_tecla();  }
    | KW_ALEATORIO DELIM_PAR_IZQ expresion DELIM_PAR_DER
                                               { $$ = nodo_aleatorio($3); }
    ;

%%

/* ----- FUNCIONES DE SOPORTE ------ */

void yyerror(const char *s) {
    if (modo_gui) {
        /* En modo grafico se captura el error para emitirlo en @@ERRORES. */
        hubo_error_sint  = 1;
        linea_error_sint = yylineno;
        snprintf(msg_error_sint, sizeof(msg_error_sint), "%s", s);
        return;
    }
    fprintf(stderr, "\n[ERROR SINTACTICO] Linea %d: %s\n\n", yylineno, s);
}

/* ── Flujo del compilador en modo grafico (--gui) ─────────────
   Emite, en una sola pasada, las secciones que consume la GUI:
     @@TOKENS / @@AST / @@SIMBOLOS / @@ERRORES / @@ESTADO / @@END
   Las tres centrales las produce la regla 'programa'; aqui se anaden
   los tokens (pre-pasada lexica) y el estado global de cada fase. */
static int correr_gui(void) {
    printf("@@TOKENS\n");
    emitir_tokens_gui();          /* recorre el archivo y lo rebobina */

    int ok = (yyparse() == 0);    /* la regla 'programa' emite @@AST.. si ok */

    if (!ok) {
        /* Con error sintactico la regla 'programa' no llega a ejecutarse:
           se emiten las secciones vacias y el error capturado. */
        printf("@@AST\n@@SIMBOLOS\n@@ERRORES\n");
        if (hubo_error_sint)
            printf("sintactico|%d|%s\n", linea_error_sint, msg_error_sint);
        else
            printf("sintactico|%d|error de sintaxis\n", yylineno);
        printf("@@INTERMEDIO\n@@Z80\n");
    }

    printf("@@ESTADO\n");
    printf("sintactico|%s\n", ok ? "ok" : "error");
    if (!ok)                  printf("semantico|na\nintermedio|na\nz80|na\n");
    else if (sem_con_errores) printf("semantico|error\nintermedio|na\nz80|na\n");
    else                      printf("semantico|ok\nintermedio|ok\nz80|ok\n");
    printf("@@END\n");
    return 0;
}

int main(int argc, char **argv) {
    const char *ruta = NULL;

    /* Procesa argumentos: bandera --gui y la ruta del archivo. */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--gui") == 0) modo_gui = 1;
        else                               ruta = argv[i];
    }

    if (!ruta) {
        printf("Por favor ingresa un archivo de texto con extension '.g5z80'.\n");
        printf("Ejemplo: .\\compilador.exe ejemplo1.g5z80   (o --gui para la interfaz)\n");
        return 1;
    }
    if (!ends_with_g5z80(ruta)) {
        fprintf(stderr, "\n[ERROR DE EXTENSION] El compilador solo acepta archivos con extension '.g5z80'.\n");
        fprintf(stderr, "Ejemplo de uso: .\\compilador.exe ejemplo1.g5z80\n\n");
        return 1;
    }

    yyin = fopen(ruta, "r");
    if (!yyin) {
        perror(ruta);
        return 1;
    }

    if (modo_gui) {
        lexer_verbose = 0;        /* sin trazas del lexer en el flujo @@ */
        int r = correr_gui();
        fclose(yyin);
        return r;
    }

    printf("\n==================================================\n");
    printf("Iniciando analisis de: %s\n", ruta);
    printf("==================================================\n\n");

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
