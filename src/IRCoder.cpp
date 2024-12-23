#include "IRCoder.h"
#include "Libraries.h"

bool IRCoder::equal_data_types_or_error(DataType t1,DataType t2){
  return  t1 == DataType::ERROR || t2 == DataType::ERROR || t1==t2;
}

// dodac obsluge jak bedzie null 
void IRCoder::set_listener(IRCoderListener *listener){
  m_listener = listener;
}

// Todo change the name of the function 
void IRCoder::set_function(Function *fn){
    m_current_fn=fn;
    m_position=m_current_fn->m_triples.size();
};

void IRCoder::set_position_after(Function *fn,Triple * triple){
  m_current_fn=fn;
  m_position=triple->m_index+1;
};

void IRCoder::set_position_before(Function *fn,Triple * triple ){
  m_current_fn=fn;
  m_position=triple->m_index; 
};

std::string IRCoder::operation_to_string(Operation operation){
    switch (operation)
    {
      case Operation::ADD:{
        return "ADD";
      case Operation::ASSIGN:
        return "ASSIGN";
      case Operation::MUL:
        return "MUL";
      case Operation::SUB:
        return "SUB";
      case Operation::DIV:
        return "DIV";
      case Operation::AND:
        return "AND";
      case Operation::OR:
        return "OR";
      case Operation::LTH:
        return "LTH";
      case Operation::LE:
        return "LE";
      case Operation::GTH:
        return "GTH";
      case Operation::GE:
        return "GE";
      case Operation::EQU:
        return "EQU";
      case Operation::NE:
        return "NE";
      case Operation::JT:
        return "JT";
      case Operation::JF:
        return "JF";
      case Operation::MARKER:
        return "MARKER";
      case Operation::JMP:
        return "JUMP";
      case Operation::CALL:
        return "CALL";
      case Operation::PARAM:
        return "PARAM";
      case Operation::RETURN:
        return "RETURN";
      default:
        return " ";
      }
    }
}

std::string IRCoder::type_to_string(DataType dt){
    switch (dt)
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
      return "";
      break;
    }
}

void IRCoder::check_triple(Triple * triple){
    switch (triple->m_operation)
    {
    case Operation::ASSIGN:{
      DataType t1 = triple->m_op_1.get_type();
      DataType t2 = triple->m_op_2.get_type();

      if(!equal_data_types_or_error(t1,t2)){

        std::string msg = fmt::format("Incompatible type");
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::RETURN:
    {
      if(triple->m_op_1.m_category==OperandCategory::EMPTY){
        if(m_current_fn->m_return_type!=DataType::VOID){
          std::string msg = fmt::format("Incompatible return type");
            //   m_error_list.add_error(triple->m_code_line_number,msg);
          m_listener->ircoder_error(triple->m_code_line_number,msg);
        }
      }else if(m_current_fn->m_return_type!=triple->m_op_1.get_type()){
        std::string msg = fmt::format("Incompatible return type");
        // m_error_list.add_error(triple->m_code_line_number,msg);
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::ADD:
    case Operation::EQU:
    {
      DataType t1 = triple->m_op_1.get_type();
      DataType t2 = triple->m_op_2.get_type();

      if(!equal_data_types_or_error(t1,t2)){
        std::string msg = fmt::format("Incompatible types");
        // m_error_list.add_error(triple->m_code_line_number,msg);
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::SUB:
    case Operation::DIV:
    case Operation::MUL:
    case Operation::MOD:
    case Operation::LTH:
    case Operation::LE:
    case Operation::GTH:
    case Operation::GE:
    {
      DataType t1 = triple->m_op_1.get_type();
      DataType t2 = triple->m_op_2.get_type();
      if(!equal_data_types_or_error(t1,t2)){
        std::string msg = fmt::format("Incompatible types");
        // m_error_list.add_error(triple->m_code_line_number,msg);
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }else if(triple->m_op_1.get_type()!=DataType::INT){
        std::string msg = fmt::format("Can't perform operation {} on operands of type {}",operation_to_string(triple->m_operation),type_to_string(triple->m_op_1.get_type()));
        // m_error_list.add_error(triple->m_code_line_number,msg);
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::NOT:
    {
      DataType t1 = triple->m_op_1.get_type();
      if(t1!=DataType::BOOL)
      {
        std::string msg = fmt::format("Can't negate non boolean types");
        // m_error_list.add_error(triple->m_code_line_number,msg);
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::JF:
    case Operation::JT:
    { 
      DataType t1 = triple->m_op_1.get_type();
      if(t1 != DataType::BOOL && t1 != DataType::ERROR){
        std::string msg = fmt::format("Expected boolean expression");
        // m_error_list.add_error(triple->m_code_line_number,msg);
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::AND:
    case Operation::OR:
    {
      DataType t1 = triple->m_op_1.get_type();
      DataType t2 = triple->m_op_2.get_type();

      if(t1!=DataType::BOOL && t1!=DataType::ERROR)
      {
        std::string msg = fmt::format("Expected boolean expression");
        // m_error_list.add_error(triple->m_code_line_number,msg);
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      if(t2!=DataType::BOOL && t2!=DataType::ERROR)
      {
        std::string msg = fmt::format("Expected boolean expression");
        // m_error_list.add_error(triple->m_code_line_number,msg);
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
    }
    default:
      break;
    }
}

DataType IRCoder::deduce_bool_type_one_argument(DataType op_1_type){
  if(op_1_type==DataType::BOOL)
    return DataType::BOOL;
  else
    return DataType::ERROR;
}

DataType IRCoder::deduce_bool_type(DataType op_1_type,DataType op_2_type){
  if(op_1_type==DataType::INT && op_2_type==DataType::INT)
    return DataType::BOOL;
  else if(op_1_type==DataType::BOOL && op_2_type==DataType::BOOL)
    return DataType::BOOL;
  else
    return DataType::ERROR;
}


DataType IRCoder::deduce_arithmetic_type(Operation operation,DataType op_1_type,DataType op_2_type){
  if(op_1_type!=op_2_type)
    return DataType::ERROR;

  if(op_1_type==DataType::INT)
    return DataType::INT;
  else if(op_1_type==DataType::STRING && operation==Operation::ADD)
    return DataType::STRING;
  else 
    return DataType::ERROR;
}

DataType IRCoder::deduce_arithmetical_neg(DataType op_1_type){
    if(op_1_type==DataType::INT)
      return DataType::INT;
    else
      return DataType::ERROR;
  }

DataType IRCoder::deduce_type(Triple *triple){
  DataType op_1_type=triple->m_op_1.get_type();
  DataType op_2_type=triple->m_op_2.get_type();
  
  switch (triple->m_operation)
  {
  case Operation::ADD:
  case Operation::MUL:
  case Operation::MOD:
  case Operation::SUB:
  case Operation::DIV:
    return deduce_arithmetic_type(triple->m_operation,op_1_type,op_2_type);
  case Operation::NOT:
    return deduce_bool_type_one_argument(op_1_type);
  case Operation::ASSIGN:
    return op_1_type;
  case Operation::NEG:
    return deduce_arithmetical_neg(op_1_type);
  case Operation::AND:
  case Operation::OR:
  case Operation::LTH:
  case Operation::LE:
  case Operation::GTH:
  case Operation::GE:
  case Operation::EQU:
  case Operation::NE:
    return deduce_bool_type(op_1_type,op_2_type);
  case Operation::JT:
  case Operation::JF:
  case Operation::JMP:
    return DataType::VOID;
  case Operation::CALL:
    return op_1_type;
  case Operation::PARAM:
    return DataType::VOID;
  case Operation::RETURN:
    return DataType::VOID; 
  case Operation::MARKER:
    return DataType::VOID;
  default:
    throw 0;
  }
}

Triple* IRCoder::push(int line_number,Operation operation,const Operand &op_1,const Operand &op_2){
    Triple *triple = new Triple{line_number,m_position,operation,op_1,op_2};
    DataType deduced_type = deduce_type(triple);
    triple->m_data_type=deduced_type;

    m_current_fn->m_triples.insert(m_current_fn->m_triples.begin()+ m_position,triple);
    
    for(size_t i=m_position+1;i<m_current_fn->m_triples.size();i++)
      m_current_fn->m_triples[i]->m_index=i;

    check_triple(triple);

    m_position++;
    
    return triple;
}
