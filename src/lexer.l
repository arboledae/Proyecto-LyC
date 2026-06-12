%{
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

"inicio"          { printf("[Lexico] Linea %d: Palabra clave 'inicio'\n",   yylineno); return KW_INICIO; }
"sino"            { printf("[Lexico] Linea %d: Palabra clave 'sino'\n",     yylineno); return KW_SINO; }
"cuando"          { printf("[Lexico] Linea %d: Palabra clave 'cuando'\n",   yylineno); return KW_CUANDO; }
"loop"            { printf("[Lexico] Linea %d: Palabra clave 'loop'\n",     yylineno); return KW_LOOP; }
"imprimir"        { printf("[Lexico] Linea %d: Palabra clave 'imprimir'\n", yylineno); return KW_IMPRIMIR; }
"retornar"        { printf("[Lexico] Linea %d: Palabra clave 'retornar'\n", yylineno); return KW_RETORNAR; }
"num"             { printf("[Lexico] Linea %d: Tipo de dato 'num'\n",       yylineno); return KW_NUM; }
"dec"             { printf("[Lexico] Linea %d: Tipo de dato 'dec'\n",       yylineno); return KW_DEC; }
"text"            { printf("[Lexico] Linea %d: Tipo de dato 'text'\n",      yylineno); return KW_TEXT; }

"verdadero"       { printf("[Lexico] Linea %d: Booleano 'verdadero'\n", yylineno); yylval.entero = 1; return LIT_BOOLEANO; }
"falso"           { printf("[Lexico] Linea %d: Booleano 'falso'\n",     yylineno); yylval.entero = 0; return LIT_BOOLEANO; }

"=="              { printf("[Lexico] Linea %d: Operador OP_IGUAL\n",        yylineno); return OP_IGUAL; }
"!="              { printf("[Lexico] Linea %d: Operador OP_DIFERENTE\n",    yylineno); return OP_DIFERENTE; }
"<="              { printf("[Lexico] Linea %d: Operador OP_MENOR_IGUAL\n",  yylineno); return OP_MENOR_IGUAL; }
">="              { printf("[Lexico] Linea %d: Operador OP_MAYOR_IGUAL\n",  yylineno); return OP_MAYOR_IGUAL; }
"<"               { printf("[Lexico] Linea %d: Operador OP_MENOR\n",        yylineno); return OP_MENOR; }
">"               { printf("[Lexico] Linea %d: Operador OP_MAYOR\n",        yylineno); return OP_MAYOR; }

"&&"              { printf("[Lexico] Linea %d: Operador OP_AND\n",  yylineno); return OP_AND; }
"||"              { printf("[Lexico] Linea %d: Operador OP_OR\n",   yylineno); return OP_OR; }
"!"               { printf("[Lexico] Linea %d: Operador OP_NOT\n",  yylineno); return OP_NOT; }

"+"               { printf("[Lexico] Linea %d: Operador OP_SUMA\n",  yylineno); return OP_SUMA; }
"-"               { printf("[Lexico] Linea %d: Operador OP_RESTA\n", yylineno); return OP_RESTA; }
"*"               { printf("[Lexico] Linea %d: Operador OP_MULT\n",  yylineno); return OP_MULT; }
"/"               { printf("[Lexico] Linea %d: Operador OP_DIV\n",   yylineno); return OP_DIV; }
"%"               { printf("[Lexico] Linea %d: Operador OP_MOD\n",   yylineno); return OP_MOD; }

"="               { printf("[Lexico] Linea %d: Operador OP_ASIGNACION (=)\n", yylineno); return OP_ASIGNACION; }

";"               { printf("[Lexico] Linea %d: Delimitador DELIM_PUNTO_COMA\n", yylineno); return DELIM_PUNTO_COMA; }
"("               { printf("[Lexico] Linea %d: Delimitador DELIM_PAR_IZQ\n",   yylineno); return DELIM_PAR_IZQ; }
")"               { printf("[Lexico] Linea %d: Delimitador DELIM_PAR_DER\n",   yylineno); return DELIM_PAR_DER; }
"{"               { printf("[Lexico] Linea %d: Delimitador DELIM_LLAVE_IZQ\n", yylineno); return DELIM_LLAVE_IZQ; }
"}"               { printf("[Lexico] Linea %d: Delimitador DELIM_LLAVE_DER\n", yylineno); return DELIM_LLAVE_DER; }

{LIT_DECIMAL}     { printf("[Lexico] Linea %d: Decimal '%s'\n",      yylineno, yytext); yylval.str    = strdup(yytext); return LIT_DECIMAL; }
{LIT_ENTERO}      { printf("[Lexico] Linea %d: Entero '%s'\n",       yylineno, yytext); yylval.entero = atoi(yytext);  return LIT_ENTERO; }
{LIT_CADENA}      { printf("[Lexico] Linea %d: Cadena de texto\n",   yylineno);         yylval.str    = strdup(yytext); return LIT_TEXT; }
{IDENTIFICADOR}   { printf("[Lexico] Linea %d: Identificador '%s'\n", yylineno, yytext); yylval.str   = strdup(yytext); return IDENTIFICADOR; }

.                 { printf("[ERROR LEXICO] Linea %d: Caracter no reconocido '%s'\n", yylineno, yytext); return TOKEN_ERROR; }

%%