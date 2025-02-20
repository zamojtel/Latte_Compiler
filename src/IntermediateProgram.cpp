#include "SSARenamer.h"

int get_operation_operand_count(Operation op){
  int operand_count;
  switch (op)
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
  case Operation::MEMEBER_ACCESS:
  case Operation::JF:
  case Operation::JT:
    operand_count=2;
    break;
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
    operand_count=1;
    break;
  case Operation::NEW_INSTANCE:
  case Operation::MARKER:
    operand_count=0;
    break;
  default:
    break;
  }

  return operand_count;
}

void IntermediateProgram::optimize(){
  for(auto fn : m_functions)
    optimize(fn);

  for(auto cl : m_classes){
    for(auto meth : cl->m_methods)
      optimize(meth);
  }
}

bool IntermediateProgram::has_class(const std::string & cl_name) const{
  for(auto cl : m_classes ){
    if(cl->m_name == cl_name)
      return true;
  }
  return false;
}

bool IntermediateProgram::has_function(const std::string & fn_name) const{
  for(size_t i=0;i<m_functions.size();i++){
    if(m_functions[i]->m_name == fn_name){
      return true;
    }
  }

  return false;
}

void IntermediateProgram::create_vtable(MyClass *cl){
  if(cl->m_vtable.size()>0)
    return;

  if(cl->m_base_class!=nullptr){
    create_vtable(cl->m_base_class);
    for(Function * fn :cl->m_base_class->m_vtable){
      cl->m_vtable.push_back(fn);
    }
  }

  for(Function *meth : cl->m_methods){
    bool overriden = false;
    if(cl->m_base_class!=nullptr){
      for(Function *base_meth : cl->m_base_class->m_vtable){
        if(meth->m_name==base_meth->m_name){
          cl->m_vtable[base_meth->m_vtable_index]=meth;
          meth->m_vtable_index = base_meth->m_vtable_index;
          overriden=true;
        }
      }
    }

    if(!overriden){
      meth->m_vtable_index=cl->m_vtable.size();
      cl->m_vtable.push_back(meth);
    }

  }
}

void IntermediateProgram::create_virtual_tables(){
  for(auto *cl : m_classes)
    create_vtable(cl);
}

Function* IntermediateProgram::get_function(const std::string &name) const{
  for(size_t i=0;i<m_functions.size();i++){
    if(m_functions[i]->m_name==name)
      return m_functions[i];
  }

  return nullptr;
}

void IntermediateProgram::check_difference_between_methods(Function *method,MyClass *base_cl,ErrorList &error_list){
  if(base_cl==nullptr)
    return;
  Function *base_method = base_cl->get_method(method->m_name);
  if(base_method!=nullptr){
    if(method->m_arguments.size()!=base_method->m_arguments.size()){
      std::string msg=fmt::format("Number of arguments between methods differ base: {} current: {}",base_method->m_arguments.size(),method->m_arguments.size());
      error_list.add_error(method->m_decl_line,msg);
      return;
    }

    for(size_t i=0;i<method->m_arguments.size();i++){
      if(i==0)
        continue;

      if(method->m_arguments[i]->m_type != base_method->m_arguments[i]->m_type){
        error_list.add_error(method->m_decl_line,"Types of arguments between inherited methods don't match");
        return;
      }
    }

    if(method->m_return_type!=base_method->m_return_type){
      std::string msg=fmt::format("Return type of overriden methods should be the same");
      error_list.add_error(method->m_decl_line,"Return type of overriden methods should be the same");
      return;
    }

  }

  return check_difference_between_methods(method,base_cl->m_base_class,error_list);
}

void IntermediateProgram::check_all_methods(ErrorList &error_list){
  for(auto cl : m_classes){
    for(auto meth : cl->m_methods){
      check_difference_between_methods(meth,cl->m_base_class,error_list);

      // if(dm.m_error_detected){
      //   error_list.add_error(meth->m_decl_line,dm.m_msg);
      // }
    }
  }
}


MyClass* IntermediateProgram::get_class(const std::string &name) const{
  for(size_t i=0;i<m_classes.size();i++){
    if(m_classes[i]->m_name==name)
      return m_classes[i];
  }

  return nullptr;
}

// this function checks if we can reach the end of the function without finding any return on the way
bool IntermediateProgram::check_all_returns(Function *fn,size_t start){
  if(start >= fn->m_triples.size() || fn->m_triples[start]->m_visited)
    return false;

  fn->m_triples[start]->m_visited=true;

  for(size_t i=start;i<fn->m_triples.size();i++){
    if(fn->m_triples[i]->m_operation==Operation::RETURN)
      return true;

    if(fn->m_triples[i]->m_operation==Operation::JF || fn->m_triples[i]->m_operation==Operation::JT || fn->m_triples[i]->m_operation==Operation::JMP){
      bool jump_possible;
      bool no_jump_possible;

      if(fn->m_triples[i]->m_operation==Operation::JMP){
        jump_possible=true;
        no_jump_possible=false;
      }else{
        switch (fn->m_triples[i]->m_op_1.m_category)
        {
        case OperandCategory::CONSTANT:
        {
          jump_possible = (fn->m_triples[i]->m_op_1.m_constant.u.boolean == false && fn->m_triples[i]->m_operation==Operation::JF)
          || (fn->m_triples[i]->m_op_1.m_constant.u.boolean && fn->m_triples[i]->m_operation==Operation::JT);
          no_jump_possible = !jump_possible;
          break;
        }
        case OperandCategory::VARIABLE:
        case OperandCategory::ARGUMENT:
        case OperandCategory::TRIPLE:
        {
          jump_possible=true;
          no_jump_possible=true;
          break;
        }
        default:
          break;
        }
      }

      if(jump_possible){
        // its enough if one path has a return statement at the end
        size_t index;
        if(fn->m_triples[i]->m_operation==Operation::JMP)
          index = fn->m_triples[i]->m_op_1.m_label->m_jump_to->m_index;
        else
          index = fn->m_triples[i]->m_op_2.m_label->m_jump_to->m_index;

        bool result = check_all_returns(fn,index);
        if(result)
          return true;
      }

      if(no_jump_possible)
        continue;
      else
        break;
    }
  }
  return false;
}

BasicBlock* find_second_gretest_element(const std::set<BasicBlock*> &set,BasicBlock *current_blk){
    int second=INT_MIN;
    int current = current_blk->m_number;
    BasicBlock * second_blk=nullptr;
    for(auto blk : set){
      if(blk->m_number>second && blk->m_number!=current){
          second = blk->m_number;
          second_blk=blk;
      }
    }
    return second_blk;
}


void IntermediateProgram::optimize(Function *fn){
  if(fn->m_type!=PredefinedFunction::USERDEFINED)
    return;
  // create_basic_blocks(fn);
  // numeracja blokow
  enumerate_basic_blocks(fn);

  compute_dominator_sets(fn);
  // kazdy block ma swoi idom
  for(auto *blk : fn->m_basic_blocks){
    auto second_greatest_blk = find_second_gretest_element(blk->m_dominators,blk);
    blk->m_idom=second_greatest_blk;
    if(second_greatest_blk){
      second_greatest_blk->m_dominator_tree_successors.push_back(blk);
    }
  }
  compute_dominance_frontiers(fn);

  insert_phi(fn);

  SSARenamer ssa_renamer;
  ssa_renamer.rename_ssa_function(fn);
}

void IntermediateProgram::compute_dominance_frontiers(Function *fn){

  for(auto *blk : fn->m_basic_blocks){
    blk->m_dominance_frontiers.clear();
  }

  for(auto *n : fn->m_basic_blocks){
    if(n->m_predecessors.size()>1){
      for(auto *predecessor : n->m_predecessors){
        BasicBlock * runner = predecessor;
        while(runner != n->m_idom){
          runner->m_dominance_frontiers.insert(n);
          runner=runner->m_idom;
        }
      }
    }
  }
}

// BASICBLOCK
Triple* BasicBlock::get_last_triple(){
  size_t next_blk_index = m_index+1;
  if(next_blk_index<m_fn->m_basic_blocks.size()){
    Triple* first_triple_next_blk=m_fn->m_basic_blocks.at(next_blk_index)->m_first_triple;
    return m_fn->m_triples.at(first_triple_next_blk->m_index-1);
  }

  return nullptr;
}

BasicBlock::iterator BasicBlock::begin(){
  if(m_first_triple)
    return m_fn->m_triples.begin()+m_first_triple->m_index;
  else
    return m_fn->m_triples.begin();
}

BasicBlock::const_iterator BasicBlock::begin() const{
  if(m_first_triple)
    return m_fn->m_triples.begin()+m_first_triple->m_index;
  else
    return m_fn->m_triples.begin();
}

BasicBlock::iterator BasicBlock::end(){
  size_t next_blk_index = m_index+1;
  if(next_blk_index >= m_fn->m_basic_blocks.size())
    return m_fn->m_triples.end();

  BasicBlock* next_blk = m_fn->m_basic_blocks[next_blk_index];
  return m_fn->m_triples.begin()+next_blk->m_first_triple->m_index;
}

BasicBlock::const_iterator BasicBlock::end() const {
  size_t next_blk_index = m_index+1;
  if(next_blk_index >= m_fn->m_basic_blocks.size())
    return m_fn->m_triples.end();

  BasicBlock* next_blk = m_fn->m_basic_blocks[next_blk_index];
  return m_fn->m_triples.begin()+next_blk->m_first_triple->m_index;
}

std::vector<Operand> IntermediateProgram::collect_args_and_vars(Triple *triple){
  int operand_count=triple->get_operand_count();
  // switch (triple->m_operation)
  // {
  // case Operation::MUL:
  // case Operation::ADD:
  // case Operation::SUB:
  // case Operation::DIV:
  // case Operation::MOD:
  // case Operation::LTH:
  // case Operation::LE:
  // case Operation::GTH:
  // case Operation::GE:
  // case Operation::EQU:
  // case Operation::NE:
  // case Operation::INIT:
  // case Operation::ASSIGN:
  // case Operation::ACCESS_ARRAY:
  // case Operation::MEMEBER_ACCESS:
  // case Operation::JF:
  // case Operation::JT:
  //   operand_count=2;
  //   break;
  // case Operation::NEG:
  // case Operation::NOT:
  // case Operation::NEW_ARRAY:
  // case Operation::ARRAY_LENGTH:
  // case Operation::CAST:
  // case Operation::JMP:
  // case Operation::RETURN:
  // case Operation::INC:
  // case Operation::DEC:
  // case Operation::CALL:
  //   operand_count=1;
  //   break;
  // case Operation::NEW_INSTANCE:
  // case Operation::MARKER:
  //   operand_count=0;
  //   break;
  // default:
  //   break;
  // }

  std::vector<Operand> op_refs;
  if(operand_count==1){
    OperandCategory op1_cat = triple->m_op_1.m_category;
    if(op1_cat==OperandCategory::ARGUMENT || op1_cat==OperandCategory::VARIABLE)
      op_refs.push_back(triple->m_op_1);
  }else if(operand_count==2){
    OperandCategory op1_cat = triple->m_op_1.m_category;
    OperandCategory op2_cat = triple->m_op_2.m_category;
    if(op1_cat==OperandCategory::ARGUMENT || op1_cat==OperandCategory::VARIABLE)
      op_refs.push_back(triple->m_op_1);

    if(op2_cat==OperandCategory::ARGUMENT || op2_cat==OperandCategory::VARIABLE)
      op_refs.push_back(triple->m_op_2);
  }

  if(triple->m_operation==Operation::CALL){
    for(auto arg : triple->m_call_args){
      if(arg.m_category==OperandCategory::ARGUMENT || arg.m_category==OperandCategory::VARIABLE)
        op_refs.push_back(arg);
    }
  }

  return op_refs;
}

void IntermediateProgram::insert_phi(Function *fn){
  std::set<Operand> Globals={};
  std::map<Operand,std::set<BasicBlock*>> Blocks;
  for(auto *blk : fn->m_basic_blocks){
    std::set<Operand> varKill;

    for(auto triple : *blk){
      std::vector<Operand> op_refs = collect_args_and_vars(triple);
      for(size_t i=0;i<op_refs.size();i++){
        if(varKill.count(op_refs[i])==0){
          Globals.insert(op_refs[i]);
        }
      }

      if(triple->m_operation==Operation::ASSIGN || triple->m_operation==Operation::INIT || triple->m_operation==Operation::INC || triple->m_operation==Operation::DEC){
        if(triple->m_op_1.m_category==OperandCategory::ARGUMENT || triple->m_op_1.m_category==OperandCategory::VARIABLE){
          varKill.insert(triple->m_op_1);
          Blocks[triple->m_op_1].insert(blk);
        }
      }
    }
  }

  for(auto x : Globals){
    if(Blocks.count(x)==0)
      continue;

    std::set<BasicBlock*> worklist = Blocks.at(x);
    for(auto blk : worklist){
      for(auto d : blk->m_dominance_frontiers){
        if(!(d->m_variable_has_phi.count(x))){
          d->m_variable_has_phi.insert(x);
          worklist.insert(d);
        }
      }
    }
  }
}

void IntermediateProgram::print_idoms_for_function(Function *fn){
  for(auto blk : fn->m_basic_blocks){
    std::cout<<"Idom for a block: "<<blk->m_index<<std::endl;
    if(blk->m_idom==nullptr)
      std::cout<<"Idom: Brak "<<std::endl;
    else
      std::cout<<"Idom: "<<blk->m_idom->m_index<<std::endl;
  }
}

void IntermediateProgram::connect_blocks(BasicBlock *source_block,BasicBlock *destination_block){
  source_block->m_successors.push_back(destination_block);
  destination_block->m_predecessors.push_back(source_block);
}

void IntermediateProgram::disconnect_blocks(BasicBlock *source_block,BasicBlock *destination_block){
  for(size_t i=0;i<source_block->m_successors.size();i++){
    if(destination_block==source_block->m_successors[i]){
      source_block->m_successors.erase(source_block->m_successors.begin()+i);
      break;
    }
    i++;
  }

  for(size_t j=0;j<destination_block->m_predecessors.size();j++){
    if(source_block==destination_block->m_predecessors[j]){
      destination_block->m_predecessors.erase(destination_block->m_predecessors.begin()+j);
      break;
    }
    j++;
  }
}

void IntermediateProgram::substitute_vars_args(Function *fn){
  struct Helper{
    static const Operand& get_substitution(const std::map<Operand,Operand> var_arg_to_triple,const Operand& op) {
      if(var_arg_to_triple.count(op)>0){
        if(op.m_category==OperandCategory::ARGUMENT && op.m_version==0)
          return op;

        return get_substitution(var_arg_to_triple,var_arg_to_triple.at(op));
      }
      return op;
    }
  };

  for(auto blk : fn->m_basic_blocks){
    for(auto triple : *blk){
      Operand &op_1 = triple->m_op_1;
      Operand &op_2 = triple->m_op_2;
      Operation operation = triple->m_operation;
      if(operation != Operation::ASSIGN && operation!= Operation::INIT){
        if(operation!=Operation::PHI){
          if(op_1.m_category==OperandCategory::ARGUMENT || op_1.m_category==OperandCategory::VARIABLE)
            op_1=Helper::get_substitution(fn->m_var_arg_to_triple,op_1);
        }
      }

      if(op_2.m_category==OperandCategory::ARGUMENT || op_2.m_category==OperandCategory::VARIABLE)
        op_2=Helper::get_substitution(fn->m_var_arg_to_triple,op_2);

      if(operation==Operation::CALL){
        for(auto &op : triple->m_call_args){
          if(op.m_category==OperandCategory::ARGUMENT || op.m_category==OperandCategory::VARIABLE)
            op=Helper::get_substitution(fn->m_var_arg_to_triple,op);
        }
      }
    }
  }
}

void IntermediateProgram::analyze_member_accesses(){
  for(auto fn : m_functions){
    for(auto blk : fn->m_basic_blocks){
      for(auto triple : *blk)
        analyze_member_accesses(triple);
    }
  }

  for(auto cl : m_classes){
    for(auto meth : cl->m_methods){
      for(auto blk: meth->m_basic_blocks){
        for(auto triple : *blk)
          analyze_member_accesses(triple);
      }
    }
  }
}

void IntermediateProgram::analyze_member_accesses(Triple *triple){
  struct Help{
    static void set_access_type(Operand &op,Operation operation){
      if(op.m_category==OperandCategory::TRIPLE){
        if(op.m_triple->m_operation==Operation::MEMEBER_ACCESS)
          op.m_triple->m_operation=operation;
      }
    }
  };

  Operation operation = triple->m_operation;
  switch (operation)
  {
    case Operation::ASSIGN:
    {
      Help::set_access_type(triple->m_op_1,Operation::MEMBER_FOR_WRITE);
      Help::set_access_type(triple->m_op_2,Operation::MEMBER_FOR_READ);
      break;
    }
    case Operation::CALL:{
      for(auto arg : triple->m_call_args)
        Help::set_access_type(arg,Operation::MEMBER_FOR_READ);
      break;
    }
    default:
    {
      Help::set_access_type(triple->m_op_1,Operation::MEMBER_FOR_READ);
      Help::set_access_type(triple->m_op_2,Operation::MEMBER_FOR_READ);
      break;
    }
  }

}

void IntermediateProgram::analyze_array_accesses(Triple * triple){
  struct Help{
    static void set_access_type(Operand &op,Operation operation){
      if(op.m_category==OperandCategory::TRIPLE){
        if(op.m_triple->m_operation==Operation::ACCESS_ARRAY)
          op.m_triple->m_operation=operation;
      }
    }
  };

  Operation operation  = triple->m_operation;
  switch (operation)
  {
    case Operation::ASSIGN:
    {
      Help::set_access_type(triple->m_op_1,Operation::ACCESS_ARRAY_FOR_WRITE);
      Help::set_access_type(triple->m_op_2,Operation::ACCESS_ARRAY_FOR_READ);
      break;
    }
    case Operation::CALL:{
      for(auto arg : triple->m_call_args)
        Help::set_access_type(arg,Operation::ACCESS_ARRAY_FOR_READ);
      break;
    }
    default:
    {
      Help::set_access_type(triple->m_op_1,Operation::ACCESS_ARRAY_FOR_READ);
      Help::set_access_type(triple->m_op_2,Operation::ACCESS_ARRAY_FOR_READ);
      break;
    }
  }
}

void IntermediateProgram::analyze_array_accesses(){
  for(auto fn : m_functions){
    for(auto blk : fn->m_basic_blocks){
      for(auto triple : *blk)
        analyze_array_accesses(triple);
    }
  }

  for(auto fn : m_classes){
    for(auto meth : fn->m_methods){
      for(auto blk : meth->m_basic_blocks){
        for(auto triple : *blk)
          analyze_array_accesses(triple);
      }
    }
  }
}

void IntermediateProgram::substitute_all_vars_and_args(){
  for(auto *fn : m_functions)
    substitute_vars_args(fn);

  for(auto cl : m_classes){
    for(auto meth : cl->m_methods)
      substitute_vars_args(meth);
  }
}

void IntermediateProgram::compute_dominator_sets(Function *fn){
  int n = fn->m_basic_blocks.size();
  fn->m_basic_blocks[0]->m_dominators.insert(fn->m_basic_blocks[0]);
  std::set<BasicBlock*> all_blocks;

  for(auto blk : fn->m_basic_blocks)
    all_blocks.insert(blk);

  for(int i=1;i<n;i++)
    fn->m_basic_blocks[i]->m_dominators = all_blocks;

  bool changed = true;

  while(changed){
    changed = false;
    for(int i=1;i<n;i++){
        std::set<BasicBlock*> intersection = all_blocks;
        for(size_t j=0;j<fn->m_basic_blocks[i]->m_predecessors.size();j++){
          auto pred = fn->m_basic_blocks[i]->m_predecessors[j];
          std::set<BasicBlock*> temp;
          std::set_intersection(pred->m_dominators.begin(),pred->m_dominators.end(),intersection.begin(),intersection.end(),std::inserter(temp,temp.end()));
          intersection = std::move(temp);
        }

        intersection.insert(fn->m_basic_blocks[i]);
        if(intersection != fn->m_basic_blocks[i]->m_dominators){
          fn->m_basic_blocks[i]->m_dominators = intersection;
          changed=true;
        }
    }
  }
}

void IntermediateProgram::remove_all_triples_from_block(BasicBlock *blk){
  size_t start_index =blk->m_first_triple->m_index;
  auto fn = blk->m_fn;

  for(size_t j=blk->m_first_triple->m_index; j<fn->m_triples.size() && blk==fn->m_triples[j]->m_basic_block;)
    blk->m_fn->m_triples.erase(blk->m_fn->m_triples.begin()+j);

  for(size_t i=start_index; i<fn->m_triples.size();i++)
    fn->m_triples[i]->m_index=i;

}

void IntermediateProgram::enumarate(BasicBlock *blk,int &counter){
  if(blk->m_number!=-1)
    return;

  blk->m_number = counter;
  counter++;
  for(size_t i=0;i<blk->m_successors.size();i++)
    enumarate(blk->m_successors[i],counter);
}

void IntermediateProgram::enumerate_basic_blocks(Function *fn){
  int counter = 0;
  BasicBlock *blk = fn->m_basic_blocks[0];
  enumarate(blk,counter);
}

void IntermediateProgram::create_basic_blocks(Function *fn){

  size_t block_id=0;
  fn->m_basic_blocks.push_back(new BasicBlock{fn,block_id});
  BasicBlock *current_block=fn->m_basic_blocks.back();

  for(auto *triple : fn->m_triples){
    Operation op = triple->m_operation;
    if(op == Operation::JT || op == Operation::JF || op == Operation::JMP){
      if(current_block->m_first_triple==nullptr)
        current_block->m_first_triple=triple;

      triple->m_basic_block=current_block;
      block_id++;
      fn->m_basic_blocks.push_back(new BasicBlock{fn,block_id});
      current_block=fn->m_basic_blocks.back();
    }else if(op == Operation::MARKER){
      if(current_block->m_first_triple==nullptr){
        triple->m_basic_block=current_block;
        current_block->m_first_triple=triple;
      }else{
        block_id++;
        fn->m_basic_blocks.push_back(new BasicBlock{fn,block_id});
        current_block=fn->m_basic_blocks.back();
        triple->m_basic_block=current_block;
        current_block->m_first_triple=triple;
      }
    }
    else if(op == Operation::RETURN){
      if(current_block->m_first_triple==nullptr)
        current_block->m_first_triple=triple;

      triple->m_basic_block=current_block;
      block_id++;
      fn->m_basic_blocks.push_back(new BasicBlock{fn,block_id});
      current_block=fn->m_basic_blocks.back();
    }
    else{
      if(current_block->m_first_triple==nullptr)
        current_block->m_first_triple=triple;

      triple->m_basic_block=current_block;
    }
  }

  // if the last block is empty then it's removed
  if(fn->m_basic_blocks.back()->m_first_triple==nullptr){
    BasicBlock * blk = fn->m_basic_blocks.back();
    fn->m_basic_blocks.pop_back();
    delete blk;
  }

  for(auto *triple : fn->m_triples){
    Operation op = triple->m_operation;
    Triple * jump_to_triple = nullptr;
    BasicBlock* destination_block = nullptr;
    BasicBlock* source_block = nullptr;
    if(op == Operation::JT || op == Operation::JF){
      jump_to_triple = triple->m_op_2.m_label->m_jump_to;
    }else if(op == Operation::JMP){
      jump_to_triple = triple->m_op_1.m_label->m_jump_to;
    }else if(op == Operation::RETURN){
      continue;
    }
    else{
      source_block = triple->m_basic_block;
      Triple* next_triple=nullptr;
      if(triple->m_index+1<fn->m_triples.size()){
        next_triple = fn->m_triples.at(triple->m_index+1);
        destination_block = next_triple->m_basic_block;
        if(source_block!=destination_block)
          connect_blocks(source_block,destination_block);
      }

      continue;
    }

    source_block = triple->m_basic_block;
    destination_block = jump_to_triple->m_basic_block;
    connect_blocks(source_block,destination_block);

    if(op != Operation::JMP){
      Triple* next_triple = nullptr;
      if(triple->m_index+1<fn->m_triples.size()){
        next_triple = fn->m_triples.at(triple->m_index+1);
        destination_block = next_triple->m_basic_block;
        connect_blocks(source_block,destination_block);
      }
    }
  }

  for(size_t i=1;i<fn->m_basic_blocks.size();){
    auto current_blk = fn->m_basic_blocks[i];
    if(current_blk->m_predecessors.size()>0){
      i++;
      continue;
    }

    while(current_blk->m_successors.size()>0)
      disconnect_blocks(current_blk,current_blk->m_successors.back());

    remove_all_triples_from_block(fn->m_basic_blocks[i]);
    delete fn->m_basic_blocks[i];
    fn->m_basic_blocks.erase(fn->m_basic_blocks.begin()+i);
  }

  for(size_t i=1;i<fn->m_basic_blocks.size();i++){
    fn->m_basic_blocks[i]->m_index=i;
  }

}

void IntermediateProgram::print_predecessors_blk(BasicBlock *blk){
  for(auto *pred_blk : blk->m_predecessors)
    std::cout<<pred_blk->m_index<<" ";
}

void IntermediateProgram::print_dominator_sets(){
  for(auto fn : m_functions){
    if(fn->m_type!=PredefinedFunction::USERDEFINED)
      continue;

    std::cout<<"Printing dominator sets for:"<<std::endl;
    std::cout<<"Function name : "<<fn->m_name<<std::endl;
    for(auto blk : fn->m_basic_blocks){
      std::cout<<"Block number : "<<blk->m_index<<std::endl;
      std::cout<<"{";
      int i=0;
      for(auto dom : blk->m_dominators ){
        if(i>0)
          std::cout<<",";
        std::cout<<dom->m_index;
        i++;
      }
      std::cout<<"}\n";
    }
  }
}

void IntermediateProgram::print_predecessors(){
  for(auto *fn : m_functions){
    if(fn->m_type!=PredefinedFunction::USERDEFINED)
      continue;
    std::cout<<"Printing Predecessors for a function: "<<std::endl;
    std::cout<<"Function name : "<<fn->m_name<<std::endl;
    for(auto *blk : fn->m_basic_blocks){
      std::string msg = fmt::format("Block{} ",blk->m_index);
      std::cout<<msg;
      print_predecessors_blk(blk);
      std::cout<<std::endl;
    }
  }
}

void IntermediateProgram::print_blocks(){
  for(auto *fn : m_functions){
    if(fn->m_type!=PredefinedFunction::USERDEFINED)
      continue;

    for(auto * blk : fn->m_basic_blocks){
      std::string msg = fmt::format("Block {}, t{}: \n",blk->m_index,blk->m_first_triple->m_index);
      std::cout<<msg;
    }
  }
}

void IntermediateProgram::create_basic_blocks(){
  for(auto *fn : m_functions)
    create_basic_blocks(fn);

  for(auto *cl : m_classes){
    for(auto *meth : cl->m_methods)
      create_basic_blocks(meth);
  }
}

void IntermediateProgram::correct_labels(Function *fn){
  for(Triple * triple : fn->m_triples){
    if(triple->m_operation==Operation::JT || triple->m_operation==Operation::JF){
      Operand &op_2 = triple->m_op_2;
      op_2.m_label->m_jump_to = op_2.m_label->m_jump_to->m_basic_block->m_first_triple;
    }else if(triple->m_operation==Operation::JMP){
      Operand &op_1 = triple->m_op_1;
      op_1.m_label->m_jump_to = op_1.m_label->m_jump_to->m_basic_block->m_first_triple;
    }
  }
}

bool IntermediateProgram::find_path_without_return(Function *fn,size_t start){
  if(start >= fn->m_triples.size())
    return true;

  if(fn->m_triples[start]->m_visited)
    return false;

  for(size_t i=start;i<fn->m_triples.size();i++){
    fn->m_triples[start]->m_visited=true;

    if(fn->m_triples[i]->m_operation==Operation::RETURN)
      return false;

    if(fn->m_triples[i]->m_operation==Operation::JF || fn->m_triples[i]->m_operation==Operation::JT || fn->m_triples[i]->m_operation==Operation::JMP){
      bool jump_possible;
      bool no_jump_possible;

      if(fn->m_triples[i]->m_operation==Operation::JMP){
        jump_possible=true;
        no_jump_possible=false;
      }else{
        switch (fn->m_triples[i]->m_op_1.m_category)
        {
        case OperandCategory::CONSTANT:
        {
          jump_possible = (fn->m_triples[i]->m_op_1.m_constant.u.boolean == false && fn->m_triples[i]->m_operation==Operation::JF)
          || (fn->m_triples[i]->m_op_1.m_constant.u.boolean && fn->m_triples[i]->m_operation==Operation::JT);
          no_jump_possible = !jump_possible;
          break;
        }
        case OperandCategory::VARIABLE:
        case OperandCategory::ARGUMENT:
        case OperandCategory::TRIPLE:
        {
          jump_possible=true;
          no_jump_possible=true;
          break;
        }
        default:
          break;
        }
      }

      if(jump_possible){
        size_t index;
        if(fn->m_triples[i]->m_operation==Operation::JMP)
          index = fn->m_triples[i]->m_op_1.m_label->m_jump_to->m_index;
        else
          index = fn->m_triples[i]->m_op_2.m_label->m_jump_to->m_index;

        bool result = find_path_without_return(fn,index);
        if(result)
          return true;
      }

      if(no_jump_possible)
        continue;
      else
        return false;
    }
  }

  return true;
}

IntermediateProgram::~IntermediateProgram(){
  for(auto *fn: m_functions)
    delete fn;
}
