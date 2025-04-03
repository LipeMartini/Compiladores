/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TOKEN_ID = 258,
    KW_BYTE = 259,
    KW_INT = 260,
    KW_REAL = 261,
    KW_STRING = 262,
    KW_CHAR = 263,
    KW_IF = 264,
    KW_ELSE = 265,
    KW_DO = 266,
    KW_WHILE = 267,
    KW_READ = 268,
    KW_PRINT = 269,
    KW_RETURN = 270,
    OPERATOR_LE = 271,
    OPERATOR_GE = 272,
    OPERATOR_EQ = 273,
    OPERATOR_DIF = 274,
    TK_IDENTIFIER = 275,
    LIT_INT = 276,
    LIT_CHAR = 277,
    LIT_REAL = 278,
    LIT_STRING = 279,
    TOKEN_ERROR = 280
  };
#endif
/* Tokens.  */
#define TOKEN_ID 258
#define KW_BYTE 259
#define KW_INT 260
#define KW_REAL 261
#define KW_STRING 262
#define KW_CHAR 263
#define KW_IF 264
#define KW_ELSE 265
#define KW_DO 266
#define KW_WHILE 267
#define KW_READ 268
#define KW_PRINT 269
#define KW_RETURN 270
#define OPERATOR_LE 271
#define OPERATOR_GE 272
#define OPERATOR_EQ 273
#define OPERATOR_DIF 274
#define TK_IDENTIFIER 275
#define LIT_INT 276
#define LIT_CHAR 277
#define LIT_REAL 278
#define LIT_STRING 279
#define TOKEN_ERROR 280

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
