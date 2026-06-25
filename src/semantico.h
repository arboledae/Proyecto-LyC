#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "ast.h"

/* ── Analisis semantico (Semana 12) ───────────────────────────
   Recorre el AST construido por Bison y verifica:
   - variables declaradas / doble declaracion
   - compatibilidad de tipos en asignaciones y declaraciones
   - operaciones invalidas entre tipos
   - condiciones booleanas en cuando / loop
   Los errores se acumulan en una lista y se imprimen al final. */

typedef struct ErrorSem {
    int   linea;
    char *mensaje;
    struct ErrorSem *siguiente;
} ErrorSem;

void analizar_semantico  (NodoAST *raiz);
void imprimir_errores_sem(void);          /* volcado legible para consola              */
void emitir_errores_gui  (void);          /* "semantico|linea|mensaje" (modo --gui)    */
int  hay_errores_sem     (void);
void liberar_errores_sem (void);

#endif /* SEMANTICO_H */
