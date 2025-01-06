#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <list>
#include <string>
#include <forward_list>
#include <map>
#include <vector>
#include "IRCoderListener.h"

class Variable;
class Triple;
class Function;

enum class DataType{
    INT,BOOL,STRING,VOID,ERROR
};

class Argument{
public:
    DataType m_type;
    std::string m_identifier;
    Argument(){}
    Argument(DataType type,std::string id):m_type{type},m_identifier{id}{}
};

std::string data_type_to_string(const DataType &type);

enum class Operation{
    MUL,ADD,SUB,DIV,AND,OR,NEG,NOT,ASSIGN,
    MOD,LTH,LE,GTH,GE,EQU,NE,INIT,
    //Special Operations
    JT,JF, // jump if true ,jump if false
    MARKER, // It will indicate a special triple
    JMP, 
    CALL, // Function Invocation
    PARAM,
    RETURN,
    INC,
    DEC
};

enum class OperandCategory{
    CONSTANT,VARIABLE,TRIPLE,EMPTY,LABEL,FUNCTION,ARGUMENT,ERROR
};

template<class T>
void call_destructor(T* arg){
    arg->~T();
}

class Constant{
public:
    DataType m_type;
    union Value
    {
        int integer;
        bool boolean;
        std::string str;
        Value(){}
        ~Value(){}
    }u;
    
    Constant(int value);
    Constant(bool value);
    Constant(const std::string &s);
    
    Constant(const Constant &constant);

    Constant& operator=(const Constant &other);

    std::string get_value_as_string()const;

    ~Constant();
};

class Label{
public:
    size_t m_index;  // index in the llvm
    Triple *m_jump_to;

    Label(size_t index,Triple* jump_to=nullptr):m_index{index},m_jump_to{jump_to}{}
};

class Variable{
public:
    std::string m_ident;     
    DataType m_type;

    Variable(const std::string &ident,DataType type):m_ident{ident},m_type{type}{}
};

class Operand{
public:
    OperandCategory m_category;
    union
    {
        Constant m_constant;
        Variable *m_var=nullptr;
        Triple* m_triple;
        Label* m_label;
        Function* m_function;
        Argument* m_argument;
    };
   
    Operand(const Constant &c):m_category{OperandCategory::CONSTANT},m_constant{c}{}
    Operand(Variable *v):m_category{OperandCategory::VARIABLE},m_var{v}{}
    Operand(Triple *t):m_category{OperandCategory::TRIPLE},m_triple{t}{}
    Operand(Label *l):m_category{OperandCategory::LABEL},m_label{l}{}
    Operand(Function *f):m_category{OperandCategory::FUNCTION},m_function{f}{}
    Operand(Argument *arg):m_category{OperandCategory::ARGUMENT},m_argument{arg}{}
    Operand():m_category{OperandCategory::EMPTY}{}

    Operand(const Operand &other){
        m_category=other.m_category;
        switch (other.m_category)
        {
        case OperandCategory::CONSTANT:
            m_constant=other.m_constant;
            break;
        case OperandCategory::VARIABLE:
            m_var=other.m_var;
            break;
        case OperandCategory::TRIPLE:
            m_triple = other.m_triple; 
            break;
        case OperandCategory::LABEL:
            m_label=other.m_label;
            break;
        case OperandCategory::EMPTY:
            break;
        case OperandCategory::FUNCTION:
            m_function=other.m_function;
            break;
        case OperandCategory::ARGUMENT:
            m_argument=other.m_argument;
            break;
        case OperandCategory::ERROR:
            break;
        default:
            throw("No Operand Category");
            break;
        }
    }
    //  CONSTANT,VARIABLE,TRIPLE,EMPTY,LABEL,FUNCTION,ARGUMENT,ERROR
    Operand& operator=(const Operand &other){
        m_category=other.m_category;
        switch (other.m_category)
        {
        case OperandCategory::CONSTANT:
            this->m_constant=other.m_constant;
            break;
        case OperandCategory::VARIABLE:
            this->m_var=other.m_var;
            break;
        case OperandCategory::TRIPLE:
            this->m_triple=other.m_triple;
            break;
        case OperandCategory::FUNCTION:
            this->m_function=other.m_function;
            break;
        case OperandCategory::ARGUMENT:
            this->m_argument=other.m_argument;
            break;
        case OperandCategory::LABEL:
            this->m_label=other.m_label;
            break;
        case OperandCategory::ERROR:
        case OperandCategory::EMPTY:
            break;
        default:
            throw 0;
            break;
        }
        return *this;
    }

    static Operand error(){
        Operand operand;
        operand.m_category=OperandCategory::ERROR;
        return operand;
    }
    DataType get_type() const;
    std::string get_category_as_string() const;
    ~Operand(){
    }
};

class Triple{
public:
    int m_code_line_number;
    bool m_visited;
    DataType m_data_type;
    size_t m_index;
    Operation m_operation;
    Operand m_op_1;
    Operand m_op_2;
    std::vector<Label*> m_pointing_labels;
    // Operands as arguemnts
    std::list<Operand> m_call_args;
    Triple(int line_number,size_t index,Operation operation,const Operand &op_1={},const Operand &op_2={}):m_code_line_number{line_number},m_visited{false}, m_index{index},m_operation{operation},m_op_1{op_1},m_op_2{op_2}{}
};

class Error{
public:
    size_t m_code=0;
    size_t m_line;
    std::string m_msg;
    Error(size_t l,const std::string &msg):m_line{l},m_msg{msg}{}
};

enum class PredefinedFunction{
    PRINTINT,
    PRINTSTRING,
    ERROR,
    READINT,
    READSTRING,
    USERDEFINED
};

enum class SymbolTableCategory{
    ARGUMENT,VARIABLE,FUNCTION,EMPTY
};

class Function{
private:
public:
    PredefinedFunction m_type;
    bool m_used;
    std::string m_name;
    DataType m_return_type;
    std::vector<Variable*> m_variables;
    std::vector<Triple*> m_triples;
    std::vector<Argument*> m_arguments;
    // funkcja skacze tylko w swoim obrebie 
    std::vector<Label*> m_labels;
    Function(PredefinedFunction p=PredefinedFunction::USERDEFINED):m_type{p},m_used{false}{}
    void add_label(Label *label);
    ~Function(){
        for(auto *arg: m_arguments)
            delete arg;
    }
    bool contains_argument(const std::string &name) const;
};

class SymbolTableEntry{
public:
    SymbolTableCategory m_category;
    union{
        Argument *m_argument;
        Variable *m_variable;
        Function *m_function;
    };

    SymbolTableEntry(Variable *var):m_category{SymbolTableCategory::VARIABLE},m_variable{var}{}
    SymbolTableEntry(Function *fun):m_category{SymbolTableCategory::FUNCTION},m_function{fun}{}
    SymbolTableEntry(Argument *arg):m_category{SymbolTableCategory::ARGUMENT},m_argument{arg}{}
    SymbolTableEntry():m_category{SymbolTableCategory::EMPTY}{}
};

class SymbolTable{
public:
    std::vector<std::map<std::string,SymbolTableEntry>> m_entries;
    IRCoderListener *m_listener = nullptr;

    void push();
    void pop();
    bool add(const std::string &entry_name,const SymbolTableEntry &entry);
    void set_listener(IRCoderListener *listener);
    // to-do
    // std::optional 
    const SymbolTableEntry* get_entry(const std::string &name);
    Variable* get_variable(const std::string &name);
    Argument* get_argument(const std::string &name);
    Function * get_function(const std::string &name);
    const SymbolTableEntry* get_entry_at_top(const std::string &name);
    SymbolTable();
};

#endif 