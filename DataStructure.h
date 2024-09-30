#include <list>
#include <string>

class Variable;
class Triple;
std::list<Triple> m_triples;

enum class DataType{
    INT,BOOL,STRING,
};

// EVar.      Expr6 ::= Ident ;
// ELitInt.   Expr6 ::= Integer ;
// ELitTrue.  Expr6 ::= "true" ;
// ELitFalse. Expr6 ::= "false" ;
// EApp.      Expr6 ::= Ident "(" [Expr] ")" ;
// EString.   Expr6 ::= String ;
// Neg.       Expr5 ::= "-" Expr6 ;
// Not.       Expr5 ::= "!" Expr6 ;
// EMul.      Expr4 ::= Expr4 MulOp Expr5 ;
// EAdd.      Expr3 ::= Expr3 AddOp Expr4 ;
// ERel.      Expr2 ::= Expr2 RelOp Expr3 ;
// EAnd.      Expr1 ::= Expr2 "&&" Expr1 ;
// EOr.       Expr ::= Expr1 "||" Expr ;

// Mod.       MulOp ::= "%" ;
// LTH.       RelOp ::= "<" ;
// LE.        RelOp ::= "<=" ;
// GTH.       RelOp ::= ">" ;
// GE.        RelOp ::= ">=" ;
// EQU.       RelOp ::= "==" ;
// NE.        RelOp ::= "!=" ;

enum class Operation{
    MUL,ADD,SUB,DIV,AND,OR,NEG,NOT,ASSIGN,
    MOD,LTH,LE,GTH,GE,EQU,NE
};

enum class OperandCategory{
    CONSTANT,VARIABLE,TRIPLE,EMPTY
};

// Int.       Type ::= "int" ;

// Str.       Type ::= "string" ;

// Bool.      Type ::= "boolean" ;

// Void.      Type ::= "void" ;

// internal   Fun. Type ::= Type "(" [Type] ")" ;

// separator  Type "," ;
class Constant{
public:
    DataType m_type;
    //  INT,BOOL,STRING,
    union 
    {
        int integer;
        bool boolean;
    }u;

    Constant(int value):m_type{DataType::INT}{
        u.integer=value;
    }

    Constant(bool value):m_type{DataType::INT}{
        u.boolean=value;
    }
};

// struct Exp_
// {
//   int line_number, char_number;
//   enum { is_ExpAdd, is_ExpSub, is_ExpMul, is_ExpDiv, is_ExpLit, is_ExpVar } kind;
//   union
//   {
//     struct { Exp exp_1, exp_2; } expadd_;
//     struct { Exp exp_1, exp_2; } expsub_;
//     struct { Exp exp_1, exp_2; } expmul_;
//     struct { Exp exp_1, exp_2; } expdiv_;
//     struct { Integer integer_; } explit_;
//     struct { Ident ident_; } expvar_;
//   } u;

class Operand{
public:
    OperandCategory m_category;
    union 
    {
        Constant constant;
        Variable *var=nullptr;
        std::list<Triple>::iterator triple;
    };
    
    Operand(const Constant &c):constant{c}{}
    Operand(Variable *v):var{v}{}
    Operand(const std::list<Triple>::iterator &t):triple{t}{}
    Operand():m_category{OperandCategory::EMPTY}{}
};

//   union
//   {
//     struct { Exp exp_1, exp_2; } expadd_;
//     struct { Exp exp_1, exp_2; } expsub_;
//     struct { Exp exp_1, exp_2; } expmul_;
//     struct { Exp exp_1, exp_2; } expdiv_;
//     struct { Integer integer_; } explit_;
//     struct { Ident ident_; } expvar_;
//   } u;

class Variable{
    std::string m_ident;     
    DataType m_type;

    Variable(const std::string &ident,DataType type):m_ident{ident},m_type{type}{}
};

class Triple{
    Operand m_op_1;
    Operand m_op_2;
    Operation m_operation;

    Triple(Operation operation,const Operand &op_1,const Operand &op_2={}):m_op_1{}{}
};

class Argument{
public:
    DataType m_type;
    std::string m_identifier;
};

class Function{
private:
public:
    std::string m_name;
    DataType m_return_type;
    std::vector<Argument*> m_arguments;
    ~Function(){
        for(auto *arg: m_arguments)
            delete arg;
    }
};