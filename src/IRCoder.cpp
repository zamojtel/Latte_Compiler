#include "IRCoder.h"
#include "Libraries.h"

bool IRCoder::equal_data_types_or_error(DataType t1,DataType t2){
  // 1st for arrays
  return (t1.dimensions!=0 && t2==BasicType::NULLPTR) || t1 == BasicType::ERROR || t2 == BasicType::ERROR || t1==t2;
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
      case Operation::INIT:
        return "INIT";
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

void IRCoder::check_triple(Triple * triple){
    switch (triple->m_operation)
    {
    case Operation::ASSIGN:{
      DataType t1 = triple->m_op_1.get_type();
      DataType t2 = triple->m_op_2.get_type();
      //  ident"["expr"]" -> expr "["expr"]"  -> t[x0]
      if(!equal_data_types_or_error(t1,t2)){
        std::string msg = fmt::format("Incompatible type");
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::INIT:
    {
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
        if(m_current_fn->m_return_type!=BasicType::VOID){
          std::string msg = fmt::format("Incompatible return type");
          m_listener->ircoder_error(triple->m_code_line_number,msg);
        }
      }else if(m_current_fn->m_return_type!=triple->m_op_1.get_type()){
        std::string msg = fmt::format("Incompatible return type");
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::ADD:{
      DataType t1 = triple->m_op_1.get_type();
      DataType t2 = triple->m_op_2.get_type();
       if(!equal_data_types_or_error(t1,t2) || t1 == BasicType::BOOL){
        std::string msg = fmt::format("Incompatible types");
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::EQU:
    {
      DataType t1 = triple->m_op_1.get_type();
      DataType t2 = triple->m_op_2.get_type();
      if(!equal_data_types_or_error(t1,t2)){
        std::string msg = fmt::format("Incompatible types");
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      
      if(t1==BasicType::VOID)
        m_listener->ircoder_error(triple->m_code_line_number,"Void types can't be compared");
      
      break;
    }
    case Operation::NE:{

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
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }else if(triple->m_op_1.get_type()!=BasicType::INT){
        std::string msg = fmt::format("Can't perform operation {} on operands of type {}",operation_to_string(triple->m_operation),data_type_to_string(triple->m_op_1.get_type()));
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::NOT:
    {
      DataType t1 = triple->m_op_1.get_type();
      if(t1!=BasicType::BOOL)
      {
        std::string msg = fmt::format("Can't negate non boolean types");
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::JF:
    case Operation::JT:
    { 
      DataType t1 = triple->m_op_1.get_type();
      if(t1 != BasicType::BOOL && t1 != BasicType::ERROR){
        std::string msg = fmt::format("Expected boolean expression");
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::AND:
    case Operation::OR:
    {
      DataType t1 = triple->m_op_1.get_type();
      DataType t2 = triple->m_op_2.get_type();

      if(t1!=BasicType::BOOL && t1!=BasicType::ERROR)
      {
        std::string msg = fmt::format("Expected boolean expression");
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      if(t2!=BasicType::BOOL && t2!=BasicType::ERROR)
      {
        std::string msg = fmt::format("Expected boolean expression");
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::NEG:
    case Operation::INC:
    case Operation::DEC:
    {
      DataType t1 = triple->m_op_1.get_type();
      if(t1!=BasicType::INT){
        std::string msg = fmt::format("Invalid type");
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }

      break;
    }
    case Operation::NEW_ARRAY:{

      break;
    }
    default:
      break;
    }
}

DataType IRCoder::deduce_bool_type_one_argument(DataType op_1_type){
  if(op_1_type==BasicType::BOOL)
    return BasicType::BOOL;
  else
    return BasicType::ERROR;
}

DataType IRCoder::deduce_bool_type(DataType op_1_type,DataType op_2_type){
  if(op_1_type==BasicType::INT && op_2_type==BasicType::INT)
    return BasicType::BOOL;
  else if(op_1_type==BasicType::BOOL && op_2_type==BasicType::BOOL)
    return BasicType::BOOL;
  else
    return BasicType::ERROR;
}


DataType IRCoder::deduce_arithmetic_type(Operation operation,DataType op_1_type,DataType op_2_type){
  if(op_1_type!=op_2_type)
    return BasicType::ERROR;

  if(op_1_type==BasicType::INT)
    return BasicType::INT;
  else if(op_1_type==BasicType::STRING && operation==Operation::ADD)
    return BasicType::STRING;
  else 
    return BasicType::ERROR;
}

DataType IRCoder::deduce_arithmetic_type_one_argument(DataType op_1_type){
    if(op_1_type==BasicType::INT)
      return BasicType::INT;
    else
      return BasicType::ERROR;
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
  case Operation::INIT:
    return op_1_type;
  case Operation::NEG:
    return deduce_arithmetic_type_one_argument(op_1_type);
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
    return BasicType::VOID;
  case Operation::CALL:
    return op_1_type;
  case Operation::PARAM:
    return BasicType::VOID;
  case Operation::RETURN:
    return BasicType::VOID; 
  case Operation::MARKER:
    return BasicType::VOID;
  case Operation::INC:
  case Operation::DEC:
    return deduce_arithmetic_type_one_argument(op_1_type);
  case Operation::NEW_ARRAY: // int [] t
    return {triple->m_data_type_for_new.basic_type,triple->m_data_type_for_new.dimensions+1};
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
