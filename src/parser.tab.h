/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SRC_PARSER_TAB_H_INCLUDED
# define YY_YY_SRC_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    KW_INICIO = 258,               /* KW_INICIO  */
    KW_SINO = 259,                 /* KW_SINO  */
    KW_CUANDO = 260,               /* KW_CUANDO  */
    KW_LOOP = 261,                 /* KW_LOOP  */
    KW_IMPRIMIR = 262,             /* KW_IMPRIMIR  */
    KW_RETORNAR = 263,             /* KW_RETORNAR  */
    KW_NUM = 264,                  /* KW_NUM  */
    KW_DEC = 265,                  /* KW_DEC  */
    KW_TEXT = 266,                 /* KW_TEXT  */
    LIT_BOOLEANO = 267,            /* LIT_BOOLEANO  */
    LIT_ENTERO = 268,              /* LIT_ENTERO  */
    LIT_DECIMAL = 269,             /* LIT_DECIMAL  */
    LIT_TEXT = 270,                /* LIT_TEXT  */
    ID = 271,                      /* ID  */
    OP_ASIG = 272,                 /* OP_ASIG  */
    OP_SUMA = 273,                 /* OP_SUMA  */
    OP_RESTA = 274,                /* OP_RESTA  */
    OP_MULT = 275,                 /* OP_MULT  */
    OP_DIV = 276,                  /* OP_DIV  */
    OP_MOD = 277,                  /* OP_MOD  */
    OP_IGUAL = 278,                /* OP_IGUAL  */
    OP_DIFERENTE = 279,            /* OP_DIFERENTE  */
    OP_MAYOR = 280,                /* OP_MAYOR  */
    OP_MENOR = 281,                /* OP_MENOR  */
    OP_MAYOR_IGUAL = 282,          /* OP_MAYOR_IGUAL  */
    OP_MENOR_IGUAL = 283,          /* OP_MENOR_IGUAL  */
    OP_AND = 284,                  /* OP_AND  */
    OP_OR = 285,                   /* OP_OR  */
    OP_NOT = 286,                  /* OP_NOT  */
    PUNTO_COMA = 287,              /* PUNTO_COMA  */
    COMA = 288,                    /* COMA  */
    PARENTESIS_ABRIR = 289,        /* PARENTESIS_ABRIR  */
    PARENTESIS_CERRAR = 290,       /* PARENTESIS_CERRAR  */
    LLAVE_ABRIR = 291,             /* LLAVE_ABRIR  */
    LLAVE_CERRAR = 292             /* LLAVE_CERRAR  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SRC_PARSER_TAB_H_INCLUDED  */
