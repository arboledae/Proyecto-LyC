%{
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
%}

%option noyywrap
%option yylineno

DIGITO          [0-9]
LETRA           [a-zA-Z_]
IDENTIFICADOR   {LETRA}({LETRA}|{DIGITO})*
LIT_ENTERO      {DIGITO}+
LIT_DECIMAL     {DIGITO}+\.{DIGITO}+
LIT_CADENA      \"([^\"\\\n]|\\.)*\"
COMENT_LINEA    \/\/.*
COMENT_BLOQUE   \/\*([^*]|\*+[^*/])*\*+\/
ESPACIOS        [ \t\r\n]+

%%

{ESPACIOS}        { /* Ignorar */ }
{COMENT_LINEA}    { /* Ignorar */ }
{COMENT_BLOQUE}   { /* Ignorar */ }

  /* Palabras Clave (Keywords) */
"inicio"          { return KW_INICIO; }
"sino"            { return KW_SINO; }
"cuando"          { return KW_CUANDO; }
"loop"            { return KW_LOOP; }
"imprimir"        { return KW_IMPRIMIR; }
"retornar"        { return KW_RETORNAR; }
"num"             { return KW_NUM; }
"dec"             { return KW_DEC; }
"text"            { return KW_TEXT; }
"verdadero"       { return LIT_BOOLEANO; }
"falso"           { return LIT_BOOLEANO; }

  /* Operadores Relacionales */
"=="              { return OP_IGUAL; }
"!="              { return OP_DIFERENTE; }
"<="              { return OP_MENOR_IGUAL; }
">="              { return OP_MAYOR_IGUAL; }
"<"               { return OP_MENOR; }
">"               { return OP_MAYOR; }

  /* Operadores Lógicos */
"&&"              { return OP_AND; }
"||"              { return OP_OR; }
"!"               { return OP_NOT; }

  /* Operadores Aritméticos */
"+"               { return OP_SUMA; }
"-"               { return OP_RESTA; }
"*"               { return OP_MULT; }
"/"               { return OP_DIV; }
"%"               { return OP_MOD; }

  /* Operador de Asignación */
"="               { return OP_ASIG; }

  /* Delimitadores y Puntuación */
";"               { return PUNTO_COMA; }
","               { return COMA; }
"("               { return PARENTESIS_ABRIR; }
")"               { return PARENTESIS_CERRAR; }
"{"               { return LLAVE_ABRIR; }
"}"               { return LLAVE_CERRAR; }

  /* Literales Dinámicos e Identificadores */
{LIT_DECIMAL}     { return LIT_DECIMAL; }
{LIT_ENTERO}      { return LIT_ENTERO; }
{IDENTIFICADOR}   { return ID; }
{LIT_CADENA}      { return LIT_TEXT; }

  /* Error Léxico: cualquier otro carácter no coincidente */
.                 { printf("[ERROR LEXICO] Linea %d: Caracter no reconocido '%s'\n", yylineno, yytext); return yytext[0]; }

%%
