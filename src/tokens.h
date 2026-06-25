#ifndef TOKENS_H
#define TOKENS_H

/* Mapea el codigo numerico de un token (definido por Bison en parser.tab.h)
   a su nombre simbolico. Compartido por el analizador lexico independiente
   y por la emision de tokens del modo --gui del compilador. */
const char* nombre_token(int token);

#endif /* TOKENS_H */
