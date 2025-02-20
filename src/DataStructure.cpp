#include "DataStructure.h"
#include "IRCoder.h"

Function::Function(IntermediateProgram *ip,PredefinedFunction p):m_int_program{ip},m_type{p},m_used{false}{
    IRCoder coder;
    coder.set_function(this);
    coder.push(0,Operation::START);
}
Function::~Function(){
    for(auto *arg: m_arguments)
        delete arg;
}

bool DataType::operator==(const DataType &other) const{
    if(category==other.category && dimensions==other.dimensions){
        if(category==DataTypeCategory::BASIC)
            return basic_type==other.basic_type;
        else
            return class_type == other.class_type;
    }else
        return false;
}

bool DataType::operator==(BasicType other) const {
    return category==DataTypeCategory::BASIC && basic_type==other && dimensions == 0;
}

bool DataType::operator!=(const DataType &other) const {
    return !(*this==other);
}

bool DataType::operator!=(BasicType other) const {
    return !(*this==other);
}

bool DataType::isNullable() const {
    return category==DataTypeCategory::CLASS || dimensions>=1 || (*this)==BasicType::STRING;
}

DataType DataType::decrement_dimensions() const {
    return category==DataTypeCategory::BASIC ? DataType{basic_type,dimensions-1} : DataType{class_type,dimensions-1};
}

DataType DataType::increment_dimensions() const {
    return category==DataTypeCategory::BASIC ? DataType{basic_type,dimensions+1} : DataType{class_type,dimensions+1};
}

std::ostream& operator<<(std::ostream &os,const ArgOrVar &arg)
{
  bool result = std::holds_alternative<Argument*>(arg);
  if(result){
    auto argument = std::get<Argument*>(arg);
    os<<"Argument("<<argument->m_identifier<<")";
  }else{
    auto variable = std::get<Variable*>(arg);
    os<<"Variable("<<variable->m_ident<<")";
  }

  return os;
}

std::ostream& operator<<(std::ostream &os,const Operand &op)
{
    switch (op.m_category)
    {
    case OperandCategory::VARIABLE:
    {
        os<<"Variable("<<op.m_var->m_ident<<"."<<op.m_version<<")";
        break;
    }
    case OperandCategory::ARGUMENT:
    {
        os<<"Argument("<<op.m_argument->m_identifier<<")";
        break;
    }
    case OperandCategory::CONSTANT:{
        os<<"Constant("<<op.m_constant.get_value_as_string()<<")";
        break;
    }
    case OperandCategory::FIELD:{
        os<<"Class: "<<op.m_field->get_class()->m_name<<" Field("<<op.m_field->m_name<<")";
        break;
    }
    case OperandCategory::FUNCTION:{
        os<<"Function: "<<op.m_function->m_name<<" ";
        os<<"Argumnts(";
        int index = 0;
        for(auto arg : op.m_function->m_arguments){
            if(index>0)
                os<<",";
            os<<"Type:"<<data_type_to_string(arg->m_type)<<" Id: "<<arg->m_identifier;
        }
        break;
    }
    case OperandCategory::LABEL:{
        os<<"Label"<<op.m_label->m_index<<" jumps to triple: "<<op.m_label->m_jump_to->m_index;
        break;
    }
    case OperandCategory::TRIPLE:
        os<<"Triple"<<op.m_triple->m_index<<" op_1: "<<op.m_triple->m_op_1;
        break;
    default:
        os<<"Default Brak"<<std::endl;
        break;
    }

    return os;
}

void MyClass::add_method(Function *fn){
    m_methods.push_back(fn);
}

bool MyClass::has_method(const std::string &name) const {
    for(auto *m : m_methods){
        if(m->m_name==name)
            return true;
    }

    return false;
}

Function *MyClass::get_method(const std::string &name) const{
    for(auto *m : m_methods){
        if(m->m_name == name)
            return m;
    }

    return nullptr;
}


Field *MyClass::get_field(const std::string &name) const{
    for(size_t i=0;i<m_fields.size();i++){
        if(m_fields[i]->m_name==name)
            return m_fields[i];
    }

    return nullptr;
}

Field *MyClass::get_field_considering_inheritance(const std::string &name) const{
    Field *f = get_field(name);
    if(f)
        return f;

    return m_base_class ? m_base_class->get_field_considering_inheritance(name) : nullptr;
}

bool MyClass::inherits(MyClass * cl) const {
    MyClass *current_cl = m_base_class;
    while(current_cl!=nullptr){
        if(current_cl==cl)
            return true;
        current_cl =current_cl->m_base_class;
    }

    return false;
}

Function *MyClass::get_method_considering_inheritance(const std::string &name) const {
    Function * method = get_method(name);
    if(method)
        return method;

    return m_base_class ? m_base_class->get_method_considering_inheritance(name) : nullptr;
}

void MyClass::add_field(DataType type,const std::string &name){
    m_fields.push_back(new Field {type,name,this,m_fields.size()});
}

int Triple::get_operand_count(){
    switch (m_operation)
    {
    case Operation::MUL:
    case Operation::ADD:
    case Operation::SUB:
    case Operation::DIV:
    case Operation::MOD:
    case Operation::LTH:
    case Operation::LE:
    case Operation::GTH:
    case Operation::GE:
    case Operation::EQU:
    case Operation::NE:
    case Operation::INIT:
    case Operation::ASSIGN:
    case Operation::ACCESS_ARRAY:
    case Operation::ACCESS_ARRAY_FOR_READ:
    case Operation::ACCESS_ARRAY_FOR_WRITE:
    case Operation::MEMEBER_ACCESS:
    case Operation::MEMBER_FOR_WRITE:
    case Operation::MEMBER_FOR_READ:
    case Operation::JF:
    case Operation::JT:
        return 2;
    case Operation::NEG:
    case Operation::NOT:
    case Operation::NEW_ARRAY:
    case Operation::ARRAY_LENGTH:
    case Operation::CAST:
    case Operation::JMP:
    case Operation::RETURN:
    case Operation::INC:
    case Operation::DEC:
    case Operation::CALL:
        return 1;
    case Operation::START:
    case Operation::NEW_INSTANCE:
    case Operation::MARKER:
    case Operation::PHI:
        return 0;
    default:
        throw std::runtime_error("Triple::get_operand_count()");
    }
}

std::vector<Operand> Triple::collect_args_and_vars(){
  int operand_count = get_operand_count();

  std::vector<Operand> op_refs;
  if(operand_count==1){
    OperandCategory op1_cat = m_op_1.m_category;
    if(op1_cat==OperandCategory::ARGUMENT || op1_cat==OperandCategory::VARIABLE)
      op_refs.push_back(m_op_1);
  }else if(operand_count==2){
    OperandCategory op1_cat = m_op_1.m_category;
    OperandCategory op2_cat = m_op_2.m_category;
    if(op1_cat==OperandCategory::ARGUMENT || op1_cat==OperandCategory::VARIABLE)
      op_refs.push_back(m_op_1);

    if(op2_cat==OperandCategory::ARGUMENT || op2_cat==OperandCategory::VARIABLE)
      op_refs.push_back(m_op_2);
  }

  if(m_operation==Operation::CALL){
    for(auto arg : m_call_args){
      if(arg.m_category==OperandCategory::ARGUMENT || arg.m_category==OperandCategory::VARIABLE)
        op_refs.push_back(arg);
    }
  }

  return op_refs;
}

std::string data_type_to_string(const DataType &type){
    std::string basic_type;
    if(type.category==DataTypeCategory::BASIC){
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
            throw std::runtime_error("data_type_to_string");
        }
        std::string arr="";
        for(int i =0;i<type.dimensions;i++)
            arr+="[]";

        std::string final_type = basic_type+arr;

        return final_type;
    }else{
        return type.class_type->m_name;
    }
}

Constant::Constant(nullptr_t ptr):m_type{BasicType::NULLPTR}{}

Constant::Constant(int value):m_type{BasicType::INT}{
    u.integer=value;
}

Constant::Constant(bool value):m_type{BasicType::BOOL}{
    u.boolean=value;
}

Constant::Constant(const std::string &s):m_type{BasicType::STRING}{
    new (&u.str) std::string(s);
    u.str=s;
}

Constant& Constant::operator=(const Constant &other){
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
    if(m_type==BasicType::STRING)
        call_destructor(&u.str);
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
        break;
    }
    default:
        break;
    }
}


bool Function::contains_argument(const std::string &name) const{
    for(size_t i=0;i<m_arguments.size();i++){
        if(name==m_arguments[i]->m_identifier)
            return true;
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
    case OperandCategory::FIELD:
        return m_field->m_type;
    case OperandCategory::ERROR:
        return BasicType::ERROR;
    default:
        throw std::runtime_error("Operand::get_type()");
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
    case OperandCategory::FIELD:
        return "field";
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
    if(m_entries.back().count(entry_name)>0)
        return false;

    m_entries.back()[entry_name]=entry;
    return true;
}

void SymbolTable::add_override(const std::string &entry_name,const SymbolTableEntry &entry){
    m_entries.back()[entry_name]=entry;
}

const SymbolTableEntry* SymbolTable::get_entry(const std::string &name){
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
