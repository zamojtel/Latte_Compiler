#include "LiveAnalyzer.h"

// wrzucic do jednego pliku
std::vector<Operand> LiveAnalyzer::collect_args_and_vars(Triple *triple){
  int operand_count;
  switch (triple->m_operation)
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
  case Operation::PHI:
    operand_count=0;
    break;
  // default:
  //   break;
  }
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
// for collecting uses and defs
void LiveAnalyzer::init(BasicBlock *blk){

  std::set<ArgOrVar> already_assigned;

  for(auto triple : *blk){
    // if(triple->m)
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
  // std::cout<<"Function: "<<fn->m_name<<std::endl;
  // for(auto blk: fn->m_basic_blocks){
  //   print_use_def(blk);
  // }
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

      // std::set_difference();
      std::set<ArgOrVar> diff;
      for(auto out_elem : blk->m_live_out){
        if(blk->m_def.count(out_elem)==0){
          diff.insert(out_elem);
        }
      }
      // user(blk)
      blk->m_live_in=blk->m_use;
      for(auto inter_elem : diff){
        blk->m_live_in.insert(inter_elem);
      }

      if(blk->m_live_in!=in_prim || blk->m_live_out!=out_prim)
        changed=true;
    }
  }


  // print_live_variables(fn);
  // for(auto *blk : fn->m_basic_blocks){
  //   blk->m_live_in.clear();
  //   blk->m_live_out.clear();
  // }

  // m_UEVar.clear();
  // m_UEKill.clear();
  // m_AllVariablesBlk.clear();

  // for(auto blk : m_current_fn->m_basic_blocks)
  //   init(blk);

  // for(auto *blk : fn->m_basic_blocks){
  //   LiveOut[blk];
  //   for(auto triple : *blk){
  //     auto res = collect_args_and_vars(triple);
  //     m_AllVariablesBlk[blk].insert(res.begin(),res.end());
  //   }
  // }

  // bool changed = true;
  // while(changed){
  //   changed=false;
  //   for(auto *blk : fn->m_basic_blocks){
  //       for(auto m : blk->m_successors){
  //         std::set<Operand> temp;
  //         std::set_difference(m_AllVariablesBlk.at(m).begin(),m_AllVariablesBlk.at(m).end(),m_UEKill.at(m).begin(),m_UEKill.at(m).end(),std::inserter(temp,temp.end()));
  //         auto varKillComplement = std::move(temp);
  //         std::set<Operand> temp_2;
  //         std::set_intersection(LiveOut.at(m).begin(),LiveOut.at(m).end(),varKillComplement.begin(),varKillComplement.end(),std::inserter(temp_2,temp_2.end()));
  //         auto liveInterVarKillComplement = std::move(temp_2);
  //         // m_UEVar
  //         std::set<Operand> temp_3;
  //         std::set_union(m_UEVar.at(m).begin(),m_UEVar.at(m).end(),liveInterVarKillComplement.begin(),liveInterVarKillComplement.end(),std::inserter(temp_3,temp_3.end()));
  //         auto live_out_blk = std::move(temp_3);
  //         auto current_blk = LiveOut.at(blk);
  //         std::set<Operand> new_blk_live;
  //         std::set_union(LiveOut.at(blk).begin(),LiveOut.at(blk).end(),live_out_blk.begin(),live_out_blk.end(),std::inserter(new_blk_live,new_blk_live.end()));
  //         if(current_blk!=new_blk_live){
  //           LiveOut.at(blk).insert(new_blk_live.begin(),new_blk_live.end());
  //           changed=true;
  //         }
  //       }
  //   }
  // }
  // print_live_variables(fn);
}

void LiveAnalyzer::run(){
    for(auto *fn : m_int_program->m_functions){
      if(fn->m_type!=PredefinedFunction::USERDEFINED)
        continue;

      analyze(fn);
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
    std::cout<<"Printing IN["<<blk->m_index<<"](";
    for(auto in : blk->m_live_in){
      std::cout<<in<<" ";
    }
    std::cout<<")";
    std::cout<<std::endl;

    std::cout<<"Printing OUT["<<blk->m_index<<"](";
    for(auto out : blk->m_live_out){
      std::cout<<out<<" ";
    }
    std::cout<<")";
    std::cout<<std::endl;

  }
}
