#ifndef INTERMEDIATE_PROGRAM_STRUCTURE_H
#define INTERMEDIATE_PROGRAM_STRUCTURE_H

class IntermediateProgram{
public:
  std::vector<Function*> m_functions;
  std::vector<MyClass*> m_classes;
  IntermediateProgram(){}
  bool check_all_returns(Function *fn,size_t start);
  bool find_path_without_return(Function *fn,size_t start);
  bool has_function(const std::string & fn_name) const;
  bool has_class(const std::string & cl_name) const;
  Function* get_function(const std::string &name) const;
  MyClass* get_class(const std::string &name) const;
  void create_virtual_tables();
  void create_vtable(MyClass *cl);
  ~IntermediateProgram(){
    for(auto *fn: m_functions)
      delete fn;
  }
};

// przeniesc do cpp 
class IntermediateProgramPrinter{
public:

  void print_constant(const Constant &constant){
    std::cout<<data_type_to_string(constant.m_type)+"("+constant.get_value_as_string()+")";
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
      case OperandCategory::FIELD:
        std::cout<<operand.m_field->m_name<<" ";
        break;
      case OperandCategory::ERROR:
        std::cout<<"error ";
        break;
      default:
        throw ;
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
      case Operation::NEG:
        return "NEG";
      case Operation::INC:
        return "INC";
      case Operation::DEC:
        return "DEC";
      case Operation::NEW_ARRAY:
        return "NEW_ARRAY";
      case Operation::ACCESS_ARRAY:
        return "ACCESS_ARRAY";
      case Operation::ARRAY_LENGTH:
        return "ARRAY_LENGTH";
      case Operation::NEW_INSTANCE:
        return "NEW_INSTANCE";
      case Operation::MEMEBER_ACCESS:
        return "MEMEBER_ACCESS";
      default:
        throw 1;
      }
    }
  }
  void print_arguments(Triple *triple){
    std::cout<<"{ "; 
    for(auto op : triple->m_call_args)
      print_operand(op);
    std::cout<<"}";
  }

  void print_triples(Function * fn){
    for(auto *triple: fn->m_triples){
      std::cout<<"t"<<triple->m_index<<": "<<operation_to_string(triple->m_operation)<<" ";
      print_operand(triple->m_op_1);
      print_operand(triple->m_op_2);

      std::cout<<"Type: "<<data_type_to_string(triple->m_data_type)<<" ";
      if(triple->m_operation==Operation::CALL)
        print_arguments(triple);

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