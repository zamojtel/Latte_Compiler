#ifndef INTERMEDIATE_PROGRAM_STRUCTURE_H
#define INTERMEDIATE_PROGRAM_STRUCTURE_H
#include <iterator>
class BasicBlock{
public:
  // czy dla danej zmiennej mamy phi
  // if operand in our case variable or argument is in the set then it has phi function
  std::set<Operand> m_variable_has_phi;
  Function* m_fn = nullptr;
  size_t m_index;
  int m_number;
  BasicBlock* m_idom = nullptr;
  std::vector<BasicBlock*> m_dominator_tree_successors;
  std::set<BasicBlock*> m_dominance_frontiers;
  Triple* m_first_triple=nullptr;
  std::set<BasicBlock*> m_dominators;
  std::vector<BasicBlock*> m_successors;
  std::vector<BasicBlock*> m_predecessors;
  BasicBlock(Function* fn,size_t index):m_fn{fn},m_index{index},m_number{-1}{}
  Triple* get_last_triple();
  using iterator = std::vector<Triple*>::iterator;
  using const_iterator= std::vector<Triple*>::const_iterator;
  // TODO pierwsz i ostatnia trojka w funkcji
  iterator begin(){
    if(m_first_triple)
      return m_fn->m_triples.begin()+m_first_triple->m_index;
    else
      return m_fn->m_triples.begin();
  }

  const_iterator begin() const{
    // return m_fn->m_triples.begin()+m_first_triple->m_index;
    if(m_first_triple)
      return m_fn->m_triples.begin()+m_first_triple->m_index;
    else
      return m_fn->m_triples.begin();
  }

  iterator end(){
    // Obejrzec 1
    size_t next_blk_index = m_index+1;
    if(next_blk_index >= m_fn->m_basic_blocks.size())
      return m_fn->m_triples.end();

    BasicBlock* next_blk = m_fn->m_basic_blocks[next_blk_index];
    return m_fn->m_triples.begin()+next_blk->m_first_triple->m_index;
  }

  const_iterator end() const {
    size_t next_blk_index = m_index+1;
    if(next_blk_index >= m_fn->m_basic_blocks.size())
      return m_fn->m_triples.end();

    BasicBlock* next_blk = m_fn->m_basic_blocks[next_blk_index];
    return m_fn->m_triples.begin()+next_blk->m_first_triple->m_index;
  }

};

int get_operation_operand_count(Operation op);

class IntermediateProgram{
public:
  // variable/argument versioning
  std::map<Operand,int> m_counter;
  std::vector<Operand> m_versioning_stack;
  std::vector<Function*> m_functions;
  std::vector<MyClass*> m_classes;
  IntermediateProgram(){}
  bool check_all_returns(Function *fn,size_t start);
  bool find_path_without_return(Function *fn,size_t start);
  bool has_function(const std::string & fn_name) const;
  bool has_class(const std::string & cl_name) const;
  Function* get_function(const std::string &name) const;
  MyClass* get_class(const std::string &name) const;
  void create_basic_blocks(Function *fn);
  void create_basic_blocks();
  void print_blocks();
  void print_predecessors();
  void print_predecessors_blk(BasicBlock *blk);
  void print_dominator_sets();
  void connect_blocks(BasicBlock *source,BasicBlock *destination);
  void compute_dominator_sets(Function *fn);
  void compute_dominance_frontiers(Function *fn);
  void create_virtual_tables();
  void enumarate(BasicBlock *blk,int &counter);
  void enumerate_basic_blocks(Function * fn);
  void enumerate_all_functions();
  void optimize(Function *fn);
  int new_name(Operand &op);
  void rename(BasicBlock* blk);
  void rename_ssa_function(Function* fn);
  std::vector<Operand> collect_args_and_vars(Triple *triple);
  void insert_phi(Function *fn);
  void print_idoms_for_function(Function *fn);
  void create_vtable(MyClass *cl);
  void correct_labels(Function *fn);
  void optimize();
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
        std::cout<<operand.m_var->m_ident<<"."<<operand.m_version<<" ";
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
        std::cout<<operand.m_argument->m_identifier<<"."<<operand.m_version<<" ";
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
      case Operation::MOD:
        return "MOD";
      case Operation::PHI:
        return "PHI";
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

  void print_triple(Triple * triple){
    std::cout<<"t"<<triple->m_index<<": "<<operation_to_string(triple->m_operation)<<" ";
    print_operand(triple->m_op_1);
    print_operand(triple->m_op_2);

    std::cout<<"Type: "<<data_type_to_string(triple->m_data_type)<<" ";
    if(triple->m_operation==Operation::CALL)
      print_arguments(triple);
    else if(triple->m_operation==Operation::PHI){
      for(auto phi_arg : triple->m_phi_arguments){
        std::cout<<"Block: "<<phi_arg.m_basic_block->m_index<<" ";
        print_operand(phi_arg.m_operand);
      }
    }
    std::cout<<std::endl;
  }

  void print_triples(Function * fn){
    BasicBlock *current_blk=nullptr;
    for(auto *triple: fn->m_triples){
      if(triple->m_basic_block!=current_blk){
        current_blk = triple->m_basic_block;
        std::cout<<std::endl;
        std::cout<<"Block: "<<current_blk->m_index<<" Predecessors: (";
        for(auto predecessor : current_blk->m_predecessors)
          std::cout<<predecessor->m_index<<" ";
        std::cout<<std::endl;
      }
      print_triple(triple);
      // std::cout<<"t"<<triple->m_index<<": "<<operation_to_string(triple->m_operation)<<" ";
      // print_operand(triple->m_op_1);
      // print_operand(triple->m_op_2);

      // std::cout<<"Type: "<<data_type_to_string(triple->m_data_type)<<" ";
      // if(triple->m_operation==Operation::CALL)
      //   print_arguments(triple);
      // else if(triple->m_operation==Operation::PHI){
      //   for(auto phi_arg : triple->m_phi_arguments){
      //     std::cout<<"Block: "<<phi_arg.m_basic_block->m_index<<" ";
      //     print_operand(phi_arg.m_operand);
      //   }
      // }
      // std::cout<<std::endl;
    }
  }

  void print_predecessors_blk(BasicBlock *blk){
    int i = 0;
    for(auto *pred_blk : blk->m_predecessors){
      if(i>0)
        std::cout<<",";
      std::cout<<pred_blk->m_index;
      i++;
    }
  }

  void print_function(Function *fn){
    if(fn->m_basic_blocks.size()==0){
      print_triples(fn);
    }else{
      for(auto blk : fn->m_basic_blocks){
        std::cout<<"Block: "<<blk->m_index<<" (";
        print_predecessors_blk(blk);
        std::cout<<")";
        std::cout<<std::endl;
        for(auto triple : *blk){
          print_triple(triple);
        }
        std::cout<<std::endl;
      }
    }
  }

  void print_program(const IntermediateProgram& ip){
    for(auto fn : ip.m_functions){
      if(fn->m_type==PredefinedFunction::USERDEFINED){
        std::cout<<"Function Name: "<<fn->m_name<<std::endl;
        print_function(fn);
      }
    }
  }

  // void print_program_blocks(const IntermediateProgram& ip){
  //   for(auto fn : ip.m_functions){
  //     if(fn->m_type==PredefinedFunction::USERDEFINED){
  //       std::cout<<"Function Name: "<<fn->m_name<<std::endl;
  //       print_function(fn);
  //     }
  //   }
  // }

  // void print_function_blocks(Fucntion *fn){
  //   for(){

  //   }
  // }

  void print_dominance_frontiers(Function *fn){
    for(auto *blk : fn->m_basic_blocks){
      std::cout<<"Block"<<blk->m_index<<std::endl;
      std::cout<<"Dominance Frontier:"<<std::endl;
      int i=0;
      std::string all_blocks_indexes="";
      for(auto *el : blk->m_dominance_frontiers){
        if(i>0)
          all_blocks_indexes+=",";
        all_blocks_indexes+=std::to_string(el->m_index);
        i++;
      }
      std::cout<<"{"<<all_blocks_indexes<<"}\n";
    }
  }
  void print_blocks_with_phi(Function *fn){
    std::cout<<"Printing Blocks with phi "<<std::endl;
    for(auto *blk : fn->m_basic_blocks){
      std::cout<<"Block : "<<blk->m_index<<std::endl;
      for(auto op : blk->m_variable_has_phi)
        std::cout<<op<<std::endl;
    }
  }
};

#endif
