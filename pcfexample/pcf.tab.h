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
     ZERO = 258,
     TRUE = 259,
     FALSE = 260,
     IDEN = 261,
     FIX = 262,
     ZEROFUNC = 263,
     LPAREN = 264,
     RPAREN = 265,
     SUCC = 266,
     PRED = 267,
     IF = 268,
     THEN = 269,
     ELSE = 270,
     FN = 271,
     COLON = 272,
     DOT = 273,
     NAT = 274,
     ARROW = 275,
     BOOL = 276
   };
#endif
/* Tokens.  */
#define ZERO 258
#define TRUE 259
#define FALSE 260
#define IDEN 261
#define FIX 262
#define ZEROFUNC 263
#define LPAREN 264
#define RPAREN 265
#define SUCC 266
#define PRED 267
#define IF 268
#define THEN 269
#define ELSE 270
#define FN 271
#define COLON 272
#define DOT 273
#define NAT 274
#define ARROW 275
#define BOOL 276




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

