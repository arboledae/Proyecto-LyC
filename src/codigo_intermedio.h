#ifndef CODIGO_INTERMEDIO_H
#define CODIGO_INTERMEDIO_H

#include "ast.h"

/* ── Codigo intermedio: tres direcciones / cuadruplas ─────────
   Cuarta fase del compilador (tras lexico, sintactico y semantico).
   Recorre el AST ya validado y lo linealiza en una lista de
   cuadruplas (op, arg1, arg2, res) con temporales t1..tn y
   etiquetas L1..Ln. Este codigo es la entrada del generador Z80.

   Operaciones emitidas:
     "+","-","*","/","%","<",">","<=",">=","==","!=","&&","||"
                res = arg1 op arg2
     "neg"      res = - arg1          (menos unario)
     "not"      res = ! arg1
     "="        res = arg1            (copia / asignacion)
     "label"    res:                  (declara la etiqueta res)
     "goto"     goto res
     "ifFalse"  ifFalse arg1 goto res
     "imprimir" imprimir arg1         (arg2 = tipo del valor)
     "retornar" retornar [arg1]
     "fin"      fin del programa

   Operandos (cadenas):
     t<N>          temporal            L<N>       etiqueta
     "..."         literal de texto    123        literal entero
     12.5          literal decimal     nombre     variable global
     nombre.<S>    variable declarada en el ambito S (unicidad
                   entre ambitos: 'x' de scope 2 se llama x.2)   */

typedef struct Cuad {
    char *op;
    char *arg1;              /* NULL si no aplica */
    char *arg2;              /* NULL si no aplica */
    char *res;               /* NULL si no aplica */
    struct Cuad *sig;
} Cuad;

/* Variables del programa con su nombre unico entre ambitos. */
typedef struct TacSim {
    char *nombre;            /* nombre original                  */
    char *unico;             /* nombre unico (x, x.2, ...)       */
    char *tipo;              /* num | dec | text | booleano      */
    int   scope;
    struct TacSim *sig;
} TacSim;

int      generar_tac       (NodoAST *raiz);   /* devuelve nro de cuadruplas */
Cuad*    tac_cuadruplas    (void);
TacSim*  tac_simbolos      (void);
int      tac_num_temporales(void);
void     tac_formatear     (const Cuad *c, char *buf, int tam);
void     imprimir_tac      (void);            /* listado legible (consola)  */
void     emitir_tac_gui    (void);            /* "n|op|arg1|arg2|res|texto" */
void     liberar_tac       (void);

#endif /* CODIGO_INTERMEDIO_H */
