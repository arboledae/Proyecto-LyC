#ifndef GUI_SALIDA_H
#define GUI_SALIDA_H

#include "ast.h"

/* Emision en formato delimitado para la interfaz grafica (modo --gui).
   El formato evita JSON en C: cada seccion empieza con una marca @@ y sus
   lineas usan '|' como separador de campos, trivial de parsear en Python.
   Lo definido aqui corresponde a las secciones @@TOKENS y @@AST; las
   secciones @@SIMBOLOS y @@ERRORES las emiten simbolos.c y semantico.c. */

/* Re-lexa el archivo de entrada (yyin) y emite una linea por token:
   "linea|NOMBRE_TOKEN|lexema". Deja el lexer reiniciado para que el
   parser pueda volver a recorrer el archivo desde el inicio. */
void emitir_tokens_gui(void);

/* Recorre el AST y emite una linea por nodo: "profundidad|etiqueta".
   La profundidad refleja exactamente el nivel de anidamiento, de modo
   que el frontend reconstruye el arbol sin ambiguedad. */
void emitir_ast_gui(NodoAST *raiz);

#endif /* GUI_SALIDA_H */
