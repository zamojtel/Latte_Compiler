
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

MyClass* IntermediateProgram::get_class(const std::string &name) const{
  for(size_t i=0;i<m_classes.size();i++){
    if(m_classes[i]->m_name==name)
      return m_classes[i];
  }
  
  return nullptr;
}

// this function checks if we can reach the end of the function without finding any return on the way
bool IntermediateProgram::check_all_returns(Function *fn,size_t start){
  if(start >= fn->m_triples.size() || fn->m_triples[start]->m_visited){
    return false;
  }

  fn->m_triples[start]->m_visited=true;

  for(size_t i=start;i<fn->m_triples.size();i++){
    if(fn->m_triples[i]->m_operation==Operation::RETURN){
      return true;
    }

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

bool IntermediateProgram::find_path_without_return(Function *fn,size_t start){
  if(start >= fn->m_triples.size()){
    return true;
  }

  if(fn->m_triples[start]->m_visited){
    return false;
  }
  
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
        if(result){
          return true;
        }
      }

      if(no_jump_possible){
        continue;
      }else{
        return false;
      }
    }
  }

  return true;
}