#include "IRCoder.h"
#include "Libraries.h"

IRCoder::TypeCompatibility IRCoder::equal_data_types_or_error(DataType t1,DataType t2){
  if(t1==t2)
    return TypeCompatibility::COMPATIBLE;

  if(t1==BasicType::ERROR || t2==BasicType::ERROR)
    return TypeCompatibility::COMPATIBLE;

  if(t1==BasicType::NULLPTR)
    std::swap(t1,t2);

  if(t2==BasicType::NULLPTR)
    return t1.isNullable() ? TypeCompatibility::COMPATIBLE : TypeCompatibility::INCOMPATIBLE;

  if(t1.category!=DataTypeCategory::CLASS || t2.category!=DataTypeCategory::CLASS)
    return TypeCompatibility::INCOMPATIBLE;

  if(t2.class_type->inherits(t1.class_type))
    return TypeCompatibility::CAST_RIGHT_TO_LEFT;

  if(t1.class_type->inherits(t2.class_type))
    return TypeCompatibility::CAST_LEFT_TO_RIGHT;

  return TypeCompatibility::INCOMPATIBLE;
}


// Todo handle in case of a null value
void IRCoder::set_listener(IRCoderListener *listener){
  m_listener = listener;
}

// Todo change the name of the function
void IRCoder::set_function(Function *fn){
    m_current_fn=fn;
    m_position=m_current_fn->m_triples.size();
};

void IRCoder::set_position_after(Function *fn,Triple * triple){
  m_current_blk=triple->m_basic_block;
  m_current_fn=fn;
  m_position=triple->m_index+1;
};

void IRCoder::set_position_before(Function *fn,Triple * triple ){
  m_current_blk=triple->m_basic_block;
  m_current_fn=fn;
  m_position=triple->m_index;
};

void IRCoder::set_basic_block(BasicBlock * blk){
  m_current_blk = blk;
};

void IRCoder::set_position_after(Triple * triple){
  m_current_blk=triple->m_basic_block;
  m_position=triple->m_index+1;
};

void IRCoder::set_position_before(Triple * triple ){
  m_current_blk=triple->m_basic_block;
  m_position=triple->m_index;
};

void IRCoder::insert_cast_and_replace_op(Triple *triple,Operand &op, DataType type){
    size_t p = m_position;
    set_position_before(triple);
    bool add_one = m_position<=p;
    Triple * t = push_no_check(triple->m_code_line_number,Operation::CAST,op);
    t->m_cast_type = type;
    analyze_triple(t);
    if(add_one)
      m_position=p+1;
    else
      m_position=p;

    op=t;
}

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

void IRCoder::analyze_triple(Triple *triple){
  DataType type =  deduce_type(triple);
  triple->m_data_type=type;
  check_triple(triple);
}

void IRCoder::check_triple(Triple * triple){
    switch (triple->m_operation)
    {
    case Operation::ARRAY_LENGTH:{
      DataType t1 = triple->m_op_1.get_type();
      if(t1.dimensions==0){
        std::string msg = fmt::format("Expected array type");
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::ACCESS_ARRAY:{
      DataType t1 = triple->m_op_1.get_type();
      DataType t2 = triple->m_op_2.get_type();
      if(t1.dimensions==0){
        std::string msg = fmt::format("Expected array type");
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      if(t2.basic_type!=BasicType::INT){
        std::string msg = fmt::format("Expected integer type");
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::INIT:
    case Operation::ASSIGN:{
      DataType t1 = triple->m_op_1.get_type();
      DataType t2 = triple->m_op_2.get_type();
      switch (equal_data_types_or_error(t1,t2))
      {
      case TypeCompatibility::INCOMPATIBLE:
      case TypeCompatibility::CAST_LEFT_TO_RIGHT:
      {
        std::string op_1 = data_type_to_string(triple->m_op_1.get_type());
        std::string op_2 = data_type_to_string(triple->m_op_2.get_type());
        // todo triple index should be shown in debug version only
        std::string msg;
        if(m_debug){
          int index = triple->m_index;
          msg = fmt::format("Incompatible types | op1 type {} op2 type {} triple index = {}",op_1,op_2,index);
        }else
          msg = fmt::format("Incompatible types");

        m_listener->ircoder_error(triple->m_code_line_number,msg);
        break;
      }
      case TypeCompatibility::CAST_RIGHT_TO_LEFT:{
        insert_cast_and_replace_op(triple,triple->m_op_2,triple->m_op_1.get_type());
        break;
      }
      case TypeCompatibility::COMPATIBLE:
        break;
      default:
        break;
      }

      break;
    }
    case Operation::RETURN:
    {
      // Obejrzec
      DataType type = triple->m_op_1.get_type();

      switch (equal_data_types_or_error(m_current_fn->m_return_type,type))
      {
      case TypeCompatibility::COMPATIBLE:
        break;
      case TypeCompatibility::CAST_RIGHT_TO_LEFT:
      {
        insert_cast_and_replace_op(triple,triple->m_op_1,m_current_fn->m_return_type);
        break;
      }
      default:
        std::string msg = fmt::format("Incompatible return type");
        m_listener->ircoder_error(triple->m_code_line_number,msg);
        break;
      }
      break;
    }
    case Operation::ADD:{
      DataType t1 = triple->m_op_1.get_type();
      DataType t2 = triple->m_op_2.get_type();
      // Przejrzec #1
      //  if(equal_data_types_or_error(t1,t2)==TypeCompatibility::COMPATIBLE){
       if(equal_data_types_or_error(t1,t2)==TypeCompatibility::INCOMPATIBLE){
        // if(t1 == BasicType::INT || t1 == BasicType::STRING || (t1 == BasicType::ERROR && t2 == BasicType::ERROR))
        //   break;

        std::string op_1 =data_type_to_string(triple->m_op_1.get_type());
        std::string op_2 =data_type_to_string(triple->m_op_2.get_type());
        std::string msg;
        if(m_debug){
          int index = triple->m_index;
          msg = fmt::format("Incompatible types | op1 type {} op2 type {} triple index = {}",op_1,op_2,index);
        }else
          msg = fmt::format("Incompatible types");

        // std::string msg = fmt::format("Incompatible type | op1 type {} op2 type {} triple index = {}",op_1,op_2,index);
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::NE:
    case Operation::EQU:
    {
      DataType t1 = triple->m_op_1.get_type();
      DataType t2 = triple->m_op_2.get_type();

      switch (equal_data_types_or_error(t1,t2))
      {
      case TypeCompatibility::INCOMPATIBLE:{
        int index = triple->m_index;
        std::string op_1 =data_type_to_string(triple->m_op_1.get_type());
        std::string op_2 =data_type_to_string(triple->m_op_2.get_type());
        std::string msg = fmt::format("Incompatible type | op1 type {} op2 type {} triple index = {}",op_1,op_2,index);
        if(m_debug){
          int index = triple->m_index;
          msg = fmt::format("Incompatible types | op1 type {} op2 type {} triple index = {}",op_1,op_2,index);
        }else
          msg = fmt::format("Incompatible types");

        m_listener->ircoder_error(triple->m_code_line_number,msg);
        break;
      }
      case TypeCompatibility::CAST_LEFT_TO_RIGHT:{
        insert_cast_and_replace_op(triple,triple->m_op_1,t2);
        break;
      }
      case TypeCompatibility::CAST_RIGHT_TO_LEFT:{
        insert_cast_and_replace_op(triple,triple->m_op_2,t1);
        break;
      }
      case TypeCompatibility::COMPATIBLE:{
        if(t1==BasicType::VOID)
          m_listener->ircoder_error(triple->m_code_line_number,"Void types can't be compared");
        break;
      }
      default:
        break;
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
      if(equal_data_types_or_error(t1,t2)==TypeCompatibility::INCOMPATIBLE){
        std::string op_1 =data_type_to_string(triple->m_op_1.get_type());
        std::string op_2 =data_type_to_string(triple->m_op_2.get_type());

        std::string msg;
        if(m_debug){
          int index = triple->m_index;
          msg = fmt::format("Incompatible types | op1 type {} op2 type {} triple index = {}",op_1,op_2,index);
        }else
          msg = fmt::format("Incompatible types");

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
      if(t1!=BasicType::BOOL && t1==BasicType::ERROR)
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
      if(triple->m_data_type_for_new==BasicType::VOID){
        std::string msg = fmt::format("Cannot create an array of void");
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
      break;
    }
    case Operation::CALL:{
      auto fn = triple->m_op_1.m_function;

      if(triple->m_call_args.size() == fn->m_arguments.size()){
          for(size_t i=0;i<fn->m_arguments.size(); i++){
            switch( equal_data_types_or_error(triple->m_call_args[i].get_type(),fn->m_arguments[i]->m_type))
            {
            case TypeCompatibility::COMPATIBLE:
              continue;
            case TypeCompatibility::INCOMPATIBLE:
            case TypeCompatibility::CAST_RIGHT_TO_LEFT:
            {
              std::string msg = fmt::format("Incompatible argument number {} in function {}",i,fn->m_name);
              m_listener->ircoder_error(triple->m_code_line_number,msg);
              break;
            }
            case TypeCompatibility::CAST_LEFT_TO_RIGHT:{
            {
              insert_cast_and_replace_op(triple,triple->m_call_args[i],fn->m_arguments[i]->m_type);
              break;
            }
            default:
              break;
            }
          }
        }
      }else{
        std::string msg = fmt::format("Incorrect number of arguments");
        m_listener->ircoder_error(triple->m_code_line_number,msg);
      }
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

DataType IRCoder::deduce_bool_type_for_inequality_operators(DataType op_1_type,DataType op_2_type){
  if(op_1_type.basic_type==BasicType::INT && op_2_type.basic_type==BasicType::INT)
    return BasicType::BOOL;
  else
    return BasicType::ERROR;
}

DataType IRCoder::deduce_bool_type_for_equality(DataType op_1_type,DataType op_2_type){
  if(op_1_type==op_2_type){
    if(op_1_type.basic_type==BasicType::VOID)
      return BasicType::ERROR;
    else
      return BasicType::BOOL;
  }

  if(op_1_type==BasicType::NULLPTR)
    std::swap(op_1_type,op_2_type);

  if(op_1_type.isNullable() && op_2_type==BasicType::NULLPTR)
    return BasicType::BOOL;

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
  // obejrzec
  switch (triple->m_operation)
  {
  case Operation::CAST:
    return triple->m_cast_type;
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
  case Operation::LTH:
  case Operation::LE:
  case Operation::GTH:
  case Operation::GE:
    return deduce_bool_type_for_inequality_operators(op_1_type,op_2_type);
  case Operation::EQU:
  case Operation::NE:
    return deduce_bool_type_for_equality(op_1_type,op_2_type);
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
    return triple->m_data_type_for_new.increment_dimensions();
  case Operation::ACCESS_ARRAY:{
    if(op_1_type.dimensions>0)
      return op_1_type.decrement_dimensions(); // okreslanie typu elementu do ktorego uzyskujemy dostep
    else
      return BasicType::ERROR;
  }
  case Operation::ARRAY_LENGTH:
    return BasicType::INT;
  case Operation::NEW_INSTANCE:
    return triple->m_data_type_for_new;
  case Operation::MEMEBER_ACCESS:
    return op_2_type;
  case Operation::PHI:
    return triple->m_op_1.get_type();
  default:
    throw std::runtime_error("IRCoder::deduce_type(Triple *triple)");
  }
}

Triple* IRCoder::push(int line_number,Operation operation,const Operand &op_1,const Operand &op_2){
  Triple * triple = push_no_check(line_number,operation,op_1,op_2);
  analyze_triple(triple);
  return triple;
}

Triple* IRCoder::push_no_check(int line_number,Operation operation,const Operand &op_1,const Operand &op_2){
    Triple *triple = new Triple{line_number,m_position,operation,op_1,op_2};

    m_current_fn->m_triples.insert(m_current_fn->m_triples.begin()+ m_position,triple);
    triple->m_basic_block=m_current_blk;

    for(size_t i=m_position+1;i<m_current_fn->m_triples.size();i++)
      m_current_fn->m_triples[i]->m_index=i;

    m_position++;
    // setting a new first triple of a block
    if(m_current_blk!=nullptr && triple->m_index < m_current_blk->m_first_triple->m_index){
      m_current_blk->m_first_triple=triple;
    }

    return triple;
}
