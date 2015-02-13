/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     CLASS = 258,
     VAR = 259,
     EXTENDS = 260,
     IS = 261,
     DEF = 262,
     OVERRIDE = 263,
     RETURNS = 264,
     YIELD = 265,
     IF = 266,
     THEN = 267,
     ELSE = 268,
     NEWO = 269,
     PLUS = 270,
     MINUS = 271,
     AFFECT = 272,
     MUL = 273,
     DIV = 274,
     CONCAT = 275,
     ID = 276,
     CSTS = 277,
     IDCLASS = 278,
     RETURN = 279,
     STATIC = 280,
     CSTE = 281,
     RELOP = 282,
     unaire = 283
   };
#endif
/* Tokens.  */
#define CLASS 258
#define VAR 259
#define EXTENDS 260
#define IS 261
#define DEF 262
#define OVERRIDE 263
#define RETURNS 264
#define YIELD 265
#define IF 266
#define THEN 267
#define ELSE 268
#define NEWO 269
#define PLUS 270
#define MINUS 271
#define AFFECT 272
#define MUL 273
#define DIV 274
#define CONCAT 275
#define ID 276
#define CSTS 277
#define IDCLASS 278
#define RETURN 279
#define STATIC 280
#define CSTE 281
#define RELOP 282
#define unaire 283




/* Copy the first part of user declarations.  */
#line 47 "tp.y"

#include "tp.h"     /* les definition des types et les etiquettes des noeuds */
PCLASS classActuel; /* Classe en cours d'analyse*/
TreeP programme;

extern int yylex(); /* fournie par Flex */
extern void yyerror();  /* definie dans tp.c */



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 172 "tp_y.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   205

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  37
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  34
/* YYNRULES -- Number of rules.  */
#define YYNRULES  75
/* YYNRULES -- Number of states.  */
#define YYNSTATES  158

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   283

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      34,    35,     2,     2,    36,     2,    28,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    32,    33,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    30,     2,    31,     2,     2,     2,     2,
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
      25,    26,    27,    29
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     6,     9,    10,    14,    17,    22,    25,
      26,    35,    38,    39,    42,    45,    49,    51,    56,    63,
      66,    68,    70,    72,    73,    76,    87,    90,    99,   100,
     102,   103,   106,   107,   117,   120,   121,   123,   125,   126,
     128,   130,   132,   133,   135,   139,   143,   149,   150,   151,
     153,   155,   159,   162,   165,   169,   173,   177,   181,   185,
     189,   191,   193,   195,   197,   201,   203,   207,   211,   215,
     217,   219,   225,   232,   239,   246
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      38,     0,    -1,    39,    40,    -1,    50,    39,    -1,    -1,
      30,    41,    31,    -1,    44,    42,    -1,    43,     6,    45,
      42,    -1,    10,    65,    -1,    -1,     4,    53,    21,    32,
      23,    54,    33,    52,    -1,    46,    44,    -1,    -1,    46,
      44,    -1,    65,    33,    -1,    24,    65,    33,    -1,    40,
      -1,    47,    17,    65,    33,    -1,    11,    65,    12,    46,
      13,    46,    -1,    24,    33,    -1,    21,    -1,    67,    -1,
      40,    -1,    -1,     3,    23,    -1,    49,    34,    59,    35,
      62,    48,     6,    30,    51,    31,    -1,    52,    56,    -1,
       4,    53,    21,    32,    23,    54,    33,    52,    -1,    -1,
      25,    -1,    -1,    17,    65,    -1,    -1,     7,    57,    21,
      34,    59,    35,     9,    23,    58,    -1,    55,    56,    -1,
      -1,     8,    -1,    25,    -1,    -1,    54,    -1,    40,    -1,
      60,    -1,    -1,    61,    -1,    61,    36,    60,    -1,    21,
      32,    23,    -1,     5,    23,    34,    63,    35,    -1,    -1,
      -1,    64,    -1,    65,    -1,    64,    36,    65,    -1,    15,
      65,    -1,    16,    65,    -1,    65,    20,    65,    -1,    65,
      15,    65,    -1,    65,    16,    65,    -1,    65,    19,    65,
      -1,    65,    18,    65,    -1,    65,    27,    65,    -1,    68,
      -1,    69,    -1,    70,    -1,    66,    -1,    34,    65,    35,
      -1,    47,    -1,    23,    28,    21,    -1,    70,    28,    21,
      -1,    66,    28,    21,    -1,    22,    -1,    26,    -1,    14,
      23,    34,    63,    35,    -1,    23,    28,    21,    34,    63,
      35,    -1,    70,    28,    21,    34,    63,    35,    -1,    66,
      28,    21,    34,    63,    35,    -1,    68,    28,    21,    34,
      63,    35,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    85,    85,    92,    93,    99,   109,   110,   117,   118,
     121,   132,   133,   138,   149,   150,   151,   152,   153,   154,
     163,   164,   170,   171,   178,   211,   365,   371,   373,   377,
     378,   382,   383,   391,   396,   397,   401,   402,   403,   407,
     408,   411,   412,   415,   416,   419,   422,   471,   474,   475,
     479,   480,   484,   485,   486,   487,   488,   489,   490,   491,
     492,   493,   494,   495,   498,   499,   507,   508,   509,   515,
     516,   519,   527,   529,   531,   533
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "CLASS", "VAR", "EXTENDS", "IS", "DEF",
  "OVERRIDE", "RETURNS", "YIELD", "IF", "THEN", "ELSE", "NEWO", "PLUS",
  "MINUS", "AFFECT", "MUL", "DIV", "CONCAT", "ID", "CSTS", "IDCLASS",
  "RETURN", "STATIC", "CSTE", "RELOP", "'.'", "unaire", "'{'", "'}'",
  "':'", "';'", "'('", "')'", "','", "$accept", "Programme", "LClassOpt",
  "Bloc", "ContenuBloc", "YieldOpt", "ListDeclVar", "LInstructionOpt",
  "LInstruction", "Instruction", "Cible", "BlocOpt", "DefClass",
  "DeclClass", "ContenuClassOpt", "LDeclChampsOpt", "StaticOpt",
  "AffectExprOpt", "Methode", "LDeclMethodeOpt", "OverrideOuStaticOpt",
  "BlocOuExpr", "ListParamOpt", "LParam", "Param", "ListExtendsOpt",
  "ListOptArg", "LArg", "expr", "OuRien", "selection", "constante",
  "instanciation", "envoiMessage", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,    46,   283,
     123,   125,    58,    59,    40,    41,    44
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    37,    38,    39,    39,    40,    41,    41,    42,    42,
      43,    44,    44,    45,    46,    46,    46,    46,    46,    46,
      47,    47,    48,    48,    49,    50,    51,    52,    52,    53,
      53,    54,    54,    55,    56,    56,    57,    57,    57,    58,
      58,    59,    59,    60,    60,    61,    62,    62,    63,    63,
      64,    64,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    66,    66,    67,    67,    67,    68,
      68,    69,    70,    70,    70,    70
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     0,     3,     2,     4,     2,     0,
       8,     2,     0,     2,     2,     3,     1,     4,     6,     2,
       1,     1,     1,     0,     2,    10,     2,     8,     0,     1,
       0,     2,     0,     9,     2,     0,     1,     1,     0,     1,
       1,     1,     0,     1,     3,     3,     5,     0,     0,     1,
       1,     3,     2,     2,     3,     3,     3,     3,     3,     3,
       1,     1,     1,     1,     3,     1,     3,     3,     3,     1,
       1,     5,     6,     6,     6,     6
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,     0,     0,     0,     4,    24,     1,    12,     2,
      42,     3,    30,     0,     0,     0,     0,    20,    69,     0,
       0,    70,     0,    16,     0,     0,     9,    12,    65,     0,
      63,    21,    60,    61,    62,     0,     0,    41,    43,    29,
       0,    65,     0,     0,    52,    53,     0,    19,     0,     0,
       5,     0,     0,     6,    11,     0,     0,     0,     0,     0,
       0,     0,    14,     0,     0,     0,     0,    47,     0,     0,
       0,    48,    66,    15,    64,     9,    12,     8,     0,    55,
      56,    58,    57,    54,    59,    68,     0,    67,    45,     0,
      23,    44,     0,     0,     0,    49,    50,    48,     7,    13,
      17,    48,    48,    48,     0,    22,     0,    32,     0,    71,
       0,     0,     0,     0,     0,    48,     0,     0,     0,    18,
      51,    72,    74,    75,    73,     0,    28,    31,    28,    46,
      30,     0,    35,    10,     0,    25,    38,    35,    26,     0,
      36,    37,     0,    34,     0,     0,    32,    42,     0,     0,
      28,     0,    27,     0,    32,    40,    39,    33
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,    23,    24,    53,    25,    26,    75,    27,
      41,   106,     4,     5,   131,   132,    40,   118,   137,   138,
     142,   157,    36,    37,    38,    90,    94,    95,    29,    30,
      31,    32,    33,    34
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -117
static const yytype_int16 yypact[] =
{
       3,     9,    20,     5,    21,     3,  -117,  -117,     7,  -117,
      18,  -117,    17,    94,    31,    94,    94,  -117,  -117,    33,
      78,  -117,    94,  -117,    36,    71,    68,    42,    62,   116,
      52,  -117,    53,  -117,    56,    51,    60,  -117,    50,  -117,
      67,  -117,   152,    63,  -117,  -117,    75,  -117,   126,   103,
    -117,    42,    94,  -117,  -117,    94,    94,    94,    94,    94,
      94,    94,  -117,    81,    85,    92,    91,   119,    18,    93,
      42,    94,    95,  -117,  -117,    68,    42,   158,   142,    -2,
      -2,  -117,  -117,    55,   165,    99,   105,   106,  -117,   104,
       5,  -117,   114,   113,   112,   118,   158,    94,  -117,  -117,
    -117,    94,    94,    94,   121,  -117,   144,   131,    42,  -117,
      94,   117,   128,   130,   147,    94,   136,    94,   123,  -117,
     158,  -117,  -117,  -117,  -117,   151,   183,   158,   183,  -117,
      17,   157,   182,  -117,   169,  -117,    11,   182,  -117,   159,
    -117,  -117,   172,  -117,   171,   161,   131,    18,   163,   162,
     183,   189,  -117,   176,     8,  -117,  -117,  -117
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -117,  -117,   195,    -3,  -117,   127,  -117,   -25,  -117,   -46,
      -1,  -117,  -117,  -117,  -117,  -116,    73,   -94,  -117,    64,
    -117,  -117,    57,   137,  -117,  -117,   -88,  -117,   -12,  -117,
    -117,  -117,  -117,  -117
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_int16 yytable[] =
{
       9,    42,    54,    44,    45,    76,     1,    28,    48,   111,
      49,    12,   133,   112,   113,   114,    58,    59,    13,   140,
       7,    14,    15,    16,    93,   117,    28,   125,    17,    18,
      19,    20,     6,    21,   152,     8,   141,     8,     8,    35,
      77,    22,    39,    78,    79,    80,    81,    82,    83,    84,
      28,    99,   148,    13,    43,    10,    14,    15,    16,    96,
     156,    46,   119,    17,    18,    19,    20,    50,    21,    28,
      56,    57,     8,    58,    59,    28,    22,    51,    52,    55,
      63,    64,    61,    66,    65,    96,    68,   105,    69,    96,
      96,    96,    14,    15,    16,    67,    72,    71,   120,    17,
      18,    19,    85,    96,    21,   127,    86,    28,    14,    15,
      16,    47,    22,    87,    88,    17,    18,    19,    56,    57,
      21,    58,    59,    60,    89,    92,   108,   104,    22,    97,
      61,    56,    57,   101,    58,    59,    60,   107,    74,   102,
     103,    56,    57,    61,    58,    59,    60,   109,   117,    62,
     116,   155,   121,    61,   110,   115,   128,    56,    57,    73,
      58,    59,    60,   122,    70,   123,   126,    56,    57,    61,
      58,    59,    60,    56,    57,   100,    58,    59,    60,    61,
      56,    57,   124,    58,    59,    61,   129,   130,   135,   136,
     139,   144,    -1,   145,   146,   147,   150,   151,   153,   154,
      11,   143,    98,   134,   149,    91
};

static const yytype_uint8 yycheck[] =
{
       3,    13,    27,    15,    16,    51,     3,     8,    20,    97,
      22,     4,   128,   101,   102,   103,    18,    19,    11,     8,
       0,    14,    15,    16,    70,    17,    27,   115,    21,    22,
      23,    24,    23,    26,   150,    30,    25,    30,    30,    21,
      52,    34,    25,    55,    56,    57,    58,    59,    60,    61,
      51,    76,   146,    11,    23,    34,    14,    15,    16,    71,
     154,    28,   108,    21,    22,    23,    24,    31,    26,    70,
      15,    16,    30,    18,    19,    76,    34,     6,    10,    17,
      28,    28,    27,    32,    28,    97,    36,    90,    21,   101,
     102,   103,    14,    15,    16,    35,    21,    34,   110,    21,
      22,    23,    21,   115,    26,   117,    21,   108,    14,    15,
      16,    33,    34,    21,    23,    21,    22,    23,    15,    16,
      26,    18,    19,    20,     5,    32,    13,    23,    34,    34,
      27,    15,    16,    34,    18,    19,    20,    23,    35,    34,
      34,    15,    16,    27,    18,    19,    20,    35,    17,    33,
       6,   154,    35,    27,    36,    34,    33,    15,    16,    33,
      18,    19,    20,    35,    12,    35,    30,    15,    16,    27,
      18,    19,    20,    15,    16,    33,    18,    19,    20,    27,
      15,    16,    35,    18,    19,    27,    35,     4,    31,     7,
      21,    32,    27,    21,    23,    34,    33,    35,     9,    23,
       5,   137,    75,   130,   147,    68
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,    38,    39,    49,    50,    23,     0,    30,    40,
      34,    39,     4,    11,    14,    15,    16,    21,    22,    23,
      24,    26,    34,    40,    41,    43,    44,    46,    47,    65,
      66,    67,    68,    69,    70,    21,    59,    60,    61,    25,
      53,    47,    65,    23,    65,    65,    28,    33,    65,    65,
      31,     6,    10,    42,    44,    17,    15,    16,    18,    19,
      20,    27,    33,    28,    28,    28,    32,    35,    36,    21,
      12,    34,    21,    33,    35,    45,    46,    65,    65,    65,
      65,    65,    65,    65,    65,    21,    21,    21,    23,     5,
      62,    60,    32,    46,    63,    64,    65,    34,    42,    44,
      33,    34,    34,    34,    23,    40,    48,    23,    13,    35,
      36,    63,    63,    63,    63,    34,     6,    17,    54,    46,
      65,    35,    35,    35,    35,    63,    30,    65,    33,    35,
       4,    51,    52,    52,    53,    31,     7,    55,    56,    21,
       8,    25,    57,    56,    32,    21,    23,    34,    54,    59,
      33,    35,    52,     9,    23,    40,    54,    58
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
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



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 85 "tp.y"
    {

	(yyval.T)=makeTree(PROGRAM,2,makeLeafClass(LISTCLASS,(yyvsp[(1) - (2)].CL)),(yyvsp[(2) - (2)].T));programme=(yyval.T);;}
    break;

  case 3:
#line 92 "tp.y"
    {(yyvsp[(1) - (2)].CL)->suivant=(yyvsp[(2) - (2)].CL); (yyval.CL)=(yyvsp[(1) - (2)].CL);;}
    break;

  case 4:
#line 93 "tp.y"
    {(yyval.CL)=NIL(SCLASS);;}
    break;

  case 5:
#line 99 "tp.y"
    {(yyval.T)=(yyvsp[(2) - (3)].T);;}
    break;

  case 6:
#line 109 "tp.y"
    {(yyval.T)=makeTree(CONTENUBLOC,3,NIL(Tree),(yyvsp[(1) - (2)].T),(yyvsp[(2) - (2)].T));;}
    break;

  case 7:
#line 110 "tp.y"
    {(yyval.T)=makeTree(CONTENUBLOC,3,makeLeafVar(LISTEVAR, (yyvsp[(1) - (4)].V)),(yyvsp[(3) - (4)].T),(yyvsp[(4) - (4)].T));;}
    break;

  case 8:
#line 117 "tp.y"
    {(yyval.T)=makeTree(ETIQUETTE_YIELD, 1, (yyvsp[(2) - (2)].T));;}
    break;

  case 9:
#line 118 "tp.y"
    {(yyval.T)=NIL(Tree);;}
    break;

  case 10:
#line 122 "tp.y"
    {
		(yyval.V)=makeListVar((yyvsp[(3) - (8)].S),getClasseBis(listeDeClass,(yyvsp[(5) - (8)].S)),(yyvsp[(2) - (8)].I),(yyvsp[(6) - (8)].T));
            	(yyval.V)->suivant=(yyvsp[(8) - (8)].V);
            ;}
    break;

  case 11:
#line 132 "tp.y"
    {(yyval.T)=makeTree(LIST_INSTRUCTION, 2, (yyvsp[(1) - (2)].T), (yyvsp[(2) - (2)].T));;}
    break;

  case 12:
#line 133 "tp.y"
    {(yyval.T)=NIL(Tree);;}
    break;

  case 13:
#line 138 "tp.y"
    {(yyval.T)=makeTree(LIST_INSTRUCTION, 2, (yyvsp[(1) - (2)].T), (yyvsp[(2) - (2)].T));;}
    break;

  case 14:
#line 149 "tp.y"
    {(yyval.T)=(yyvsp[(1) - (2)].T);;}
    break;

  case 15:
#line 150 "tp.y"
    {(yyval.T)=makeTree(EXPRESSIONRETURN, 1, (yyvsp[(2) - (3)].T));;}
    break;

  case 16:
#line 151 "tp.y"
    {(yyval.T)=(yyvsp[(1) - (1)].T);;}
    break;

  case 17:
#line 152 "tp.y"
    {(yyval.T)=makeTree(ETIQUETTE_AFFECT, 2, (yyvsp[(1) - (4)].T), (yyvsp[(3) - (4)].T));;}
    break;

  case 18:
#line 153 "tp.y"
    {(yyval.T)=makeTree(IFTHENELSE, 3, (yyvsp[(2) - (6)].T), (yyvsp[(4) - (6)].T), (yyvsp[(6) - (6)].T));;}
    break;

  case 19:
#line 154 "tp.y"
    {(yyval.T)=makeLeafStr(RETURN_VOID, MSG_VOID);;}
    break;

  case 20:
#line 163 "tp.y"
    {(yyval.T)=makeLeafStr(IDENTIFICATEUR,(yyvsp[(1) - (1)].S));;}
    break;

  case 21:
#line 164 "tp.y"
    {(yyval.T)=(yyvsp[(1) - (1)].T);;}
    break;

  case 22:
#line 170 "tp.y"
    {(yyval.T)=(yyvsp[(1) - (1)].T);;}
    break;

  case 23:
#line 171 "tp.y"
    {(yyval.T)=NIL(Tree);;}
    break;

  case 24:
#line 179 "tp.y"
    {		
	/* probleme : la classe qu'on souhaite declaree existe deja */		
	if(estDansListClasse(listeDeClass, (yyvsp[(2) - (2)].S)) == TRUE){
		printf("Erreur dans DeclClass avec idClass=%s, elle existe deja\n", (yyvsp[(2) - (2)].S));
		char* message = NEW(SIZE_ERROR,char);
		sprintf(message,"Erreur la classe %s est deja declare",(yyvsp[(2) - (2)].S));
		/* TODO A MODIF pushErreur(message,classActuel,NULL,NULL); */
		(yyval.CL) = NULL; 	/* FIXME bon? */
	} 
	
	/* Pas de probleme : ajout de la classe */
	else{
		(yyval.CL)=makeDefClasse((yyvsp[(2) - (2)].S)); 
		classActuel=(yyval.CL); 		
		/* mise a jour de la variable globale (la liste des classes) */
		if(listeDeClass==NULL){
			listeDeClass=(yyval.CL);
		}
		else{
			PCLASS tmp=listeDeClass;
			while(tmp->suivant!=NULL){
				tmp=tmp->suivant;	
			}
			tmp->suivant=(yyval.CL);
		}
	}
;}
    break;

  case 25:
#line 212 "tp.y"
    {   
    	if((yyvsp[(1) - (10)].CL) == NULL){
		printf("Erreur dans DefClass\n");
	}
	else {
		int isExtend; 
		/** cas ou une classe n'herite pas d'une classe mere **/
       		if((yyvsp[(5) - (10)].CL)==NIL(SCLASS) || (yyvsp[(5) - (10)].CL)==NULL){
          		isExtend=0;
        	}
		/** cas ou une classe herite d'une classe mere **/
		else{
          		isExtend=1;
       		 }

        	(yyvsp[(1) - (10)].CL)=makeClasseApresDef((yyvsp[(1) - (10)].CL),(yyvsp[(3) - (10)].V),(yyvsp[(6) - (10)].T),(yyvsp[(9) - (10)].T)->u.children[1]->u.methode,(yyvsp[(9) - (10)].T)->u.children[0]->u.var, (yyvsp[(5) - (10)].CL),isExtend);
        	
		/*      Pour nous simplifier la tache, on copie les attributs et methodes 
		 *	de la classe mere dans les attributs et methodes de la classe fille 
		 */
		if(isExtend == 1){ 	
			/* Si une classe n'a pas de constructeur
			 Ajout des attributs ($5=classe mere)*/ 
			 

                  
			if((yyvsp[(5) - (10)].CL)->liste_champs != NULL){	/* cas ou la classe mere a d'attributs */
				/* cas ou la classe fille a des attributs -> faut ceux de la mere ajouter a la fin */
				if(classActuel->liste_champs!=NULL){
					PVAR tmp_liste_champs = classActuel->liste_champs;
					while(tmp_liste_champs->suivant != NULL){
						tmp_liste_champs=tmp_liste_champs->suivant;
					}
					PVAR tmp_champs_mere = (yyvsp[(5) - (10)].CL)->liste_champs;
					PVAR champs_a_ajoutee = NULL;
					/* On parcours les champs de la mere */
					while(tmp_champs_mere != NULL){

						/* Si le champ de la mere n'est pas dans la fille -> on ajoute */
						if(varEstDansListe(classActuel->liste_champs, tmp_champs_mere->nom)==FALSE){
							if(champs_a_ajoutee == NULL){
								champs_a_ajoutee = makeListVar(tmp_champs_mere->nom, tmp_champs_mere->type, tmp_champs_mere->categorie, tmp_champs_mere->init);

							}
							else{
								SVAR copieVar = *champs_a_ajoutee;
								champs_a_ajoutee = makeListVar(tmp_champs_mere->nom, tmp_champs_mere->type, tmp_champs_mere->categorie, tmp_champs_mere->init);
								champs_a_ajoutee->suivant = NEW(1, SVAR);
								*champs_a_ajoutee->suivant = copieVar;
							}
						}
						
						tmp_champs_mere = tmp_champs_mere->suivant;
					}

					/* ajout des attributs (en fin de liste) */
					if(champs_a_ajoutee!=NULL){
						
						SVAR copieVarFinale = *champs_a_ajoutee;
						PVAR tmp_champs_a_ajoutee = NEW(1,SVAR);	
						*tmp_champs_a_ajoutee = copieVarFinale;
						

						tmp_liste_champs->suivant = tmp_champs_a_ajoutee;

						 
					}
				}
				/* cas ou la classe fille n'a pas d'attributs -> on ajoute directement ceux de la mere */
				else{
					classActuel->liste_champs = (yyvsp[(5) - (10)].CL)->liste_champs;	
				}

			}

			 
			/* Ajout des methodes ($5 = classe mere)
			 *	-> Remarque : si classe fille redefinie une methode -> on n'ajoute pas celle de la mere
			 */
			 
			if((yyvsp[(5) - (10)].CL)->liste_methodes != NULL){
				/* cas ou la classe fille n'a pas de methode -> on ajoute directement celles de la mere */
				if(classActuel->liste_methodes==NULL){
					classActuel->liste_methodes = (yyvsp[(5) - (10)].CL)->liste_methodes;
				}
				/* cas ou la classe fille a des methodes -> on doit aller a la fin de la liste */
				else{
					PMETH tmp_liste_methodes = classActuel->liste_methodes;
					while(tmp_liste_methodes->suivant != NULL){
						tmp_liste_methodes=tmp_liste_methodes->suivant;
					}
	 				/* Ajout de maniere iterative les methodes car il faut les verifier 1 par 1 */
					PMETH tmp_liste_methodes_classMere = (yyvsp[(5) - (10)].CL)->liste_methodes;
					PMETH liste_a_ajoutee = NULL;

					/* Parcours des methodes de la classe mere */
					while(tmp_liste_methodes_classMere != NULL){
						/* On regarde si la classe fille a la meme methode que la classe mere
							-> si oui : on regarde isRedef de la methode de la classe fille
								-> si il est = a 0 : le mettre à 1 */
						PMETH methClassActuel=getMethode(classActuel, tmp_liste_methodes_classMere);
						/*if(methodeDansClasse(classActuel, tmp_liste_methodes_classMere)==TRUE){*/
						if(methClassActuel != NULL){
							if(methClassActuel->isRedef==0){
								methClassActuel->isRedef = 1;
							}
						}
						/* la methode de la mere n'est pas dans la classe fille */
						else{
							/* Condition pour que la classe mere puisse etre ajoutee dans la classe fille */
							if(tmp_liste_methodes_classMere->isStatic == 0){
							/* Ici, les methodes ajoutees ne peuvent pas etre static ni etre override */
							/* Il faut gerer le cas ou elles sont pas dites "redefinies" alors qu'elles le sont */
								if(liste_a_ajoutee == NULL){
									liste_a_ajoutee = makeMethode(tmp_liste_methodes_classMere->nom, 0, tmp_liste_methodes_classMere->corps, tmp_liste_methodes_classMere->typeRetour, tmp_liste_methodes_classMere->params, tmp_liste_methodes_classMere->home);
									/*tmp_liste_a_ajoutee = liste_a_ajoutee;*/
								}
								else{
									SMETH copie = *liste_a_ajoutee;
									liste_a_ajoutee = makeMethode(tmp_liste_methodes_classMere->nom, 0, tmp_liste_methodes_classMere->corps, tmp_liste_methodes_classMere->typeRetour, tmp_liste_methodes_classMere->params, tmp_liste_methodes_classMere->home);
									liste_a_ajoutee->suivant = NEW(1,SMETH);
									*liste_a_ajoutee->suivant = copie;
								}
								/*liste_a_ajoutee = liste_a_ajoutee->suivant;*/
								/*liste_a_ajoutee->suivant = NULL;*/
							}
						}
						tmp_liste_methodes_classMere = tmp_liste_methodes_classMere->suivant;
					}
					if(liste_a_ajoutee!=NULL){
						SMETH copieFinale = *liste_a_ajoutee;
						PMETH tmp_liste_a_ajoutee = NEW(1,SMETH);	
						*tmp_liste_a_ajoutee = copieFinale;
						tmp_liste_methodes->suivant = tmp_liste_a_ajoutee;
					}
					/*
					printf("====\nListe a ajoute = \n");
					while(tmp_liste_a_ajoutee != NULL){
						printf("tmp_liste_a_ajoutee=%s\n", tmp_liste_a_ajoutee->nom);
						tmp_liste_a_ajoutee=tmp_liste_a_ajoutee->suivant;
					}
					printf("====\n");
					tmp_liste_methodes->suivant = tmp_liste_a_ajoutee;
					*/
				}
			}	
		}
   		(yyval.CL)=classActuel;
	}
;}
    break;

  case 26:
#line 365 "tp.y"
    {(yyval.T)=makeTree(CONTENUCLASS,2,makeLeafVar(LISTEVAR,(yyvsp[(1) - (2)].V)),makeLeafMeth(LISTEMETHODE,(yyvsp[(2) - (2)].M)));;}
    break;

  case 27:
#line 372 "tp.y"
    {(yyval.V)=makeListVar((yyvsp[(3) - (8)].S),getClasseBis(listeDeClass,(yyvsp[(5) - (8)].S)),(yyvsp[(2) - (8)].I),(yyvsp[(6) - (8)].T)); (yyval.V)->suivant=(yyvsp[(8) - (8)].V);;}
    break;

  case 28:
#line 373 "tp.y"
    {(yyval.V)=NIL(SVAR);;}
    break;

  case 29:
#line 377 "tp.y"
    {(yyval.I)=1;;}
    break;

  case 30:
#line 378 "tp.y"
    {(yyval.I)=0;;}
    break;

  case 31:
#line 382 "tp.y"
    {(yyval.T)=makeTree(ETIQUETTE_AFFECT, 1, (yyvsp[(2) - (2)].T));;}
    break;

  case 32:
#line 383 "tp.y"
    {(yyval.T)=NIL(Tree);;}
    break;

  case 33:
#line 392 "tp.y"
    {(yyval.M)=makeMethode((yyvsp[(3) - (9)].S),(yyvsp[(2) - (9)].I),(yyvsp[(9) - (9)].T),getClasseBis(listeDeClass,(yyvsp[(8) - (9)].S)),(yyvsp[(5) - (9)].V),classActuel);;}
    break;

  case 34:
#line 396 "tp.y"
    {(yyvsp[(1) - (2)].M)->suivant=(yyvsp[(2) - (2)].M); (yyval.M)=(yyvsp[(1) - (2)].M);;}
    break;

  case 35:
#line 397 "tp.y"
    {(yyval.M)=NIL(SMETH);;}
    break;

  case 36:
#line 401 "tp.y"
    {(yyval.I)=1;;}
    break;

  case 37:
#line 402 "tp.y"
    {(yyval.I)=2;;}
    break;

  case 38:
#line 403 "tp.y"
    {(yyval.I)=0;;}
    break;

  case 39:
#line 407 "tp.y"
    {(yyval.T)=(yyvsp[(1) - (1)].T);;}
    break;

  case 40:
#line 408 "tp.y"
    {(yyval.T)=(yyvsp[(1) - (1)].T);;}
    break;

  case 41:
#line 411 "tp.y"
    {(yyval.V)=(yyvsp[(1) - (1)].V);;}
    break;

  case 42:
#line 412 "tp.y"
    {(yyval.V)=NIL(SVAR);;}
    break;

  case 43:
#line 415 "tp.y"
    {(yyval.V)=(yyvsp[(1) - (1)].V);;}
    break;

  case 44:
#line 416 "tp.y"
    {(yyvsp[(1) - (3)].V)->suivant=(yyvsp[(3) - (3)].V); (yyval.V)=(yyvsp[(1) - (3)].V);;}
    break;

  case 45:
#line 419 "tp.y"
    {(yyval.V)= makeListVar((yyvsp[(1) - (3)].S),getClasseBis(listeDeClass,(yyvsp[(3) - (3)].S)),0,NIL(Tree));;}
    break;

  case 46:
#line 423 "tp.y"
    {

	(yyval.CL)=getClasse(listeDeClass, (yyvsp[(2) - (5)].S));	/* peut etre besoin de getClasseBis? */
	char* message = NEW(SIZE_ERROR,char);
	if((yyval.CL) == NULL){
		/* la classe n'existe pas: erreur */
		printf("Probleme au niveau de la declaration de la classe\n");
    sprintf(message,"Erreur la classe %s n'existe pas",(yyvsp[(2) - (5)].S));
		pushErreur(message,classActuel,NULL,NULL);
	}
	else{

		classActuel->appel_constructeur_mere = (yyvsp[(4) - (5)].T);
		
		/* TODO */
		/*printf("Tentative de faire Extends : la classe existe-> ok : idclass=%s\n", $2);*/
		/* appeler une fonction qui verifie si ListOptArg est coherent avec la classe ($$) */

		/* A REMETTRE appelConstructureEstCorrecte($4,$$);*/

		/* on ajoute a la classe mere les param passees dans ListOptArg */
		/* Exemple : class PointColore(xc: Integer, yc:Integer, c: Couleur) extends Point(xc, yc) ==> on dit que les param xc 
			et yc de Point ont les valeurs respectives xc et yc **/
		
	/** C'est de l'eval : FIXME attente d'une fonction qui me rend le TreeP dans le bon ordre --> Amin et Gishan s'en occupe **/
	/*	COMPLETEMENT FAUX CAR L'ARBRE N'EST PAS DANS LE BON SENS
		TreeP listOptArg = $4;
		PVAR paramConstructeur = $$->param_constructeur;
		printf("while\n");
		while(listOptArg->u.children[1]!=NIL(Tree)){
			printf("1\n");
			paramConstructeur->init=listOptArg->u.children[0];
			printf("2\n");
			listOptArg = listOptArg->u.children[1];
			printf("3\n");
			paramConstructeur = paramConstructeur->suivant;
			printf("4\n");
			printf("nb de  children = %d\n", listOptArg->nbChildren);
		}
		printf("if\n");
		if(listOptArg->u.children[0]!=NIL(Tree)){
			paramConstructeur->init=listOptArg->u.children[0];
		}
		printf("-----passer-----\n");
	*/

	}
;}
    break;

  case 47:
#line 471 "tp.y"
    {(yyval.CL)=NIL(SCLASS);;}
    break;

  case 48:
#line 474 "tp.y"
    {(yyval.T)=NIL(Tree);;}
    break;

  case 49:
#line 475 "tp.y"
    {(yyval.T)=(yyvsp[(1) - (1)].T);;}
    break;

  case 50:
#line 479 "tp.y"
    {(yyval.T)=(yyvsp[(1) - (1)].T);;}
    break;

  case 51:
#line 480 "tp.y"
    {(yyval.T)=makeTree(LISTEARG, 2, (yyvsp[(1) - (3)].T),(yyvsp[(3) - (3)].T));;}
    break;

  case 52:
#line 484 "tp.y"
    { (yyval.T)=makeTree(PLUSUNAIRE, 1, (yyvsp[(2) - (2)].T)); ;}
    break;

  case 53:
#line 485 "tp.y"
    { (yyval.T)=makeTree(MINUSUNAIRE, 1, (yyvsp[(2) - (2)].T)); ;}
    break;

  case 54:
#line 486 "tp.y"
    { (yyval.T)=makeTree(CONCATENATION, 2, (yyvsp[(1) - (3)].T), (yyvsp[(3) - (3)].T)); ;}
    break;

  case 55:
#line 487 "tp.y"
    { (yyval.T)=makeTree(PLUSBINAIRE, 2, (yyvsp[(1) - (3)].T), (yyvsp[(3) - (3)].T)); ;}
    break;

  case 56:
#line 488 "tp.y"
    { (yyval.T)=makeTree(MINUSBINAIRE, 2, (yyvsp[(1) - (3)].T), (yyvsp[(3) - (3)].T)); ;}
    break;

  case 57:
#line 489 "tp.y"
    { (yyval.T)=makeTree(DIVISION, 2, (yyvsp[(1) - (3)].T), (yyvsp[(3) - (3)].T)); ;}
    break;

  case 58:
#line 490 "tp.y"
    { (yyval.T)=makeTree(MULTIPLICATION, 2, (yyvsp[(1) - (3)].T), (yyvsp[(3) - (3)].T)); ;}
    break;

  case 59:
#line 491 "tp.y"
    { (yyval.T)=makeTree(OPCOMPARATEUR, 3 , (yyvsp[(1) - (3)].T), (yyvsp[(3) - (3)].T), makeLeafInt(OPERATEUR,(yyvsp[(2) - (3)].I)));;}
    break;

  case 60:
#line 492 "tp.y"
    { (yyval.T)=(yyvsp[(1) - (1)].T); ;}
    break;

  case 61:
#line 493 "tp.y"
    { (yyval.T)=(yyvsp[(1) - (1)].T); ;}
    break;

  case 62:
#line 494 "tp.y"
    { (yyval.T)=(yyvsp[(1) - (1)].T); ;}
    break;

  case 63:
#line 495 "tp.y"
    { (yyval.T)=(yyvsp[(1) - (1)].T); ;}
    break;

  case 64:
#line 498 "tp.y"
    {(yyval.T)=(yyvsp[(2) - (3)].T);;}
    break;

  case 65:
#line 499 "tp.y"
    {(yyval.T)=(yyvsp[(1) - (1)].T);;}
    break;

  case 66:
#line 507 "tp.y"
    {(yyval.T)=makeTree(SELECTION, 2, makeLeafStr(IDENTIFICATEURCLASS,(yyvsp[(1) - (3)].S)),makeLeafStr(IDENTIFICATEUR,(yyvsp[(3) - (3)].S)));;}
    break;

  case 67:
#line 508 "tp.y"
    {(yyval.T)=makeTree(SELECTION, 2, (yyvsp[(1) - (3)].T),makeLeafStr(IDENTIFICATEUR,(yyvsp[(3) - (3)].S)));;}
    break;

  case 68:
#line 509 "tp.y"
    {(yyval.T)=makeTree(SELECTION, 2, (yyvsp[(1) - (3)].T),makeLeafStr(IDENTIFICATEUR,(yyvsp[(3) - (3)].S)));;}
    break;

  case 69:
#line 515 "tp.y"
    {(yyval.T) = makeLeafStr(CSTSTRING,(yyvsp[(1) - (1)].S)); ;}
    break;

  case 70:
#line 516 "tp.y"
    {(yyval.T) = makeLeafInt(CSTENTIER,(yyvsp[(1) - (1)].I));;}
    break;

  case 71:
#line 519 "tp.y"
    { (yyval.T)=makeTree(INSTANCIATION, 2, makeLeafStr(IDENTIFICATEURCLASS,(yyvsp[(2) - (5)].S)), (yyvsp[(4) - (5)].T)); ;}
    break;

  case 72:
#line 528 "tp.y"
    {(yyval.T)=makeTree(ENVOIMESSAGE, 3, makeLeafStr(IDENTIFICATEURCLASS,(yyvsp[(1) - (6)].S)),makeLeafStr(IDENTIFICATEUR,(yyvsp[(3) - (6)].S)),(yyvsp[(5) - (6)].T)); ;}
    break;

  case 73:
#line 530 "tp.y"
    {(yyval.T)=makeTree(ENVOIMESSAGE, 3,(yyvsp[(1) - (6)].T),makeLeafStr(IDENTIFICATEUR,(yyvsp[(3) - (6)].S)),(yyvsp[(5) - (6)].T)); ;}
    break;

  case 74:
#line 532 "tp.y"
    {(yyval.T)=makeTree(ENVOIMESSAGE, 3,(yyvsp[(1) - (6)].T),makeLeafStr(IDENTIFICATEUR,(yyvsp[(3) - (6)].S)),(yyvsp[(5) - (6)].T)); ;}
    break;

  case 75:
#line 534 "tp.y"
    {(yyval.T)=makeTree(ENVOIMESSAGE, 3,(yyvsp[(1) - (6)].T),makeLeafStr(IDENTIFICATEUR,(yyvsp[(3) - (6)].S)),(yyvsp[(5) - (6)].T)); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 2108 "tp_y.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
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

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



