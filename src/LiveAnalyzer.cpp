#include "LiveAnalyzer.h"

std::vector<Operand> LiveAnalyzer::collect_args_and_vars(Triple *triple){
  int operand_count = triple->get_operand_count();

  std::vector<Operand> op_refs;
  if(operand_count==1){
    OperandCategory op1_cat = triple->m_op_1.m_category;
    if(op1_cat==OperandCategory::ARGUMENT || op1_cat==OperandCategory::VARIABLE){
      op_refs.push_back(triple->m_op_1);
    }
  }else if(operand_count==2){
    OperandCategory op1_cat = triple->m_op_1.m_category;
    OperandCategory op2_cat = triple->m_op_2.m_category;
    if(op1_cat==OperandCategory::ARGUMENT || op1_cat==OperandCategory::VARIABLE){
      op_refs.push_back(triple->m_op_1);
    }

    if(op2_cat==OperandCategory::ARGUMENT || op2_cat==OperandCategory::VARIABLE){
      op_refs.push_back(triple->m_op_2);
    }
  }

  if(triple->m_operation==Operation::CALL){
    for(auto arg : triple->m_call_args){
      if(arg.m_category==OperandCategory::ARGUMENT || arg.m_category==OperandCategory::VARIABLE)
        op_refs.push_back(arg);
    }
  }

  return op_refs;
}


void LiveAnalyzer::init(BasicBlock *blk){

  std::set<ArgOrVar> already_assigned;

  for(auto triple : *blk){

    Operation operation = triple->m_operation;
    Operand &op_1 = triple->m_op_1;
    Operand &op_2 = triple->m_op_2;
    if(operation==Operation::INIT || operation==Operation::ASSIGN){
      if(op_1.is_arg_or_var()){
        blk->m_def.insert(op_1.get_arg_or_var());
        already_assigned.insert(op_1.get_arg_or_var());
      }

      if(op_2.is_arg_or_var()){
        if(already_assigned.count(op_2.get_arg_or_var())==0){
          blk->m_use.insert(op_2.get_arg_or_var());
        }
      }
    }else if(operation==Operation::INC || operation==Operation::DEC){
      if(op_1.is_arg_or_var()){
        if(already_assigned.count(op_1.get_arg_or_var())==0){
          blk->m_def.insert(op_1.get_arg_or_var());
          blk->m_use.insert(op_1.get_arg_or_var());
        }
      }
    }else{

      auto op_refs = collect_args_and_vars(triple);
      for(auto &item : op_refs){
        if(already_assigned.count(item.get_arg_or_var())==0){
          blk->m_use.insert(item.get_arg_or_var());
        }
      }
    }
  }

}

void LiveAnalyzer::analyze(Function *fn){
  m_current_fn = fn;

  for(auto blk: fn->m_basic_blocks){
    init(blk);
  }

  bool changed=true;
  while(changed){
    changed=false;
    for(auto blk : fn->m_basic_blocks){
      auto out_prim = blk->m_live_out;
      auto in_prim = blk->m_live_in;

      blk->m_live_out.clear();

      for(auto succ : blk->m_successors){
        blk->m_live_out.insert(succ->m_live_in.begin(),succ->m_live_in.end());
      }

      std::set<ArgOrVar> diff;
      for(auto out_elem : blk->m_live_out){
        if(blk->m_def.count(out_elem)==0){
          diff.insert(out_elem);
        }
      }

      blk->m_live_in=blk->m_use;
      for(auto inter_elem : diff){
        blk->m_live_in.insert(inter_elem);
      }

      if(blk->m_live_in!=in_prim || blk->m_live_out!=out_prim)
        changed=true;
    }
  }

}

void LiveAnalyzer::run(){
  for(auto *fn : m_int_program->m_functions){
    if(fn->m_type!=PredefinedFunction::USERDEFINED)
      continue;

    analyze(fn);
  }

  for(auto cl : m_int_program->m_classes){
    for(auto meth : cl->m_methods){
      analyze(meth);
    }
  }
}

void LiveAnalyzer::print_use_def(BasicBlock * blk){

  std::cout<<"Block: "<<blk->m_index<<std::endl;
  std::cout<<"Printing def: (";
  for(auto &item : blk->m_def){
    std::cout<<item<<" ";
  }
  std::cout<<")";
  std::cout<<std::endl;

  std::cout<<"Printing use: (";
  for(auto &item : blk->m_use){
    std::cout<<item<<" ";
  }
  std::cout<<")";
  std::cout<<std::endl;

}

void LiveAnalyzer::print_live_variables(Function *fn){
  std::cout<<"Printing live variables for "<<fn->m_name<<std::endl;
  for(auto *blk : fn->m_basic_blocks){
    std::cout<<"Block"<<blk->m_index<<std::endl;
    std::cout<<"Printing IN["<<blk->m_index<<"] ( ";
    for(auto in : blk->m_live_in){
      std::cout<<in<<" ";
    }
    std::cout<<")";
    std::cout<<std::endl;

    std::cout<<"Printing OUT["<<blk->m_index<<"] ( ";
    for(auto out : blk->m_live_out){
      std::cout<<out<<" ";
    }
    std::cout<<")";
    std::cout<<std::endl;

  }
}

void LiveAnalyzer::print_live_variables(){
  for(auto *fn : m_int_program->m_functions){
    if(fn->m_type==PredefinedFunction::USERDEFINED)
      print_live_variables(fn);
  }
}
