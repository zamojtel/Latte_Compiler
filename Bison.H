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

#ifndef YY_LATTE_CPP_BISON_H_INCLUDED
# define YY_LATTE_CPP_BISON_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int latte_cpp_debug;
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
    _ERROR_ = 258,                 /* _ERROR_  */
    _BANG = 259,                   /* _BANG  */
    _BANGEQ = 260,                 /* _BANGEQ  */
    _PERCENT = 261,                /* _PERCENT  */
    _DAMP = 262,                   /* _DAMP  */
    _LPAREN = 263,                 /* _LPAREN  */
    _RPAREN = 264,                 /* _RPAREN  */
    _STAR = 265,                   /* _STAR  */
    _PLUS = 266,                   /* _PLUS  */
    _DPLUS = 267,                  /* _DPLUS  */
    _COMMA = 268,                  /* _COMMA  */
    _MINUS = 269,                  /* _MINUS  */
    _DMINUS = 270,                 /* _DMINUS  */
    _SLASH = 271,                  /* _SLASH  */
    _SEMI = 272,                   /* _SEMI  */
    _LT = 273,                     /* _LT  */
    _LDARROW = 274,                /* _LDARROW  */
    _EQ = 275,                     /* _EQ  */
    _DEQ = 276,                    /* _DEQ  */
    _GT = 277,                     /* _GT  */
    _GTEQ = 278,                   /* _GTEQ  */
    _KW_boolean = 279,             /* _KW_boolean  */
    _KW_else = 280,                /* _KW_else  */
    _KW_false = 281,               /* _KW_false  */
    _KW_if = 282,                  /* _KW_if  */
    _KW_int = 283,                 /* _KW_int  */
    _KW_return = 284,              /* _KW_return  */
    _KW_string = 285,              /* _KW_string  */
    _KW_true = 286,                /* _KW_true  */
    _KW_void = 287,                /* _KW_void  */
    _KW_while = 288,               /* _KW_while  */
    _LBRACE = 289,                 /* _LBRACE  */
    _DBAR = 290,                   /* _DBAR  */
    _RBRACE = 291,                 /* _RBRACE  */
    _STRING_ = 292,                /* _STRING_  */
    _INTEGER_ = 293,               /* _INTEGER_  */
    _IDENT_ = 294                  /* _IDENT_  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 50 "LatteCPP.y"

  int    _int;
  char   _char;
  double _double;
  char*  _string;
  Program* program_;
  TopDef* topdef_;
  ListTopDef* listtopdef_;
  Arg* arg_;
  ListArg* listarg_;
  Block* block_;
  ListStmt* liststmt_;
  Stmt* stmt_;
  Item* item_;
  ListItem* listitem_;
  Type* type_;
  ListType* listtype_;
  Expr* expr_;
  ListExpr* listexpr_;
  AddOp* addop_;
  MulOp* mulop_;
  RelOp* relop_;

#line 127 "Bison.H"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif




int latte_cpp_parse (yyscan_t scanner, YYSTYPE *result);


#endif /* !YY_LATTE_CPP_BISON_H_INCLUDED  */
