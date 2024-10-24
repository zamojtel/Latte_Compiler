#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <list>
#include <string>

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

std::string data_type_to_string(const DataType &type){
    switch (type)
    {
    case DataType::INT:
        return "int";
    case DataType::BOOL:
        return "bool";
    case DataType::STRING:
        return "string";
    case DataType::VOID:
        return "void";
    case DataType::ERROR:
        return "error";
    default:
        throw 0;;
    }
}

enum class Operation{
    MUL,ADD,SUB,DIV,AND,OR,NEG,NOT,ASSIGN,
    MOD,LTH,LE,GTH,GE,EQU,NE,
    //Special Operations
    JT,JF, // jump if true ,jump if false
    MARKER, // It will indicate a special triple
    JMP, 
    CALL, // Function Invocation
    PARAM,
    RETURN
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
    
    Constant(int value):m_type{DataType::INT}{
        u.integer=value;
    }

    Constant(bool value):m_type{DataType::BOOL}{
        u.boolean=value;
    }

    Constant(std::string &s):m_type{DataType::STRING}{
        new (&u.str) std::string(s); // where and what  
        u.str=s;
    }
    
    Constant(const Constant &constant){
        m_type = constant.m_type;
        switch (constant.m_type)
        {
        case DataType::INT:
            u.integer = constant.u.integer;
            break;
        case DataType::BOOL:
            u.boolean = constant.u.boolean;
            break;
        case DataType::STRING:{
            new (&u.str) std::string(constant.u.str);
        }
        default:
            break;
        }
    }

    Constant& operator=(const Constant &other){
        // INT,BOOL,STRING,
        if(this == &other)
            return *this;
        if(this->m_type==DataType::STRING)
            call_destructor(&this->u.str);
        
        m_type=other.m_type;
        switch (other.m_type)
        {
        case DataType::INT:
            this->u.integer=other.u.integer;
            break;
        case DataType::BOOL:
            this->u.boolean=other.u.boolean;
            break;
        case DataType::STRING:{
            new (&u.str) std::string;
            this->u.str=other.u.str;
            break;
        }
        default:
            break;
        }
        return *this;
    }

    std::string get_value_as_string() const {
        switch (m_type)
        {
        case DataType::INT:
            return std::to_string(u.integer);
        case DataType::BOOL:
            return std::to_string(u.boolean);
        case DataType::STRING:
            return u.str;
        default:
            return "error";
        }
    }

    ~Constant(){
        if(m_type==DataType::STRING){
            call_destructor(&u.str);
        }
    }
};

class Label{
public:
    size_t m_index;
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

    //   CONSTANT,VARIABLE,TRIPLE,EMPTY
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
        case OperandCategory::ERROR:
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
    Triple(int line_number,size_t index,Operation operation,const Operand &op_1={},const Operand &op_2={}):m_code_line_number{line_number},m_visited{false}, m_index{index},m_operation{operation},m_op_1{op_1},m_op_2{op_2}{}
};

class Error{
public:
    size_t m_code=0;
    size_t m_line;
    std::string m_msg;
    Error(size_t l,const std::string &msg):m_line{l},m_msg{msg}{}
};

enum PredefinedFunction{
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

bool Function::contains_argument(const std::string &name) const{
    for(size_t i=0;i<m_arguments.size();i++){
        if(name==m_arguments[i]->m_identifier){
            return true;
        }
    }
    
    return false;
}
void Function::add_label(Label *label){
    m_labels.push_back(label);
}

DataType Operand::get_type() const{
    switch (m_category)
    {
    case OperandCategory::CONSTANT:
        return m_constant.m_type;
    case OperandCategory::VARIABLE:
        return m_var->m_type;
    case OperandCategory::TRIPLE:
        return m_triple->m_data_type;
    case OperandCategory::FUNCTION:
        return m_function->m_return_type;
    case OperandCategory::LABEL:
        return DataType::VOID;
    case OperandCategory::EMPTY:
        return DataType::VOID;
    case OperandCategory::ARGUMENT:
        return m_argument->m_type;
    case OperandCategory::ERROR:
        return DataType::ERROR;
    default:
        throw 0;
        break;
    }
}

std::string Operand::get_category_as_string() const{
    switch (m_category)
    {
    case OperandCategory::LABEL:
        return "label";
    case OperandCategory::CONSTANT:
        return "constant";
    case OperandCategory::VARIABLE:
        return "variable";
    case OperandCategory::TRIPLE:
        return "triple";
    case OperandCategory::EMPTY:
        return "empty";
    case OperandCategory::FUNCTION:
        return "function";
    default:
        return "error";
    }
}

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
    
    void push(){
        m_entries.push_back({});
    }

    void pop(){
        m_entries.pop_back();
    }

    bool add(const std::string &entry_name,const SymbolTableEntry &entry){

        if(m_entries.back().count(entry_name)>0)
            return false;

        m_entries.back()[entry_name]=entry;
        return true;
    }

    // to-do
    // std::optional 
    const SymbolTableEntry* get_entry(const std::string &name){
        // we go level by level 
        for(size_t i=m_entries.size();i-->0;){
            if(m_entries[i].count(name))
                return &m_entries[i].at(name);
        }
        return nullptr;
    }
    
    Variable* get_variable(const std::string &name){
        for(size_t i=m_entries.size();i-->0;){
            if(m_entries[i].count(name)){
                if(m_entries[i].at(name).m_category==SymbolTableCategory::VARIABLE)
                    return m_entries[i].at(name).m_variable;
                else
                    return nullptr;
            }
        }
        return nullptr;
    }

    Argument* get_argument(const std::string &name){
        for(size_t i=m_entries.size();i-->0;){
            if(m_entries[i].count(name)){
                if(m_entries[i].at(name).m_category==SymbolTableCategory::ARGUMENT)
                    return m_entries[i].at(name).m_argument;
                else
                    return nullptr;
            }
        }
        return nullptr;
    }

    Function * get_function(const std::string &name){
        for(size_t i=m_entries.size();i-->0;){
            if(m_entries[i].count(name)){
                if(m_entries[i].at(name).m_category==SymbolTableCategory::FUNCTION)
                    return m_entries[i].at(name).m_function;
                else
                    return nullptr;
            }
        }
        return nullptr;
    }

    const SymbolTableEntry* get_entry_at_top(const std::string &name){
        if(m_entries.back().count(name)){
            return &m_entries.back().at(name);
        }else
            return nullptr;
    }
    
    SymbolTable(){}
};

#endif 