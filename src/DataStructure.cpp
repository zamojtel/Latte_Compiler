#include "DataStructure.h"

std::string data_type_to_string(const DataType &type){
    std::string basic_type;
    switch (type.basic_type)
    {
    case BasicType::INT:
        basic_type= "int";
        break;
    case BasicType::BOOL:
        basic_type= "bool";
        break;
    case BasicType::STRING:
        basic_type= "string";
        break;
    case BasicType::VOID:
        basic_type= "void";
        break;
    case BasicType::ERROR:
        basic_type= "error";
        break;
    case BasicType::NULLPTR:
        basic_type= "nullptr";
        break;
    default:
        throw 0;;
    }

    std::string arr="";
    for(int i =0;i<type.dimensions;i++)
        arr+="[]";
    
    std::string final_type = basic_type+arr;

    return final_type;
}

Constant::Constant(nullptr_t ptr):m_type{BasicType::NULLPTR}{
}

Constant::Constant(int value):m_type{BasicType::INT}{
    u.integer=value;
}

Constant::Constant(bool value):m_type{BasicType::BOOL}{
    u.boolean=value;
}

Constant::Constant(const std::string &s):m_type{BasicType::STRING}{
    new (&u.str) std::string(s); // where and what  
    u.str=s;
}

Constant& Constant::operator=(const Constant &other){
    // INT,BOOL,STRING,
    if(this == &other)
        return *this;
    if(this->m_type==BasicType::STRING)
        call_destructor(&this->u.str);
    
    m_type=other.m_type;
    switch (other.m_type)
    {
    case BasicType::INT:
        this->u.integer=other.u.integer;
        break;
    case BasicType::BOOL:
        this->u.boolean=other.u.boolean;
        break;
    case BasicType::STRING:{
        new (&u.str) std::string;
        this->u.str=other.u.str;
        break;
    case BasicType::NULLPTR:
        break;
    }
    default:
        break;
    }
    return *this;
}

std::string Constant::get_value_as_string() const {
        switch (m_type)
        {
        case BasicType::INT:
            return std::to_string(u.integer);
        case BasicType::BOOL:
            return std::to_string(u.boolean);
        case BasicType::STRING:
            return u.str;
        case BasicType::NULLPTR:
            return "null";
        default:
            return "error";
        }
}

Constant::~Constant(){
    if(m_type==BasicType::STRING){
        call_destructor(&u.str);
    }
}

Constant::Constant(const Constant &constant){
    m_type = constant.m_type;
    switch (constant.m_type)
    {
    case BasicType::INT:
        u.integer = constant.u.integer;
        break;
    case BasicType::BOOL:
        u.boolean = constant.u.boolean;
        break;
    case BasicType::STRING:{
        new (&u.str) std::string(constant.u.str);
    }
    default:
        break;
    }
}


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
        return BasicType::VOID;
    case OperandCategory::EMPTY:
        return BasicType::VOID;
    case OperandCategory::ARGUMENT:
        return m_argument->m_type;
    case OperandCategory::ERROR:
        return BasicType::ERROR;
    default:
        throw 0;
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

void SymbolTable::push(){
    m_entries.push_back({});
}

void SymbolTable::pop(){
    m_entries.pop_back();
}

bool SymbolTable::add(const std::string &entry_name,const SymbolTableEntry &entry){
    if(m_entries.back().count(entry_name)>0){
        return false;
    }

    m_entries.back()[entry_name]=entry;
    return true;
}

// to-do
// std::optional 
const SymbolTableEntry* SymbolTable::get_entry(const std::string &name){
    // we go level by level 
    for(size_t i=m_entries.size();i-->0;){
        if(m_entries[i].count(name))
            return &m_entries[i].at(name);
    }
    return nullptr;
}

Variable* SymbolTable::get_variable(const std::string &name){
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

Argument* SymbolTable::get_argument(const std::string &name){
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

Function * SymbolTable::get_function(const std::string &name){
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

const SymbolTableEntry* SymbolTable::get_entry_at_top(const std::string &name){
    if(m_entries.back().count(name)){
        return &m_entries.back().at(name);
    }else
        return nullptr;
}

SymbolTable::SymbolTable(){}

void SymbolTable::set_listener(IRCoderListener *listener){
  m_listener = listener;
}