/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 5 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "simbolos.h"
#include "semantico.h"
#include "gui_salida.h"
#include "codigo_intermedio.h"
#include "generador_z80.h"

extern int yylex();
extern int yyparse();
extern FILE *yyin;
extern int yylineno;

void yyerror(const char *s);

/* ── Modo de salida ──────────────────────────────────────────
   modo_gui = 1  -> el compilador emite el formato delimitado (@@...)
                    que consume la interfaz grafica (sin JSON en C).
   modo_gui = 0  -> salida legible para consola (comportamiento normal). */
int modo_gui = 0;

/* lexer_verbose lo define el lexer (lex.yy.c). En modo --gui se apaga
   para que el flujo solo contenga las secciones @@. */
extern int lexer_verbose;

/* Estado del error sintactico capturado (para el modo --gui). */
static int  hubo_error_sint = 0;
static int  linea_error_sint = 0;
static char msg_error_sint[256] = "";

/* Resultado del semantico capturado antes de liberar las estructuras. */
static int sem_con_errores = 0;

int ends_with_g5z80(const char *filename) {
    int len = strlen(filename);
    if (len < 6) return 0;
    const char *ext = filename + len - 6;
    return (ext[0] == '.' &&
            (ext[1] == 'g' || ext[1] == 'G') &&
            (ext[2] == '5') &&
            (ext[3] == 'z' || ext[3] == 'Z') &&
            (ext[4] == '8') &&
            (ext[5] == '0'));
}

#line 120 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_KW_INICIO = 3,                  /* KW_INICIO  */
  YYSYMBOL_KW_CUANDO = 4,                  /* KW_CUANDO  */
  YYSYMBOL_KW_SINO = 5,                    /* KW_SINO  */
  YYSYMBOL_KW_LOOP = 6,                    /* KW_LOOP  */
  YYSYMBOL_KW_NUM = 7,                     /* KW_NUM  */
  YYSYMBOL_KW_DEC = 8,                     /* KW_DEC  */
  YYSYMBOL_KW_TEXT = 9,                    /* KW_TEXT  */
  YYSYMBOL_KW_IMPRIMIR = 10,               /* KW_IMPRIMIR  */
  YYSYMBOL_KW_RETORNAR = 11,               /* KW_RETORNAR  */
  YYSYMBOL_OP_ASIGNACION = 12,             /* OP_ASIGNACION  */
  YYSYMBOL_OP_SUMA = 13,                   /* OP_SUMA  */
  YYSYMBOL_OP_RESTA = 14,                  /* OP_RESTA  */
  YYSYMBOL_OP_MULT = 15,                   /* OP_MULT  */
  YYSYMBOL_OP_DIV = 16,                    /* OP_DIV  */
  YYSYMBOL_OP_MOD = 17,                    /* OP_MOD  */
  YYSYMBOL_OP_IGUAL = 18,                  /* OP_IGUAL  */
  YYSYMBOL_OP_DIFERENTE = 19,              /* OP_DIFERENTE  */
  YYSYMBOL_OP_MAYOR = 20,                  /* OP_MAYOR  */
  YYSYMBOL_OP_MENOR = 21,                  /* OP_MENOR  */
  YYSYMBOL_OP_MAYOR_IGUAL = 22,            /* OP_MAYOR_IGUAL  */
  YYSYMBOL_OP_MENOR_IGUAL = 23,            /* OP_MENOR_IGUAL  */
  YYSYMBOL_OP_AND = 24,                    /* OP_AND  */
  YYSYMBOL_OP_OR = 25,                     /* OP_OR  */
  YYSYMBOL_OP_NOT = 26,                    /* OP_NOT  */
  YYSYMBOL_DELIM_PAR_IZQ = 27,             /* DELIM_PAR_IZQ  */
  YYSYMBOL_DELIM_PAR_DER = 28,             /* DELIM_PAR_DER  */
  YYSYMBOL_DELIM_LLAVE_IZQ = 29,           /* DELIM_LLAVE_IZQ  */
  YYSYMBOL_DELIM_LLAVE_DER = 30,           /* DELIM_LLAVE_DER  */
  YYSYMBOL_DELIM_PUNTO_COMA = 31,          /* DELIM_PUNTO_COMA  */
  YYSYMBOL_TOKEN_COMENT = 32,              /* TOKEN_COMENT  */
  YYSYMBOL_TOKEN_ERROR = 33,               /* TOKEN_ERROR  */
  YYSYMBOL_IDENTIFICADOR = 34,             /* IDENTIFICADOR  */
  YYSYMBOL_LIT_TEXT = 35,                  /* LIT_TEXT  */
  YYSYMBOL_LIT_DECIMAL = 36,               /* LIT_DECIMAL  */
  YYSYMBOL_LIT_ENTERO = 37,                /* LIT_ENTERO  */
  YYSYMBOL_LIT_BOOLEANO = 38,              /* LIT_BOOLEANO  */
  YYSYMBOL_UMINUS = 39,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 40,                  /* $accept  */
  YYSYMBOL_programa = 41,                  /* programa  */
  YYSYMBOL_sentencias = 42,                /* sentencias  */
  YYSYMBOL_sentencia = 43,                 /* sentencia  */
  YYSYMBOL_declaracion = 44,               /* declaracion  */
  YYSYMBOL_tipo = 45,                      /* tipo  */
  YYSYMBOL_asignacion = 46,                /* asignacion  */
  YYSYMBOL_condicional = 47,               /* condicional  */
  YYSYMBOL_ciclo = 48,                     /* ciclo  */
  YYSYMBOL_impresion = 49,                 /* impresion  */
  YYSYMBOL_retorno = 50,                   /* retorno  */
  YYSYMBOL_condicion = 51,                 /* condicion  */
  YYSYMBOL_operador_rel = 52,              /* operador_rel  */
  YYSYMBOL_expresion = 53                  /* expresion  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   110

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  40
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  14
/* YYNRULES -- Number of rules.  */
#define YYNRULES  45
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  89

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   294


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   102,   102,   162,   167,   173,   174,   175,   176,   177,
     178,   182,   186,   193,   194,   195,   199,   206,   211,   220,
     228,   235,   239,   246,   250,   254,   258,   262,   269,   270,
     271,   272,   273,   274,   278,   279,   280,   281,   282,   283,
     284,   285,   286,   287,   288,   289
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "KW_INICIO",
  "KW_CUANDO", "KW_SINO", "KW_LOOP", "KW_NUM", "KW_DEC", "KW_TEXT",
  "KW_IMPRIMIR", "KW_RETORNAR", "OP_ASIGNACION", "OP_SUMA", "OP_RESTA",
  "OP_MULT", "OP_DIV", "OP_MOD", "OP_IGUAL", "OP_DIFERENTE", "OP_MAYOR",
  "OP_MENOR", "OP_MAYOR_IGUAL", "OP_MENOR_IGUAL", "OP_AND", "OP_OR",
  "OP_NOT", "DELIM_PAR_IZQ", "DELIM_PAR_DER", "DELIM_LLAVE_IZQ",
  "DELIM_LLAVE_DER", "DELIM_PUNTO_COMA", "TOKEN_COMENT", "TOKEN_ERROR",
  "IDENTIFICADOR", "LIT_TEXT", "LIT_DECIMAL", "LIT_ENTERO", "LIT_BOOLEANO",
  "UMINUS", "$accept", "programa", "sentencias", "sentencia",
  "declaracion", "tipo", "asignacion", "condicional", "ciclo", "impresion",
  "retorno", "condicion", "operador_rel", "expresion", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-22)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-28)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
       1,     0,    19,    -1,   -22,    -5,     5,   -22,   -22,   -22,
       8,    40,    32,    16,    -1,    17,    13,    18,   -22,   -22,
      21,    22,    24,    24,    40,    40,    40,   -22,   -22,   -22,
     -22,   -22,    11,    40,   -22,   -22,   -22,    43,   -22,   -22,
     -22,    24,    -7,     6,    87,    45,    66,   -22,    71,    40,
      40,    40,    40,    40,    11,    40,   -22,    24,    24,    28,
     -22,   -22,   -22,   -22,   -22,   -22,    40,    34,   -22,   -22,
      -4,    -4,   -22,   -22,   -22,    11,   -22,    41,    -1,    11,
      -1,    38,    59,    85,   -22,    42,    -1,    61,   -22
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     4,     1,     0,     0,    13,    14,    15,
       0,    22,     0,     0,     4,     0,     0,     0,     7,     8,
       0,     0,     0,     0,     0,     0,     0,    45,    43,    42,
      41,    44,    21,     0,     2,     3,     5,    12,     6,     9,
      10,     0,    44,     0,     0,     0,     0,    39,     0,     0,
       0,     0,     0,     0,    16,     0,    24,     0,     0,     0,
      32,    33,    28,    29,    30,    31,     0,     0,    20,    40,
      34,    35,    36,    37,    38,    11,    25,    26,     4,    23,
       4,     0,     0,    17,    19,     0,     4,     0,    18
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -22,   -22,   -14,   -22,   -22,   -22,   -22,   -22,   -22,   -22,
     -22,   -21,   -22,   -10
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     2,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    43,    66,    44
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      35,    32,    45,     5,     1,     6,     7,     8,     9,    10,
      11,    51,    52,    53,    46,    47,    48,   -27,   -27,     4,
      56,   -27,    22,    54,    49,    50,    51,    52,    53,     3,
      57,    58,    23,    12,    59,    24,    76,    77,    25,    70,
      71,    72,    73,    74,    33,    75,    34,    37,    36,    38,
      41,    26,    39,    40,    25,    55,    79,    78,    27,    28,
      29,    30,    42,    80,    81,    57,    82,    26,    83,    57,
      58,    86,    87,    67,    27,    28,    29,    30,    31,    49,
      50,    51,    52,    53,    49,    50,    51,    52,    53,    84,
      85,    88,     0,     0,    68,     0,     0,     0,     0,    69,
      49,    50,    51,    52,    53,    60,    61,    62,    63,    64,
      65
};

static const yytype_int8 yycheck[] =
{
      14,    11,    23,     4,     3,     6,     7,     8,     9,    10,
      11,    15,    16,    17,    24,    25,    26,    24,    25,     0,
      41,    28,    27,    33,    13,    14,    15,    16,    17,    29,
      24,    25,    27,    34,    28,    27,    57,    58,    14,    49,
      50,    51,    52,    53,    12,    55,    30,    34,    31,    31,
      26,    27,    31,    31,    14,    12,    66,    29,    34,    35,
      36,    37,    38,    29,    78,    24,    80,    27,    30,    24,
      25,    29,    86,    28,    34,    35,    36,    37,    38,    13,
      14,    15,    16,    17,    13,    14,    15,    16,    17,    30,
       5,    30,    -1,    -1,    28,    -1,    -1,    -1,    -1,    28,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,    41,    29,     0,     4,     6,     7,     8,     9,
      10,    11,    34,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    27,    27,    27,    14,    27,    34,    35,    36,
      37,    38,    53,    12,    30,    42,    31,    34,    31,    31,
      31,    26,    38,    51,    53,    51,    53,    53,    53,    13,
      14,    15,    16,    17,    53,    12,    51,    24,    25,    28,
      18,    19,    20,    21,    22,    23,    52,    28,    28,    28,
      53,    53,    53,    53,    53,    53,    51,    51,    29,    53,
      29,    42,    42,    30,    30,     5,    29,    42,    30
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    40,    41,    42,    42,    43,    43,    43,    43,    43,
      43,    44,    44,    45,    45,    45,    46,    47,    47,    48,
      49,    50,    50,    51,    51,    51,    51,    51,    52,    52,
      52,    52,    52,    52,    53,    53,    53,    53,    53,    53,
      53,    53,    53,    53,    53,    53
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     4,     2,     0,     2,     2,     1,     1,     2,
       2,     4,     2,     1,     1,     1,     3,     7,    11,     7,
       4,     2,     1,     3,     2,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     2,
       3,     1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* programa: KW_INICIO DELIM_LLAVE_IZQ sentencias DELIM_LLAVE_DER  */
#line 103 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        NodoAST *raiz = nuevo_nodo(NODO_PROGRAMA);
        raiz->izq = (yyvsp[-1].nodo);

        if (modo_gui) {
            /* Salida delimitada para la interfaz grafica. */
            printf("@@AST\n");
            emitir_ast_gui(raiz);

            analizar_semantico(raiz);
            sem_con_errores = hay_errores_sem();

            printf("@@SIMBOLOS\n");
            emitir_simbolos_gui();
            printf("@@ERRORES\n");
            emitir_errores_gui();

            /* Fases 4 y 5: codigo intermedio y Z80 (solo sin errores). */
            printf("@@INTERMEDIO\n");
            if (!sem_con_errores) {
                generar_tac(raiz);
                emitir_tac_gui();
            }
            printf("@@Z80\n");
            if (!sem_con_errores) {
                emitir_z80_gui();
                liberar_tac();
            }
        } else {
            /* Salida legible para consola. */
            printf("\n==================================================\n");
            printf("ARBOL DE SINTAXIS ABSTRACTA (AST)\n");
            printf("==================================================\n\n");
            imprimir_ast(raiz, 0);
            printf("\n==================================================\n");
            printf("FIN DEL AST\n");
            printf("==================================================\n");

            /* Analisis semantico: tabla de simbolos + verificacion de tipos */
            analizar_semantico(raiz);
            imprimir_tabla();
            imprimir_errores_sem();

            /* Fases 4 y 5: codigo intermedio (TAC) y ensamblador Z80 */
            if (!hay_errores_sem()) {
                generar_tac(raiz);
                imprimir_tac();
                imprimir_z80();
                liberar_tac();
            }
        }

        liberar_errores_sem();
        liberar_tabla();
        liberar_ast(raiz);
    }
#line 1274 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 3: /* sentencias: sentencia sentencias  */
#line 163 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        (yyval.nodo) = nodo_lista((yyvsp[-1].nodo), (yyvsp[0].nodo));
    }
#line 1282 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 4: /* sentencias: %empty  */
#line 167 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        (yyval.nodo) = NULL;
    }
#line 1290 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 5: /* sentencia: declaracion DELIM_PUNTO_COMA  */
#line 173 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                    { (yyval.nodo) = (yyvsp[-1].nodo); }
#line 1296 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 6: /* sentencia: asignacion DELIM_PUNTO_COMA  */
#line 174 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                    { (yyval.nodo) = (yyvsp[-1].nodo); }
#line 1302 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 7: /* sentencia: condicional  */
#line 175 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                    { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1308 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 8: /* sentencia: ciclo  */
#line 176 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                    { (yyval.nodo) = (yyvsp[0].nodo); }
#line 1314 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 9: /* sentencia: impresion DELIM_PUNTO_COMA  */
#line 177 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                    { (yyval.nodo) = (yyvsp[-1].nodo); }
#line 1320 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 10: /* sentencia: retorno DELIM_PUNTO_COMA  */
#line 178 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                    { (yyval.nodo) = (yyvsp[-1].nodo); }
#line 1326 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 11: /* declaracion: tipo IDENTIFICADOR OP_ASIGNACION expresion  */
#line 183 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        (yyval.nodo) = nodo_declaracion((yyvsp[-3].str), (yyvsp[-2].str), (yyvsp[0].nodo));
    }
#line 1334 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 12: /* declaracion: tipo IDENTIFICADOR  */
#line 187 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        (yyval.nodo) = nodo_declaracion((yyvsp[-1].str), (yyvsp[0].str), NULL);
    }
#line 1342 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 13: /* tipo: KW_NUM  */
#line 193 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
              { (yyval.str) = "num";  }
#line 1348 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 14: /* tipo: KW_DEC  */
#line 194 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
              { (yyval.str) = "dec";  }
#line 1354 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 15: /* tipo: KW_TEXT  */
#line 195 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
              { (yyval.str) = "text"; }
#line 1360 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 16: /* asignacion: IDENTIFICADOR OP_ASIGNACION expresion  */
#line 200 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        (yyval.nodo) = nodo_asignacion((yyvsp[-2].str), (yyvsp[0].nodo));
    }
#line 1368 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 17: /* condicional: KW_CUANDO DELIM_PAR_IZQ condicion DELIM_PAR_DER DELIM_LLAVE_IZQ sentencias DELIM_LLAVE_DER  */
#line 208 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        (yyval.nodo) = nodo_condicional((yyvsp[-4].nodo), (yyvsp[-1].nodo), NULL);
    }
#line 1376 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 18: /* condicional: KW_CUANDO DELIM_PAR_IZQ condicion DELIM_PAR_DER DELIM_LLAVE_IZQ sentencias DELIM_LLAVE_DER KW_SINO DELIM_LLAVE_IZQ sentencias DELIM_LLAVE_DER  */
#line 214 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        (yyval.nodo) = nodo_condicional((yyvsp[-8].nodo), (yyvsp[-5].nodo), (yyvsp[-1].nodo));
    }
#line 1384 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 19: /* ciclo: KW_LOOP DELIM_PAR_IZQ condicion DELIM_PAR_DER DELIM_LLAVE_IZQ sentencias DELIM_LLAVE_DER  */
#line 222 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        (yyval.nodo) = nodo_ciclo((yyvsp[-4].nodo), (yyvsp[-1].nodo));
    }
#line 1392 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 20: /* impresion: KW_IMPRIMIR DELIM_PAR_IZQ expresion DELIM_PAR_DER  */
#line 229 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        (yyval.nodo) = nodo_impresion((yyvsp[-1].nodo));
    }
#line 1400 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 21: /* retorno: KW_RETORNAR expresion  */
#line 236 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        (yyval.nodo) = nodo_retorno((yyvsp[0].nodo));
    }
#line 1408 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 22: /* retorno: KW_RETORNAR  */
#line 240 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        (yyval.nodo) = nodo_retorno(NULL);
    }
#line 1416 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 23: /* condicion: expresion operador_rel expresion  */
#line 247 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        (yyval.nodo) = nodo_op_binario((yyvsp[-1].str), (yyvsp[-2].nodo), (yyvsp[0].nodo));
    }
#line 1424 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 24: /* condicion: OP_NOT condicion  */
#line 251 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        (yyval.nodo) = nodo_op_unario("!", (yyvsp[0].nodo));
    }
#line 1432 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 25: /* condicion: condicion OP_AND condicion  */
#line 255 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        (yyval.nodo) = nodo_op_binario("&&", (yyvsp[-2].nodo), (yyvsp[0].nodo));
    }
#line 1440 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 26: /* condicion: condicion OP_OR condicion  */
#line 259 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        (yyval.nodo) = nodo_op_binario("||", (yyvsp[-2].nodo), (yyvsp[0].nodo));
    }
#line 1448 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 27: /* condicion: LIT_BOOLEANO  */
#line 263 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
    {
        (yyval.nodo) = nodo_booleano((yyvsp[0].entero));
    }
#line 1456 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 28: /* operador_rel: OP_MAYOR  */
#line 269 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                     { (yyval.str) = ">"; }
#line 1462 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 29: /* operador_rel: OP_MENOR  */
#line 270 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                     { (yyval.str) = "<"; }
#line 1468 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 30: /* operador_rel: OP_MAYOR_IGUAL  */
#line 271 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                     { (yyval.str) = ">="; }
#line 1474 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 31: /* operador_rel: OP_MENOR_IGUAL  */
#line 272 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                     { (yyval.str) = "<="; }
#line 1480 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 32: /* operador_rel: OP_IGUAL  */
#line 273 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                     { (yyval.str) = "=="; }
#line 1486 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 33: /* operador_rel: OP_DIFERENTE  */
#line 274 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                     { (yyval.str) = "!="; }
#line 1492 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 34: /* expresion: expresion OP_SUMA expresion  */
#line 278 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                               { (yyval.nodo) = nodo_op_binario("+",  (yyvsp[-2].nodo), (yyvsp[0].nodo)); }
#line 1498 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 35: /* expresion: expresion OP_RESTA expresion  */
#line 279 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                               { (yyval.nodo) = nodo_op_binario("-",  (yyvsp[-2].nodo), (yyvsp[0].nodo)); }
#line 1504 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 36: /* expresion: expresion OP_MULT expresion  */
#line 280 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                               { (yyval.nodo) = nodo_op_binario("*",  (yyvsp[-2].nodo), (yyvsp[0].nodo)); }
#line 1510 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 37: /* expresion: expresion OP_DIV expresion  */
#line 281 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                               { (yyval.nodo) = nodo_op_binario("/",  (yyvsp[-2].nodo), (yyvsp[0].nodo)); }
#line 1516 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 38: /* expresion: expresion OP_MOD expresion  */
#line 282 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                               { (yyval.nodo) = nodo_op_binario("%",  (yyvsp[-2].nodo), (yyvsp[0].nodo)); }
#line 1522 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 39: /* expresion: OP_RESTA expresion  */
#line 283 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                               { (yyval.nodo) = nodo_op_unario("-",   (yyvsp[0].nodo));     }
#line 1528 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 40: /* expresion: DELIM_PAR_IZQ expresion DELIM_PAR_DER  */
#line 284 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                              { (yyval.nodo) = (yyvsp[-1].nodo); }
#line 1534 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 41: /* expresion: LIT_ENTERO  */
#line 285 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                               { (yyval.nodo) = nodo_entero((yyvsp[0].entero));    }
#line 1540 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 42: /* expresion: LIT_DECIMAL  */
#line 286 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                               { (yyval.nodo) = nodo_decimal((yyvsp[0].str));   }
#line 1546 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 43: /* expresion: LIT_TEXT  */
#line 287 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                               { (yyval.nodo) = nodo_texto((yyvsp[0].str));     }
#line 1552 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 44: /* expresion: LIT_BOOLEANO  */
#line 288 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                               { (yyval.nodo) = nodo_booleano((yyvsp[0].entero));  }
#line 1558 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;

  case 45: /* expresion: IDENTIFICADOR  */
#line 289 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"
                                               { (yyval.nodo) = nodo_id((yyvsp[0].str));        }
#line 1564 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"
    break;


#line 1568 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 292 "C:\\Users\\Steveen\\Documents\\Proyectos\\Proyecto-LyC\\src\\parser.y"


/* ----- FUNCIONES DE SOPORTE ------ */

void yyerror(const char *s) {
    if (modo_gui) {
        /* En modo grafico se captura el error para emitirlo en @@ERRORES. */
        hubo_error_sint  = 1;
        linea_error_sint = yylineno;
        snprintf(msg_error_sint, sizeof(msg_error_sint), "%s", s);
        return;
    }
    fprintf(stderr, "\n[ERROR SINTACTICO] Linea %d: %s\n\n", yylineno, s);
}

/* ── Flujo del compilador en modo grafico (--gui) ─────────────
   Emite, en una sola pasada, las secciones que consume la GUI:
     @@TOKENS / @@AST / @@SIMBOLOS / @@ERRORES / @@ESTADO / @@END
   Las tres centrales las produce la regla 'programa'; aqui se anaden
   los tokens (pre-pasada lexica) y el estado global de cada fase. */
static int correr_gui(void) {
    printf("@@TOKENS\n");
    emitir_tokens_gui();          /* recorre el archivo y lo rebobina */

    int ok = (yyparse() == 0);    /* la regla 'programa' emite @@AST.. si ok */

    if (!ok) {
        /* Con error sintactico la regla 'programa' no llega a ejecutarse:
           se emiten las secciones vacias y el error capturado. */
        printf("@@AST\n@@SIMBOLOS\n@@ERRORES\n");
        if (hubo_error_sint)
            printf("sintactico|%d|%s\n", linea_error_sint, msg_error_sint);
        else
            printf("sintactico|%d|error de sintaxis\n", yylineno);
        printf("@@INTERMEDIO\n@@Z80\n");
    }

    printf("@@ESTADO\n");
    printf("sintactico|%s\n", ok ? "ok" : "error");
    if (!ok)                  printf("semantico|na\nintermedio|na\nz80|na\n");
    else if (sem_con_errores) printf("semantico|error\nintermedio|na\nz80|na\n");
    else                      printf("semantico|ok\nintermedio|ok\nz80|ok\n");
    printf("@@END\n");
    return 0;
}

int main(int argc, char **argv) {
    const char *ruta = NULL;

    /* Procesa argumentos: bandera --gui y la ruta del archivo. */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--gui") == 0) modo_gui = 1;
        else                               ruta = argv[i];
    }

    if (!ruta) {
        printf("Por favor ingresa un archivo de texto con extension '.g5z80'.\n");
        printf("Ejemplo: .\\compilador.exe ejemplo1.g5z80   (o --gui para la interfaz)\n");
        return 1;
    }
    if (!ends_with_g5z80(ruta)) {
        fprintf(stderr, "\n[ERROR DE EXTENSION] El compilador solo acepta archivos con extension '.g5z80'.\n");
        fprintf(stderr, "Ejemplo de uso: .\\compilador.exe ejemplo1.g5z80\n\n");
        return 1;
    }

    yyin = fopen(ruta, "r");
    if (!yyin) {
        perror(ruta);
        return 1;
    }

    if (modo_gui) {
        lexer_verbose = 0;        /* sin trazas del lexer en el flujo @@ */
        int r = correr_gui();
        fclose(yyin);
        return r;
    }

    printf("\n==================================================\n");
    printf("Iniciando analisis de: %s\n", ruta);
    printf("==================================================\n\n");

    if (yyparse() == 0) {
        printf("\n==================================================\n");
        printf("ANALISIS SINTACTICO Y AST COMPLETADOS EXITOSAMENTE\n");
        printf("==================================================\n");
    } else {
        printf("\n==================================================\n");
        printf("ANALISIS CONCLUIDO CON ERRORES SINTACTICOS\n");
        printf("==================================================\n");
    }

    fclose(yyin);
    return 0;
}
