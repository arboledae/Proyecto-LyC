#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <string.h>

/* ── Tipos de nodo ──────────────────────────────────────────── */
typedef enum {
    NODO_PROGRAMA,
    NODO_LISTA_SENT,
    NODO_DECLARACION,
    NODO_ASIGNACION,
    NODO_CONDICIONAL,
    NODO_CICLO,
    NODO_IMPRESION,
    NODO_RETORNO,
    NODO_OP_BINARIO,
    NODO_OP_UNARIO,
    NODO_ENTERO,
    NODO_DECIMAL,
    NODO_TEXTO,
    NODO_BOOLEANO,
    NODO_IDENTIFICADOR,
    /* ── Primitivas de juego (Amstrad CPC) ─────────────────────
       Añadidas para poder escribir videojuegos: entrada de teclado,
       control de pantalla, temporizacion y numeros aleatorios. */
    NODO_LIMPIAR,        /* limpiar()            -> borra la pantalla   */
    NODO_POSICIONAR,     /* posicionar(x, y)     -> mueve el cursor      */
    NODO_DIBUJAR,        /* dibujar(expr)        -> imprime sin salto    */
    NODO_PINTAR,         /* pintar(codigo)       -> caracter por codigo  */
    NODO_ESPERAR,        /* esperar(n)           -> espera n cuadros     */
    NODO_LEER_TECLA,     /* tecla()              -> lee teclado (expr)   */
    NODO_ALEATORIO       /* aleatorio(n)         -> azar 0..n-1 (expr)   */
} TipoNodo;

/* ── Estructura del nodo ────────────────────────────────────── */
typedef struct NodoAST {
    TipoNodo tipo;
    char          *str_val;
    int            int_val;
    int            linea;          /* linea de codigo fuente (para errores semanticos) */
    struct NodoAST *izq;
    struct NodoAST *der;
    struct NodoAST *extra;
} NodoAST;

/* ── Constructores ──────────────────────────────────────────── */
NodoAST* nuevo_nodo      (TipoNodo tipo);
NodoAST* nodo_entero     (int val);
NodoAST* nodo_decimal    (char *val);
NodoAST* nodo_texto      (char *val);
NodoAST* nodo_booleano   (int val);
NodoAST* nodo_id         (char *nombre);
NodoAST* nodo_op_binario (char *op, NodoAST *izq, NodoAST *der);
NodoAST* nodo_op_unario  (char *op, NodoAST *operando);
NodoAST* nodo_declaracion(char *tipo, char *nombre, NodoAST *expr);
NodoAST* nodo_asignacion (char *nombre, NodoAST *expr);
NodoAST* nodo_condicional(NodoAST *cond, NodoAST *bloque_si, NodoAST *bloque_sino);
NodoAST* nodo_ciclo      (NodoAST *cond, NodoAST *cuerpo);
NodoAST* nodo_impresion  (NodoAST *expr);
NodoAST* nodo_retorno    (NodoAST *expr);
NodoAST* nodo_lista      (NodoAST *sentencia, NodoAST *siguiente);

/* ── Constructores de las primitivas de juego ───────────────── */
NodoAST* nodo_limpiar    (void);
NodoAST* nodo_posicionar (NodoAST *x, NodoAST *y);
NodoAST* nodo_dibujar    (NodoAST *expr);
NodoAST* nodo_pintar     (NodoAST *expr);
NodoAST* nodo_esperar    (NodoAST *expr);
NodoAST* nodo_leer_tecla (void);
NodoAST* nodo_aleatorio  (NodoAST *expr);

/* ── Utilidades ─────────────────────────────────────────────── */
void imprimir_ast(NodoAST *nodo, int nivel);
void liberar_ast (NodoAST *nodo);

#endif /* AST_H */
