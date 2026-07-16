%{
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Traza por token: 1 = imprime (consola del compilador), 0 = silencioso.
   El analizador lexico independiente y el modo --gui la apagan para no
   contaminar su salida (el cronograma pide "retorno de tokens, no printf"). */
int lexer_verbose = 1;
#define LEXLOG(...) do { if (lexer_verbose) printf(__VA_ARGS__); } while (0)
%}
%option noyywrap
%option yylineno

DIGITO          [0-9]
LETRA           [a-zA-Z_]
IDENTIFICADOR   {LETRA}({LETRA}|{DIGITO})*
LIT_ENTERO      {DIGITO}+
LIT_DECIMAL     {DIGITO}+\.{DIGITO}+
LIT_CADENA      \"([^\"\\\n]|\\.)*\"
COMENT_BLOQUE   "%%"([^%]|"%"[^%])*"%%"
ESPACIOS        [ \t\r\n]+

%%

{ESPACIOS}        { /* Ignorar */ }
{COMENT_BLOQUE}   { /* Ignorar */ }

"inicio"          { LEXLOG("[Lexico] Linea %d: Palabra clave 'inicio'\n",   yylineno); return KW_INICIO; }
"sino"            { LEXLOG("[Lexico] Linea %d: Palabra clave 'sino'\n",     yylineno); return KW_SINO; }
"cuando"          { LEXLOG("[Lexico] Linea %d: Palabra clave 'cuando'\n",   yylineno); return KW_CUANDO; }
"loop"            { LEXLOG("[Lexico] Linea %d: Palabra clave 'loop'\n",     yylineno); return KW_LOOP; }
"imprimir"        { LEXLOG("[Lexico] Linea %d: Palabra clave 'imprimir'\n", yylineno); return KW_IMPRIMIR; }
"retornar"        { LEXLOG("[Lexico] Linea %d: Palabra clave 'retornar'\n", yylineno); return KW_RETORNAR; }

"limpiar"         { LEXLOG("[Lexico] Linea %d: Palabra clave 'limpiar'\n",    yylineno); return KW_LIMPIAR; }
"posicionar"      { LEXLOG("[Lexico] Linea %d: Palabra clave 'posicionar'\n", yylineno); return KW_POSICIONAR; }
"dibujar"         { LEXLOG("[Lexico] Linea %d: Palabra clave 'dibujar'\n",    yylineno); return KW_DIBUJAR; }
"pintar"          { LEXLOG("[Lexico] Linea %d: Palabra clave 'pintar'\n",     yylineno); return KW_PINTAR; }
"esperar"         { LEXLOG("[Lexico] Linea %d: Palabra clave 'esperar'\n",    yylineno); return KW_ESPERAR; }
"tecla"           { LEXLOG("[Lexico] Linea %d: Palabra clave 'tecla'\n",      yylineno); return KW_TECLA; }
"aleatorio"       { LEXLOG("[Lexico] Linea %d: Palabra clave 'aleatorio'\n",  yylineno); return KW_ALEATORIO; }
"sprites"         { LEXLOG("[Lexico] Linea %d: Palabra clave 'sprites'\n",    yylineno); return KW_SPRITES; }
"num"             { LEXLOG("[Lexico] Linea %d: Tipo de dato 'num'\n",       yylineno); return KW_NUM; }
"dec"             { LEXLOG("[Lexico] Linea %d: Tipo de dato 'dec'\n",       yylineno); return KW_DEC; }
"text"            { LEXLOG("[Lexico] Linea %d: Tipo de dato 'text'\n",      yylineno); return KW_TEXT; }

"verdadero"       { LEXLOG("[Lexico] Linea %d: Booleano 'verdadero'\n", yylineno); yylval.entero = 1; return LIT_BOOLEANO; }
"falso"           { LEXLOG("[Lexico] Linea %d: Booleano 'falso'\n",     yylineno); yylval.entero = 0; return LIT_BOOLEANO; }

"=="              { LEXLOG("[Lexico] Linea %d: Operador OP_IGUAL\n",        yylineno); return OP_IGUAL; }
"!="              { LEXLOG("[Lexico] Linea %d: Operador OP_DIFERENTE\n",    yylineno); return OP_DIFERENTE; }
"<="              { LEXLOG("[Lexico] Linea %d: Operador OP_MENOR_IGUAL\n",  yylineno); return OP_MENOR_IGUAL; }
">="              { LEXLOG("[Lexico] Linea %d: Operador OP_MAYOR_IGUAL\n",  yylineno); return OP_MAYOR_IGUAL; }
"<"               { LEXLOG("[Lexico] Linea %d: Operador OP_MENOR\n",        yylineno); return OP_MENOR; }
">"               { LEXLOG("[Lexico] Linea %d: Operador OP_MAYOR\n",        yylineno); return OP_MAYOR; }

"&&"              { LEXLOG("[Lexico] Linea %d: Operador OP_AND\n",  yylineno); return OP_AND; }
"||"              { LEXLOG("[Lexico] Linea %d: Operador OP_OR\n",   yylineno); return OP_OR; }
"!"               { LEXLOG("[Lexico] Linea %d: Operador OP_NOT\n",  yylineno); return OP_NOT; }

"+"               { LEXLOG("[Lexico] Linea %d: Operador OP_SUMA\n",  yylineno); return OP_SUMA; }
"-"               { LEXLOG("[Lexico] Linea %d: Operador OP_RESTA\n", yylineno); return OP_RESTA; }
"*"               { LEXLOG("[Lexico] Linea %d: Operador OP_MULT\n",  yylineno); return OP_MULT; }
"/"               { LEXLOG("[Lexico] Linea %d: Operador OP_DIV\n",   yylineno); return OP_DIV; }
"%"               { LEXLOG("[Lexico] Linea %d: Operador OP_MOD\n",   yylineno); return OP_MOD; }

"="               { LEXLOG("[Lexico] Linea %d: Operador OP_ASIGNACION (=)\n", yylineno); return OP_ASIGNACION; }

";"               { LEXLOG("[Lexico] Linea %d: Delimitador DELIM_PUNTO_COMA\n", yylineno); return DELIM_PUNTO_COMA; }
","               { LEXLOG("[Lexico] Linea %d: Delimitador DELIM_COMA\n",       yylineno); return DELIM_COMA; }
"("               { LEXLOG("[Lexico] Linea %d: Delimitador DELIM_PAR_IZQ\n",   yylineno); return DELIM_PAR_IZQ; }
")"               { LEXLOG("[Lexico] Linea %d: Delimitador DELIM_PAR_DER\n",   yylineno); return DELIM_PAR_DER; }
"{"               { LEXLOG("[Lexico] Linea %d: Delimitador DELIM_LLAVE_IZQ\n", yylineno); return DELIM_LLAVE_IZQ; }
"}"               { LEXLOG("[Lexico] Linea %d: Delimitador DELIM_LLAVE_DER\n", yylineno); return DELIM_LLAVE_DER; }

{LIT_DECIMAL}     { LEXLOG("[Lexico] Linea %d: Decimal '%s'\n",      yylineno, yytext); yylval.str    = strdup(yytext); return LIT_DECIMAL; }
{LIT_ENTERO}      { LEXLOG("[Lexico] Linea %d: Entero '%s'\n",       yylineno, yytext); yylval.entero = atoi(yytext);  return LIT_ENTERO; }
{LIT_CADENA}      { LEXLOG("[Lexico] Linea %d: Cadena de texto\n",   yylineno);         yylval.str    = strdup(yytext); return LIT_TEXT; }
{IDENTIFICADOR}   { LEXLOG("[Lexico] Linea %d: Identificador '%s'\n", yylineno, yytext); yylval.str   = strdup(yytext); return IDENTIFICADOR; }

.                 { LEXLOG("[ERROR LEXICO] Linea %d: Caracter no reconocido '%s'\n", yylineno, yytext); return TOKEN_ERROR; }

%%
