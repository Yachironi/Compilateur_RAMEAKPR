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
     STATIC = 262,
     DEF = 263,
     OVERRIDE = 264,
     RETURNS = 265,
     RETURN = 266,
     YIELD = 267,
     IF = 268,
     THEN = 269,
     ELSE = 270,
     NEW = 271,
     PLUS = 272,
     MINUS = 273,
     RELOP = 274,
     AFFECT = 275,
     MUL = 276,
     DIV = 277,
     CST = 278,
     IdClass = 279,
     ListBloc = 280,
     ID = 281,
     CSTS = 282,
     CSTE = 283,
     unaire = 284
   };
#endif
/* Tokens.  */
#define CLASS 258
#define VAR 259
#define EXTENDS 260
#define IS 261
#define STATIC 262
#define DEF 263
#define OVERRIDE 264
#define RETURNS 265
#define RETURN 266
#define YIELD 267
#define IF 268
#define THEN 269
#define ELSE 270
#define NEW 271
#define PLUS 272
#define MINUS 273
#define RELOP 274
#define AFFECT 275
#define MUL 276
#define DIV 277
#define CST 278
#define IdClass 279
#define ListBloc 280
#define ID 281
#define CSTS 282
#define CSTE 283
#define unaire 284




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

