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
     VAL = 258,
     DEFINE = 259,
     LPAREN = 260,
     RPAREN = 261,
     USE = 262,
     CHECKEXPECT = 263,
     SET = 264,
     IF = 265,
     WHILE = 266,
     BEGN = 267,
     PLUS = 268,
     MINUS = 269,
     MUL = 270,
     DIV = 271,
     EQ = 272,
     LT = 273,
     GT = 274,
     PRINT = 275,
     CHECKERR = 276,
     NUMERAL = 277,
     NAME = 278
   };
#endif
/* Tokens.  */
#define VAL 258
#define DEFINE 259
#define LPAREN 260
#define RPAREN 261
#define USE 262
#define CHECKEXPECT 263
#define SET 264
#define IF 265
#define WHILE 266
#define BEGN 267
#define PLUS 268
#define MINUS 269
#define MUL 270
#define DIV 271
#define EQ 272
#define LT 273
#define GT 274
#define PRINT 275
#define CHECKERR 276
#define NUMERAL 277
#define NAME 278




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

