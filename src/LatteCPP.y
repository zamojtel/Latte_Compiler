/* -*- c++ -*- File generated by the BNF Converter (bnfc 2.9.5). */

/* Parser definition to be used with Bison. */

/* Generate header file for lexer. */
%defines "Bison.H"

/* Reentrant parser */
%pure_parser
  /* From Bison 2.3b (2008): %define api.pure full */
%lex-param   { yyscan_t scanner }
%parse-param { yyscan_t scanner }

/* Turn on line/column tracking in the latte_cpp_lloc structure: */
%locations

/* Argument to the parser to be filled with the parsed tree. */
%parse-param { YYSTYPE *result }

%{
/* Begin C preamble code */

#include <algorithm> /* for std::reverse */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Absyn.H"

#define YYMAXDEPTH 10000000

/* The type yyscan_t is defined by flex, but we need it in the parser already. */
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE latte_cpp__scan_string(const char *str, yyscan_t scanner);
extern void latte_cpp__delete_buffer(YY_BUFFER_STATE buf, yyscan_t scanner);

extern void latte_cpp_lex_destroy(yyscan_t scanner);
extern char* latte_cpp_get_text(yyscan_t scanner);

extern yyscan_t latte_cpp__initialize_lexer(FILE * inp);

/* End C preamble code */
%}

%union
{
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
}

%{
void yyerror(YYLTYPE *loc, yyscan_t scanner, YYSTYPE *result, const char *msg)
{
  printf("ERROR\n");
  printf("Line %d, Column %d: %s at %s\n",
  loc->first_line, loc->first_column, msg, latte_cpp_get_text(scanner));
}

int yyparse(yyscan_t scanner, YYSTYPE *result);

extern int yylex(YYSTYPE *lvalp, YYLTYPE *llocp, yyscan_t scanner);
%}

%token          _ERROR_
%token          _BANG        /* ! */
%token          _BANGEQ      /* != */
%token          _PERCENT     /* % */
%token          _DAMP        /* && */
%token          _LPAREN      /* ( */
%token          _RPAREN      /* ) */
%token          _STAR        /* * */
%token          _PLUS        /* + */
%token          _DPLUS       /* ++ */
%token          _COMMA       /* , */
%token          _MINUS       /* - */
%token          _DMINUS      /* -- */
%token          _SLASH       /* / */
%token          _SEMI        /* ; */
%token          _LT          /* < */
%token          _LDARROW     /* <= */
%token          _EQ          /* = */
%token          _DEQ         /* == */
%token          _GT          /* > */
%token          _GTEQ        /* >= */
%token          _KW_boolean  /* boolean */
%token          _KW_else     /* else */
%token          _KW_false    /* false */
%token          _KW_if       /* if */
%token          _KW_int      /* int */
%token          _KW_return   /* return */
%token          _KW_string   /* string */
%token          _KW_true     /* true */
%token          _KW_void     /* void */
%token          _KW_while    /* while */
%token          _LBRACE      /* { */
%token          _DBAR        /* || */
%token          _RBRACE      /* } */
%token<_string> _STRING_
%token<_int>    _INTEGER_
%token<_string> _IDENT_

%type <program_> Program
%type <topdef_> TopDef
%type <listtopdef_> ListTopDef
%type <arg_> Arg
%type <listarg_> ListArg
%type <block_> Block
%type <liststmt_> ListStmt
%type <stmt_> Stmt
%type <item_> Item
%type <listitem_> ListItem
%type <type_> Type
%type <listtype_> ListType
%type <expr_> Expr6
%type <expr_> Expr5
%type <expr_> Expr4
%type <expr_> Expr3
%type <expr_> Expr2
%type <expr_> Expr1
%type <expr_> Expr
%type <listexpr_> ListExpr
%type <addop_> AddOp
%type <mulop_> MulOp
%type <relop_> RelOp

%start Program

%%

Program : ListTopDef { std::reverse($1->begin(),$1->end()) ;$$ = new Prog($1); $$->line_number = @$.first_line; $$->char_number = @$.first_column; result->program_ = $$; }
;
TopDef : Type _IDENT_ _LPAREN ListArg _RPAREN Block { std::reverse($4->begin(),$4->end()) ;$$ = new FnDef($1, $2, $4, $6); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
;
ListTopDef : TopDef { $$ = new ListTopDef(); $$->push_back($1); }
  | TopDef ListTopDef { $2->push_back($1); $$ = $2; }
;
Arg : Type _IDENT_ { $$ = new Ar($1, $2); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
;
ListArg : /* empty */ { $$ = new ListArg(); }
  | Arg { $$ = new ListArg(); $$->push_back($1); }
  | Arg _COMMA ListArg { $3->push_back($1); $$ = $3; }
;
Block : _LBRACE ListStmt _RBRACE { $$ = new Blk($2); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
;
ListStmt : /* empty */ { $$ = new ListStmt(); }
  | ListStmt Stmt { $1->push_back($2); $$ = $1; }
;
Stmt : _SEMI { $$ = new Empty(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | Block { $$ = new BStmt($1); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | Type ListItem _SEMI { std::reverse($2->begin(),$2->end()) ;$$ = new Decl($1, $2); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _IDENT_ _EQ Expr _SEMI { $$ = new Ass($1, $3); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _IDENT_ _DPLUS _SEMI { $$ = new Incr($1); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _IDENT_ _DMINUS _SEMI { $$ = new Decr($1); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _KW_return Expr _SEMI { $$ = new Ret($2); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _KW_return _SEMI { $$ = new VRet(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _KW_if _LPAREN Expr _RPAREN Stmt { $$ = new Cond($3, $5); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _KW_if _LPAREN Expr _RPAREN Stmt _KW_else Stmt { $$ = new CondElse($3, $5, $7); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _KW_while _LPAREN Expr _RPAREN Stmt { $$ = new While($3, $5); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | Expr _SEMI { $$ = new SExp($1); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
;
Item : _IDENT_ { $$ = new NoInit($1); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _IDENT_ _EQ Expr { $$ = new Init($1, $3); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
;
ListItem : Item { $$ = new ListItem(); $$->push_back($1); }
  | Item _COMMA ListItem { $3->push_back($1); $$ = $3; }
;
Type : _KW_int { $$ = new Int(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _KW_string { $$ = new Str(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _KW_boolean { $$ = new Bool(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _KW_void { $$ = new Void(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
;
ListType : /* empty */ { $$ = new ListType(); }
  | Type { $$ = new ListType(); $$->push_back($1); }
  | Type _COMMA ListType { $3->push_back($1); $$ = $3; }
;
Expr6 : _IDENT_ { $$ = new EVar($1); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _INTEGER_ { $$ = new ELitInt($1); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _KW_true { $$ = new ELitTrue(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _KW_false { $$ = new ELitFalse(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _IDENT_ _LPAREN ListExpr _RPAREN { std::reverse($3->begin(),$3->end()) ;$$ = new EApp($1, $3); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _STRING_ { $$ = new EString($1); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _LPAREN Expr _RPAREN { $$ = $2; $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
;
Expr5 : _MINUS Expr6 { $$ = new Neg($2); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _BANG Expr6 { $$ = new Not($2); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | Expr6 { $$ = $1; $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
;
Expr4 : Expr4 MulOp Expr5 { $$ = new EMul($1, $2, $3); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | Expr5 { $$ = $1; $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
;
Expr3 : Expr3 AddOp Expr4 { $$ = new EAdd($1, $2, $3); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | Expr4 { $$ = $1; $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
;
Expr2 : Expr2 RelOp Expr3 { $$ = new ERel($1, $2, $3); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | Expr3 { $$ = $1; $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
;
Expr1 : Expr2 _DAMP Expr1 { $$ = new EAnd($1, $3); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | Expr2 { $$ = $1; $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
;
Expr : Expr1 _DBAR Expr { $$ = new EOr($1, $3); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | Expr1 { $$ = $1; $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
;
ListExpr : /* empty */ { $$ = new ListExpr(); }
  | Expr { $$ = new ListExpr(); $$->push_back($1); }
  | Expr _COMMA ListExpr { $3->push_back($1); $$ = $3; }
;
AddOp : _PLUS { $$ = new Plus(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _MINUS { $$ = new Minus(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
;
MulOp : _STAR { $$ = new Times(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _SLASH { $$ = new Div(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _PERCENT { $$ = new Mod(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
;
RelOp : _LT { $$ = new LTH(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _LDARROW { $$ = new LE(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _GT { $$ = new GTH(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _GTEQ { $$ = new GE(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _DEQ { $$ = new EQU(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
  | _BANGEQ { $$ = new NE(); $$->line_number = @$.first_line; $$->char_number = @$.first_column; }
;

%%


/* Entrypoint: parse Program* from file. */
Program* pProgram(FILE *inp)
{
  YYSTYPE result;
  yyscan_t scanner = latte_cpp__initialize_lexer(inp);
  if (!scanner) {
    fprintf(stderr, "Failed to initialize lexer.\n");
    return 0;
  }
  int error = yyparse(scanner, &result);
  latte_cpp_lex_destroy(scanner);
  if (error)
  { /* Failure */
    return 0;
  }
  else
  { /* Success */
    return result.program_;
  }
}

/* Entrypoint: parse Program* from string. */
Program* psProgram(const char *str)
{
  YYSTYPE result;
  yyscan_t scanner = latte_cpp__initialize_lexer(0);
  if (!scanner) {
    fprintf(stderr, "Failed to initialize lexer.\n");
    return 0;
  }
  YY_BUFFER_STATE buf = latte_cpp__scan_string(str, scanner);
  int error = yyparse(scanner, &result);
  latte_cpp__delete_buffer(buf, scanner);
  latte_cpp_lex_destroy(scanner);
  if (error)
  { /* Failure */
    return 0;
  }
  else
  { /* Success */
    return result.program_;
  }
}



