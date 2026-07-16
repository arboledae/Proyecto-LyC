#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <setjmp.h>
#include "parser_rd.h"
#include "ast.h"
#include "tokens.h"

/* Simbolos que aporta el analizador lexico (Flex, lex.yy.c). */
extern int   yylex(void);
extern int   yylineno;
extern char *yytext;

/* Valor semantico del token (lo rellena el lexer). Lo define este binario. */
ValorToken yylval;

/* modo_gui lo define el driver (main.c). En modo grafico el error no se
   imprime en stderr: se captura para emitirlo en la seccion @@ERRORES. */
extern int modo_gui;

/* ── Token de anticipacion (LL(1): un solo token) ─────────────── */
static int  tok;              /* codigo del token actual                     */
static int  tok_linea;        /* linea del token actual                      */
static int  tok_entero;       /* valor de LIT_ENTERO / LIT_BOOLEANO          */
static char tok_texto[256];   /* lexema del token actual (copia de yytext)   */

/* ── Estado del error sintactico ──────────────────────────────── */
static int     hubo_error = 0;
static int     err_linea  = 0;
static char    err_msg[256] = "";
static jmp_buf punto_recuperacion;   /* corte limpio del descenso (longjmp)  */

int         parser_hubo_error (void) { return hubo_error; }
int         parser_error_linea(void) { return err_linea; }
const char* parser_error_msg  (void) { return err_msg;   }

/* ── Motor del parser ─────────────────────────────────────────── */

/* Lee el siguiente token y copia su lexema/valor (yytext y yylval se
   sobrescriben en la proxima llamada a yylex, por eso se guardan aqui). */
static void avanzar(void) {
    tok       = yylex();
    tok_linea = yylineno;
    if (yytext) {
        strncpy(tok_texto, yytext, sizeof(tok_texto) - 1);
        tok_texto[sizeof(tok_texto) - 1] = '\0';
    } else {
        tok_texto[0] = '\0';
    }
    if (tok == LIT_ENTERO || tok == LIT_BOOLEANO)
        tok_entero = yylval.entero;
}

/* Registra el primer error sintactico y aborta el descenso con longjmp. */
static void error_sintactico(const char *fmt, ...) {
    char buf[200];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    hubo_error = 1;
    err_linea  = tok_linea;
    snprintf(err_msg, sizeof(err_msg), "%s", buf);

    if (!modo_gui)
        fprintf(stderr, "\n[ERROR SINTACTICO] Linea %d: %s\n\n", tok_linea, buf);

    longjmp(punto_recuperacion, 1);
}

/* Consume el token esperado o reporta un error sintactico. */
static void esperar(int t, const char *desc) {
    if (tok != t) error_sintactico("se esperaba %s", desc);
    avanzar();
}

/* ── Conjuntos FIRST (deciden la produccion sin retroceso) ────── */
static int es_inicio_sentencia(int t) {
    return t == KW_NUM || t == KW_DEC || t == KW_TEXT ||
           t == IDENTIFICADOR || t == KW_CUANDO || t == KW_LOOP ||
           t == KW_IMPRIMIR || t == KW_RETORNAR;
}

static int es_inicio_expresion(int t) {
    return t == DELIM_PAR_IZQ || t == IDENTIFICADOR ||
           t == LIT_ENTERO || t == LIT_DECIMAL || t == LIT_TEXT ||
           t == LIT_BOOLEANO || t == OP_NOT || t == OP_RESTA;
}

/* Declaraciones adelantadas (funciones mutuamente recursivas). */
static NodoAST* bloque(void);
static NodoAST* lista_sentencias(void);
static NodoAST* sentencia(void);
static NodoAST* expresion(void);

/* ── Expresiones: una cascada de niveles de precedencia ─────────
   Cada nivel elimina la recursion izquierda de la gramatica clasica
   con un bucle 'while' (transformacion Expr -> Term Expr' de la S9y10),
   produciendo el mismo arbol asociativo a la izquierda. */

static NodoAST* e_primaria(void) {
    switch (tok) {
        case DELIM_PAR_IZQ: {
            avanzar();
            NodoAST *e = expresion();
            esperar(DELIM_PAR_DER, "')'");
            return e;
        }
        case IDENTIFICADOR: { NodoAST *n = nodo_id(tok_texto);            avanzar(); return n; }
        case LIT_ENTERO:    { NodoAST *n = nodo_entero(tok_entero);       avanzar(); return n; }
        case LIT_DECIMAL:   { NodoAST *n = nodo_decimal(strdup(tok_texto)); avanzar(); return n; }
        case LIT_TEXT:      { NodoAST *n = nodo_texto(strdup(tok_texto));   avanzar(); return n; }
        case LIT_BOOLEANO:  { NodoAST *n = nodo_booleano(tok_entero);     avanzar(); return n; }
        default:
            error_sintactico("se esperaba una expresion");
            return NULL;   /* inalcanzable: error_sintactico hace longjmp */
    }
}

static NodoAST* e_unaria(void) {
    if (tok == OP_NOT)  { avanzar(); return nodo_op_unario("!", e_unaria()); }
    if (tok == OP_RESTA){ avanzar(); return nodo_op_unario("-", e_unaria()); }
    return e_primaria();
}

static NodoAST* e_mult(void) {
    NodoAST *izq = e_unaria();
    while (tok == OP_MULT || tok == OP_DIV || tok == OP_MOD) {
        const char *op = (tok == OP_MULT) ? "*" : (tok == OP_DIV) ? "/" : "%";
        avanzar();
        izq = nodo_op_binario(op, izq, e_unaria());
    }
    return izq;
}

static NodoAST* e_aditiva(void) {
    NodoAST *izq = e_mult();
    while (tok == OP_SUMA || tok == OP_RESTA) {
        const char *op = (tok == OP_SUMA) ? "+" : "-";
        avanzar();
        izq = nodo_op_binario(op, izq, e_mult());
    }
    return izq;
}

static NodoAST* e_relacional(void) {
    NodoAST *izq = e_aditiva();
    while (tok == OP_MENOR || tok == OP_MAYOR ||
           tok == OP_MENOR_IGUAL || tok == OP_MAYOR_IGUAL) {
        const char *op = (tok == OP_MENOR) ? "<" :
                         (tok == OP_MAYOR) ? ">" :
                         (tok == OP_MENOR_IGUAL) ? "<=" : ">=";
        avanzar();
        izq = nodo_op_binario(op, izq, e_aditiva());
    }
    return izq;
}

static NodoAST* e_igualdad(void) {
    NodoAST *izq = e_relacional();
    while (tok == OP_IGUAL || tok == OP_DIFERENTE) {
        const char *op = (tok == OP_IGUAL) ? "==" : "!=";
        avanzar();
        izq = nodo_op_binario(op, izq, e_relacional());
    }
    return izq;
}

static NodoAST* e_and(void) {
    NodoAST *izq = e_igualdad();
    while (tok == OP_AND) {
        avanzar();
        izq = nodo_op_binario("&&", izq, e_igualdad());
    }
    return izq;
}

static NodoAST* e_or(void) {
    NodoAST *izq = e_and();
    while (tok == OP_OR) {
        avanzar();
        izq = nodo_op_binario("||", izq, e_and());
    }
    return izq;
}

static NodoAST* expresion(void) {
    return e_or();
}

/* ── Sentencias ───────────────────────────────────────────────── */

static NodoAST* declaracion(void) {
    const char *tipo;
    switch (tok) {
        case KW_NUM:  tipo = "num";  break;
        case KW_DEC:  tipo = "dec";  break;
        case KW_TEXT: tipo = "text"; break;
        default: error_sintactico("se esperaba un tipo (num, dec o text)"); return NULL;
    }
    avanzar();   /* consume el tipo */

    if (tok != IDENTIFICADOR) error_sintactico("se esperaba un identificador");
    char nombre[256];
    strncpy(nombre, tok_texto, sizeof(nombre) - 1);
    nombre[sizeof(nombre) - 1] = '\0';
    avanzar();   /* consume el identificador */

    if (tok == OP_ASIGNACION) {
        avanzar();
        return nodo_declaracion(tipo, nombre, expresion());
    }
    return nodo_declaracion(tipo, nombre, NULL);
}

static NodoAST* asignacion(void) {
    char nombre[256];
    strncpy(nombre, tok_texto, sizeof(nombre) - 1);   /* tok == IDENTIFICADOR */
    nombre[sizeof(nombre) - 1] = '\0';
    avanzar();
    esperar(OP_ASIGNACION, "'='");
    return nodo_asignacion(nombre, expresion());
}

static NodoAST* condicional(void) {
    avanzar();   /* consume 'cuando' */
    esperar(DELIM_PAR_IZQ, "'('");
    NodoAST *cond = expresion();
    esperar(DELIM_PAR_DER, "')'");
    NodoAST *bloque_si = bloque();

    NodoAST *bloque_sino = NULL;
    if (tok == KW_SINO) {              /* el 'sino' liga con el 'cuando' mas
                                          cercano: se consume en esta llamada */
        avanzar();
        bloque_sino = bloque();
    }
    return nodo_condicional(cond, bloque_si, bloque_sino);
}

static NodoAST* ciclo(void) {
    avanzar();   /* consume 'loop' */
    esperar(DELIM_PAR_IZQ, "'('");
    NodoAST *cond = expresion();
    esperar(DELIM_PAR_DER, "')'");
    return nodo_ciclo(cond, bloque());
}

static NodoAST* impresion(void) {
    avanzar();   /* consume 'imprimir' */
    esperar(DELIM_PAR_IZQ, "'('");
    NodoAST *e = expresion();
    esperar(DELIM_PAR_DER, "')'");
    return nodo_impresion(e);
}

static NodoAST* retorno(void) {
    avanzar();   /* consume 'retornar' */
    if (es_inicio_expresion(tok))
        return nodo_retorno(expresion());
    return nodo_retorno(NULL);
}

static NodoAST* sentencia(void) {
    switch (tok) {
        case KW_NUM:
        case KW_DEC:
        case KW_TEXT: {
            NodoAST *n = declaracion();
            esperar(DELIM_PUNTO_COMA, "';'");
            return n;
        }
        case IDENTIFICADOR: {
            NodoAST *n = asignacion();
            esperar(DELIM_PUNTO_COMA, "';'");
            return n;
        }
        case KW_CUANDO: return condicional();   /* sin ';' final */
        case KW_LOOP:   return ciclo();         /* sin ';' final */
        case KW_IMPRIMIR: {
            NodoAST *n = impresion();
            esperar(DELIM_PUNTO_COMA, "';'");
            return n;
        }
        case KW_RETORNAR: {
            NodoAST *n = retorno();
            esperar(DELIM_PUNTO_COMA, "';'");
            return n;
        }
        default:
            error_sintactico("se esperaba una sentencia");
            return NULL;
    }
}

/* ListaSent -> Sentencia ListaSent | e   (recursion derecha: lista anidada) */
static NodoAST* lista_sentencias(void) {
    if (!es_inicio_sentencia(tok)) return NULL;
    NodoAST *s     = sentencia();
    NodoAST *resto = lista_sentencias();
    return nodo_lista(s, resto);
}

/* Bloque -> { ListaSent } */
static NodoAST* bloque(void) {
    esperar(DELIM_LLAVE_IZQ, "'{'");
    NodoAST *l = lista_sentencias();
    esperar(DELIM_LLAVE_DER, "'}'");
    return l;
}

/* Programa -> inicio { ListaSent } */
static NodoAST* programa(void) {
    esperar(KW_INICIO, "'inicio'");
    NodoAST *sents = bloque();
    NodoAST *raiz  = nuevo_nodo(NODO_PROGRAMA);
    raiz->izq = sents;
    return raiz;
}

/* ── API ──────────────────────────────────────────────────────── */
NodoAST* parsear(void) {
    hubo_error = 0;
    err_linea  = 0;
    err_msg[0] = '\0';

    if (setjmp(punto_recuperacion) != 0)
        return NULL;    /* se llego aqui por un error sintactico (longjmp) */

    avanzar();          /* carga el primer token de anticipacion */
    NodoAST *raiz = programa();

    if (tok != 0)       /* debe haberse consumido todo el archivo (EOF = 0) */
        error_sintactico("se esperaba el fin del archivo");

    return raiz;
}
