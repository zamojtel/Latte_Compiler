#ifndef INTERMEDIATE_PROGRAM_STRUCTURE_H
#define INTERMEDIATE_PROGRAM_STRUCTURE_H

class IntermediateProgram{
private:
public:
  std::vector<Function*> m_functions;
  IntermediateProgram(){}
  bool check_all_returns(Function *fn,size_t start);
  bool find_path_without_return(Function *fn,size_t start);
  ~IntermediateProgram(){
    for(auto *fn: m_functions)
      delete fn;
  }
};

// przeniesc do cpp 
class IntermediateProgramPrinter{
public:

  void print_constant(const Constant &constant){
    switch (constant.m_type)
    {
    case DataType::INT:
      std::cout<<"INT("<<constant.u.integer<<") ";
      break;
    case DataType::BOOL:
      std::cout<<"BOOL("<<constant.u.boolean<<")";
      break;
    case DataType::STRING:
      std::cout<<"STRING("<<constant.u.str<<")";
      break;
    default:
      break;
    }
  }

  void print_operand(const Operand &operand){
    switch (operand.m_category)
      {
      case OperandCategory::CONSTANT:
        print_constant(operand.m_constant);
        break;
      case OperandCategory::VARIABLE:
        std::cout<<operand.m_var->m_ident<<" ";
        break;
      case OperandCategory::TRIPLE:
        std::cout<<"t"<<operand.m_triple->m_index<<" ";
        break;
      case OperandCategory::EMPTY:
        std::cout<<"- ";
        break;
      case OperandCategory::LABEL:
        if(operand.m_label->m_jump_to!=nullptr)
          std::cout<<"t"<<operand.m_label->m_jump_to->m_index<<" ";
        else
          std::cout<<"No label set ";
        break;
      case OperandCategory::FUNCTION:
        std::cout<<operand.m_function->m_name<<" ";
        break;
      case OperandCategory::ARGUMENT:
        std::cout<<operand.m_argument->m_identifier<<" ";
        break;
      case OperandCategory::ERROR:
        std::cout<<"error ";
        break;
      default:
        throw 0;
        break;
    }
  }

  std::string operation_to_string(Operation operation){
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

  void print_triples(Function * fn){
    for(auto *triple: fn->m_triples){
      std::cout<<"t"<<triple->m_index<<": "<<operation_to_string(triple->m_operation)<<" ";
      print_operand(triple->m_op_1);
      print_operand(triple->m_op_2);
      std::cout<<"Type: "<<data_type_to_string(triple->m_data_type)<<" ";
      std::cout<<std::endl;
    }
  }

  void print_function(Function *fn){
    print_triples(fn);
  }

  void print_program(IntermediateProgram &ip){
    for(auto fn : ip.m_functions){
      if(fn->m_type==PredefinedFunction::USERDEFINED){
        std::cout<<"Function Name: "<<fn->m_name<<std::endl;
        print_function(fn);
      }
    }
  }

};

#endif