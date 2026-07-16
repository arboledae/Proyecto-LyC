#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser_rd.h"
#include "simbolos.h"
#include "semantico.h"
#include "gui_salida.h"
#include "codigo_intermedio.h"
#include "generador_z80.h"

/* Simbolos del analizador lexico (Flex, lex.yy.c). */
extern int   yylineno;
extern FILE *yyin;
extern int   lexer_verbose;   /* traza por token del lexer (definida en lex.yy.c) */

/* ── Modo de salida ──────────────────────────────────────────
   modo_gui = 1  -> el compilador emite el formato delimitado (@@...)
                    que consume la interfaz grafica (sin JSON en C).
   modo_gui = 0  -> salida legible para consola (comportamiento normal).
   Lo consulta parser_rd.c para decidir como reportar los errores. */
int modo_gui = 0;

/* Acepta solo archivos con extension .g5z80 (la del lenguaje Dummy). */
static int ends_with_g5z80(const char *filename) {
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

/* ── Flujo de consola: las 5 fases en secuencia legible ───────── */
static void correr_consola(const char *ruta) {
    printf("\n==================================================\n");
    printf("Iniciando analisis de: %s\n", ruta);
    printf("==================================================\n\n");

    /* Fases 1 y 2: lexico (traza del lexer) + sintactico -> AST. */
    NodoAST *raiz = parsear();

    if (!raiz) {
        printf("\n==================================================\n");
        printf("ANALISIS CONCLUIDO CON ERRORES SINTACTICOS\n");
        printf("==================================================\n");
        return;
    }

    printf("\n==================================================\n");
    printf("ARBOL DE SINTAXIS ABSTRACTA (AST)\n");
    printf("==================================================\n\n");
    imprimir_ast(raiz, 0);
    printf("\n==================================================\n");
    printf("FIN DEL AST\n");
    printf("==================================================\n");

    /* Fase 3: analisis semantico (tabla de simbolos + verificacion de tipos). */
    analizar_semantico(raiz);
    imprimir_tabla();
    imprimir_errores_sem();

    /* Fases 4 y 5: codigo intermedio (TAC) y ensamblador Z80 (solo sin errores). */
    if (!hay_errores_sem()) {
        generar_tac(raiz);
        imprimir_tac();
        imprimir_z80();
        liberar_tac();
    }

    liberar_errores_sem();
    liberar_tabla();
    liberar_ast(raiz);

    printf("\n==================================================\n");
    printf("ANALISIS SINTACTICO Y AST COMPLETADOS EXITOSAMENTE\n");
    printf("==================================================\n");
}

/* ── Flujo grafico (--gui): una sola pasada con secciones @@ ────
   Emite @@TOKENS / @@AST / @@SIMBOLOS / @@ERRORES / @@INTERMEDIO /
   @@Z80 / @@ESTADO / @@END que consume la interfaz grafica. */
static void correr_gui(void) {
    printf("@@TOKENS\n");
    emitir_tokens_gui();          /* recorre el archivo y lo rebobina */

    NodoAST *raiz = parsear();    /* la regla 'programa' del descenso recursivo */
    int ok = (raiz != NULL);
    int sem_con_errores = 0;

    if (ok) {
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

        liberar_errores_sem();
        liberar_tabla();
        liberar_ast(raiz);
    } else {
        /* Con error sintactico se emiten las secciones vacias y el error. */
        printf("@@AST\n@@SIMBOLOS\n@@ERRORES\n");
        if (parser_hubo_error())
            printf("sintactico|%d|%s\n", parser_error_linea(), parser_error_msg());
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
        correr_gui();
    } else {
        correr_consola(ruta);
    }

    fclose(yyin);
    return 0;
}
