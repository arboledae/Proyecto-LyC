#ifndef PARSER_RD_H
#define PARSER_RD_H

#include "ast.h"

/* ── Analizador sintactico: parser predictivo LL(1) por descenso
      recursivo (teoria top-down de la S9y10) ───────────────────────

   Reemplaza al generador ascendente (Bison). Una funcion por cada no
   terminal de la gramatica del lenguaje Dummy; cada una elige su
   produccion mirando UN solo token de anticipacion (LL(1)) y devuelve el
   subarbol del AST que le corresponde.

   parsear() recorre todo el archivo abierto en 'yyin' (via yylex()) y
   devuelve la raiz del AST (NODO_PROGRAMA) o NULL si hubo un error
   sintactico. En caso de error, el detalle queda disponible en los
   accesores de abajo (usados por el modo grafico --gui). */

NodoAST* parsear(void);

/* Estado del ultimo error sintactico (para emitir @@ERRORES en la GUI). */
int         parser_hubo_error (void);
int         parser_error_linea(void);
const char* parser_error_msg  (void);

#endif /* PARSER_RD_H */
