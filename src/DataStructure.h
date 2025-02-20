#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <list>
#include <string>
#include <forward_list>
#include <map>
#include <vector>
#include <set>
#include "IRCoderListener.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <variant>

class Variable;
class Triple;
class Function;
class MyClass;
class Field;
class BasicBlock;
class IntermediateProgram;


enum class BasicType{
    INT,BOOL,STRING,VOID,ERROR,NULLPTR
};

enum class DataTypeCategory{
    BASIC,CLASS
};

struct DataType {
 MyClass * class_type=nullptr;
 BasicType basic_type;
 int dimensions;
 DataTypeCategory category;
 DataType(MyClass *cl):class_type{cl},dimensions{0},category{DataTypeCategory::CLASS}{}
 DataType(MyClass *cl,int d):class_type{cl},dimensions{d},category{DataTypeCategory::CLASS}{}
 DataType(BasicType t):basic_type{t},dimensions{0},category{DataTypeCategory::BASIC}{}
 DataType(BasicType b_t,int d):basic_type{b_t},dimensions{d},category{DataTypeCategory::BASIC}{}
 DataType(){}
 bool operator==(const DataType &other) const;
 bool operator==(BasicType other) const;
 bool operator!=(const DataType &other) const;
 bool operator!=(BasicType other) const;
 bool isNullable() const;
 DataType decrement_dimensions() const;
 DataType increment_dimensions() const;
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
    MUL,ADD,SUB,DIV,
    NEG,NOT,ASSIGN,
    MOD,LTH,LE,GTH,GE,EQU,NE,INIT,
    // Arrays
    NEW_ARRAY,
    ACCESS_ARRAY,
    ACCESS_ARRAY_FOR_READ,
    ACCESS_ARRAY_FOR_WRITE,
    ARRAY_LENGTH,
    // Classes
    NEW_INSTANCE,
    MEMEBER_ACCESS,
    MEMBER_FOR_READ,
    MEMBER_FOR_WRITE,
    // Casting
    CAST,
    // -------------------------------------
    JT,JF,JMP,
    START, // this is a special triple at the begginging of every function
    MARKER,
    CALL,
    RETURN,
    INC,
    DEC,
    PHI
};

enum class OperandCategory{
    CONSTANT,VARIABLE,TRIPLE,EMPTY,LABEL,FUNCTION,ARGUMENT,ERROR,
    ARRAY,FIELD
};

template<class T>
void call_destructor(T* arg){
    arg->~T();
}

class Constant{
public:
    BasicType m_type;
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
    Constant(nullptr_t ptr);
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

using ArgOrVar = std::variant<Argument*,Variable*>;
std::ostream& operator<<(std::ostream &o,const ArgOrVar &arg);

class Operand{
public:
    OperandCategory m_category;
    int m_version = 12345;
    union
    {
        Constant m_constant;
        Variable *m_var=nullptr;
        Triple* m_triple;
        Label* m_label;
        Function* m_function;
        Argument* m_argument;
        Field* m_field;
    };

    Operand(const Constant &c):m_category{OperandCategory::CONSTANT},m_constant{c}{}
    Operand(Triple *t):m_category{OperandCategory::TRIPLE},m_triple{t}{}
    Operand(Label *l):m_category{OperandCategory::LABEL},m_label{l}{}
    Operand(Function *f):m_category{OperandCategory::FUNCTION},m_function{f}{}
    Operand(Variable *v):m_category{OperandCategory::VARIABLE},m_version{0},m_var{v}{}
    Operand(Argument *arg):m_category{OperandCategory::ARGUMENT},m_version{0},m_argument{arg}{}
    Operand(Field *f):m_category{OperandCategory::FIELD},m_field{f}{}
    Operand():m_category{OperandCategory::EMPTY}{}

    ArgOrVar get_arg_or_var() const {
        switch (m_category)
        {
        case OperandCategory::VARIABLE:
            return m_var;
        case OperandCategory::ARGUMENT:
            return m_argument;
        default:
            throw std::runtime_error("WRONG OPERAND get_arg_or_var(const Operand &op)");
        }
    }

    Operand(const Operand &other){
        m_category=other.m_category;
        switch (other.m_category)
        {
        case OperandCategory::CONSTANT:
            m_constant=other.m_constant;
            break;
        case OperandCategory::VARIABLE:
        {
            m_var=other.m_var;
            m_version=other.m_version;
            break;
        }
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
        case OperandCategory::ARGUMENT:{
            m_argument=other.m_argument;
            m_version=other.m_version;
            break;
        }
        case OperandCategory::ERROR:
            break;
        case OperandCategory::FIELD:
            m_field=other.m_field;
            break;
        default:
            throw("No Operand Category");
            break;
        }
    }

    bool is_arg_or_var() const {
        return m_category==OperandCategory::ARGUMENT || m_category==OperandCategory::VARIABLE;
    }

    bool operator==(const Operand &other){
        if(m_category!=other.m_category)
            return false;

        switch (m_category)
        {
        case OperandCategory::VARIABLE:
            return m_var==other.m_var;
        case OperandCategory::ARGUMENT:
            return m_argument==other.m_argument;
        default:
            throw std::runtime_error("bool operator==(const Operand &other)");
            break;
        }
    }

    bool operator==(const Operand &other) const {
        if(m_category!=other.m_category)
            return false;
        switch (m_category)
        {
        case OperandCategory::VARIABLE:
            return m_var==other.m_var;
        case OperandCategory::ARGUMENT:
            return m_argument==other.m_argument;
        default:
            throw std::runtime_error("asd");
            break;
        }
    }

    Operand& operator=(const Operand &other){
        m_category=other.m_category;
        switch (other.m_category)
        {
        case OperandCategory::CONSTANT:
            this->m_constant=other.m_constant;
            break;
        case OperandCategory::VARIABLE:
            this->m_var=other.m_var;
            this->m_version=other.m_version;
            break;
        case OperandCategory::ARGUMENT:
            this->m_argument=other.m_argument;
            this->m_version=other.m_version;
            break;
        case OperandCategory::TRIPLE:
            this->m_triple=other.m_triple;
            break;
        case OperandCategory::FUNCTION:
            this->m_function=other.m_function;
            break;
        case OperandCategory::LABEL:
            this->m_label=other.m_label;
            break;
        case OperandCategory::FIELD:
            this->m_field=other.m_field;
            break;
        case OperandCategory::ERROR:
        case OperandCategory::EMPTY:
            break;
        default:
            throw std::runtime_error("Operand::operator=");
        }
        return *this;
    }

    bool operator<(const Operand &other) const {
        if(m_category!=other.m_category){
            return m_category<other.m_category;
        }else{
            switch (m_category)
            {
            case OperandCategory::ARGUMENT:
                if(m_argument!=other.m_argument)
                    return m_argument<other.m_argument;
                else
                    return m_version<other.m_version;
            case OperandCategory::VARIABLE:
                if(m_var!=other.m_var)
                    return m_var<other.m_var;
                else
                    return m_version<other.m_version;
            case OperandCategory::EMPTY:
                return false;
            default:
                throw "Operand::operator<::UNSUPPORTED CATEGORY";
            }
        }
        return false;
    }

    static Operand error(){
        Operand operand;
        operand.m_category=OperandCategory::ERROR;
        return operand;
    }

    DataType get_type() const;
    std::string get_category_as_string() const;
    ~Operand(){}
};

std::ostream& operator<<(std::ostream &os,const Operand &op);

class PHIArgument{
public:
    BasicBlock * m_basic_block;
    Operand m_operand;
    PHIArgument(BasicBlock *blk,Operand op):m_basic_block{blk},m_operand{op}{}
};

class Triple{
public:
    int m_code_line_number;
    bool m_visited;
    BasicBlock * m_basic_block;
    DataType m_cast_type;
    DataType m_data_type;
    DataType m_data_type_for_new;
    size_t m_index;
    Operation m_operation;
    Operand m_op_1;
    Operand m_op_2;
    std::vector<PHIArgument> m_phi_arguments;
    std::vector<Label*> m_pointing_labels;
    std::vector<Operand> m_call_args;
    std::vector<Operand> collect_args_and_vars();
    int get_operand_count();
    Triple(int line_number,size_t index,Operation operation,const Operand &op_1={},const Operand &op_2={}):m_code_line_number{line_number},m_visited{false}, m_index{index},m_operation{operation},m_op_1{op_1},m_op_2{op_2}{}
};

class Error{
public:
    size_t m_code=0;
    size_t m_line;
    std::string m_msg;
    Error(size_t l,const std::string &msg):m_line{l},m_msg{msg}{}
};

enum class SymbolTableCategory{
    ARGUMENT,VARIABLE,FUNCTION,EMPTY,FIELD
};

class Field{
private:
    size_t m_index;
    MyClass *m_owner;
public:
    std::string m_name;
    DataType m_type;
    int get_index() const { return m_index; }
    MyClass * get_class() const { return m_owner; }
    Field(DataType type,const std::string &name,MyClass *cl,size_t index):m_index{index},m_owner{cl},m_name{name},m_type{type}{}
};

class MyClass{
public:
    IntermediateProgram * m_int_program=nullptr;
    std::vector<Function*> m_vtable;
    int m_implicit_declaration_line = -1;
    bool m_defined=false;
    MyClass *m_base_class = nullptr;
    std::string m_name;
    std::vector<Field*> m_fields;
    std::vector<Function*> m_methods;
    Field * get_field(const std::string &name) const;
    void add_field(DataType type,const std::string &name);
    void add_method(Function *fn);
    bool has_method(const std::string &name) const;
    bool inherits(MyClass * cl) const;
    Function * get_method(const std::string &name) const;
    Field *get_field_considering_inheritance(const std::string &name) const;
    Function *get_method_considering_inheritance(const std::string &name) const;
    MyClass(const std::string name,IntermediateProgram *ip):m_int_program{ip},m_name{name}{};
    ~MyClass(){
        for(auto *f : m_fields)
            delete f;
    }
};

enum class PredefinedFunction{
    PRINTINT,
    PRINTSTRING,
    ERROR,
    READINT,
    READSTRING,
    USERDEFINED
};

class Function{
public:
    std::map<Operand,Operand> m_var_arg_to_triple;
    IntermediateProgram * m_int_program=nullptr;
    size_t m_vtable_index;
    MyClass * m_class = nullptr;
    PredefinedFunction m_type;
    bool m_used;
    std::string m_name;
    DataType m_return_type;
    std::vector<BasicBlock*> m_basic_blocks;
    std::vector<Variable*> m_variables;
    std::vector<Triple*> m_triples;
    std::vector<Argument*> m_arguments;
    std::vector<Label*> m_labels;
    void add_label(Label *label);
    bool contains_argument(const std::string &name) const;
    Function(IntermediateProgram *ip,PredefinedFunction p=PredefinedFunction::USERDEFINED);
    ~Function();
    // ~Function(){
    //     for(auto *arg: m_arguments)
    //         delete arg;
    // }
};

class SymbolTableEntry{
public:
    SymbolTableCategory m_category;
    union{
        Argument *m_argument;
        Variable *m_variable;
        Function *m_function;
        Field    *m_field;
    };

    SymbolTableEntry(Variable *var):m_category{SymbolTableCategory::VARIABLE},m_variable{var}{}
    SymbolTableEntry(Function *fun):m_category{SymbolTableCategory::FUNCTION},m_function{fun}{}
    SymbolTableEntry(Argument *arg):m_category{SymbolTableCategory::ARGUMENT},m_argument{arg}{}
    SymbolTableEntry(Field *field):m_category{SymbolTableCategory::FIELD},m_field{field}{}
    SymbolTableEntry():m_category{SymbolTableCategory::EMPTY}{}
};

class SymbolTable{
public:
    std::vector<std::map<std::string,SymbolTableEntry>> m_entries;
    IRCoderListener *m_listener = nullptr;

    void push();
    void pop();
    bool add(const std::string &entry_name,const SymbolTableEntry &entry);
    void add_override(const std::string &entry_name,const SymbolTableEntry &entry);
    void set_listener(IRCoderListener *listener);
    const SymbolTableEntry* get_entry(const std::string &name);
    Variable* get_variable(const std::string &name);
    Argument* get_argument(const std::string &name);
    Function * get_function(const std::string &name);
    const SymbolTableEntry* get_entry_at_top(const std::string &name);
    SymbolTable();
};

#endif
