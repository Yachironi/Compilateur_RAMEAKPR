/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

