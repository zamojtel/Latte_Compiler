#include "SSARenamer.h"

int SSARenamer::new_name(const ArgOrVar &op){
  int i = m_counter.at(op);
  m_counter.at(op)++;
  m_versioning_stack.at(op).push_back(i);
  return i;
}

void SSARenamer::rewrite(Operand &op){
    if(op.m_category==OperandCategory::ARGUMENT || op.m_category==OperandCategory::VARIABLE){
        op.m_version = m_versioning_stack.at(get_arg_or_var(op)).back();
    }
}

void SSARenamer::print_var_to_triple(){
    for(auto &[key,val] : m_var_arg_to_triple){
        std::cout<<"Operand "<<key<<" TRIPLE INDEX: "<<val<<std::endl;
    }
}

void SSARenamer::assign_phi_to_variable(Variable *var){

}

void SSARenamer::insert_phi(){
    IRCoder ir_coder;
    // zmieniamy first_triple danego bloku
    for(auto blk: m_current_fn->m_basic_blocks){
        if(blk->m_variable_has_phi.size()>0){
            ir_coder.set_position_before(m_current_fn,blk->m_first_triple);
            for(auto var : blk->m_variable_has_phi)
            {
                if(blk->m_live_in.count(var.get_arg_or_var())>0){
                    ir_coder.push(0,Operation::PHI,var);
                }
            }
        }
    }
}

ArgOrVar SSARenamer::get_arg_or_var(const Operand &op){
    switch (op.m_category)
    {
    case OperandCategory::VARIABLE:
        return op.m_var;
    case OperandCategory::ARGUMENT:
        return op.m_argument;
    default:
        throw std::runtime_error("WRONG OPERAND get_arg_or_var(const Operand &op)");
    }
}

void SSARenamer::rename(BasicBlock* blk){

    for(auto triple : *blk){
        if(triple->m_operation==Operation::PHI){
            int new_version = new_name(get_arg_or_var(triple->m_op_1));
            triple->m_op_1.m_version = new_version;
            m_var_arg_to_triple[triple->m_op_1]=triple;
        }
    }

    for(auto triple : *blk){
        Operation operation = triple->m_operation;
        Operand &op_1 = triple->m_op_1;
        Operand &op_2 = triple->m_op_2;

        if (operation==Operation::ASSIGN || operation == Operation::INIT){
            if(op_1.m_category==OperandCategory::ARGUMENT || op_1.m_category==OperandCategory::VARIABLE){
                int new_version = new_name(get_arg_or_var(triple->m_op_1));
                op_1.m_version = new_version;

                m_var_arg_to_triple[triple->m_op_1]=triple->m_op_2;
            }
            rewrite(op_2);
        }else{
            // a smarter way
            rewrite(op_1);
            rewrite(op_2);
            if(operation==Operation::CALL){
                for(auto &op : triple->m_call_args){
                    rewrite(op);
                }
            }
        }
    }

    for(auto successor : blk->m_successors){
        for(Triple* triple : *successor){
            if(triple->m_operation==Operation::PHI){
                Operand op = triple->m_op_1;
                ArgOrVar arg_var = get_arg_or_var(triple->m_op_1);
                if(m_versioning_stack.at(arg_var).size()>0){
                    op.m_version=m_versioning_stack.at(arg_var).back();
                    triple->m_phi_arguments.push_back({blk,op});
                }
            }
        }
    }

    for(auto successor : blk->m_dominator_tree_successors)
        rename(successor);

    for(auto triple : *blk){
        if(triple->m_operation==Operation::ASSIGN || triple->m_operation==Operation::INIT){
            Operand &op_1 = triple->m_op_1;
            if(op_1.m_category==OperandCategory::ARGUMENT || op_1.m_category==OperandCategory::VARIABLE){
                m_versioning_stack.at(get_arg_or_var(op_1)).pop_back();
            }
        }
        else if(triple->m_operation==Operation::PHI){
            m_versioning_stack.at(get_arg_or_var(triple->m_op_1)).pop_back();
        }
    }
}

void SSARenamer::rename_ssa_function(Function* fn){
    m_current_fn=fn;
    if(fn->m_type!=PredefinedFunction::USERDEFINED)
        return;

    insert_phi();
    fn->m_int_program->correct_labels(fn);

    for(auto var : fn->m_variables){
        m_counter[var]=0;
        m_versioning_stack[var];
    }

    if(fn->m_name=="f"){
        int x =123;
    }
    for(auto arg : fn->m_arguments){
        m_counter[arg]=0;
        m_versioning_stack[arg];
        // add versions for arguments
        Operand op{arg};
        // OBEJRZEC: 103 dorzucanie argumentu do mapy
        int new_version = new_name(get_arg_or_var(op));
        op.m_version=new_version;
        m_var_arg_to_triple[arg]=op;
    }

    rename(fn->m_basic_blocks[0]);

    // for(auto blk : m_current_fn->m_basic_blocks){
    //     for(auto triple : *blk){
    //         Operand &op_1 = triple->m_op_1;
    //         Operand &op_2 = triple->m_op_2;
    //         Operation operation = triple->m_operation;
    //         if(operation != Operation::ASSIGN && operation!= Operation::INIT){
    //             if(op_1.m_category==OperandCategory::ARGUMENT || op_1.m_category==OperandCategory::VARIABLE){
    //                 op_1=m_var_arg_to_triple.at(op_1);
    //             }

    //             if(op_2.m_category==OperandCategory::ARGUMENT || op_2.m_category==OperandCategory::VARIABLE){
    //                 op_2=m_var_arg_to_triple.at(op_2);
    //             }
    //         }

    //         if(operation==Operation::CALL){
    //             for(auto &op : triple->m_call_args){
    //                 if(op.m_category==OperandCategory::ARGUMENT || op.m_category==OperandCategory::VARIABLE){
    //                     op=m_var_arg_to_triple.at(op);
    //                 }
    //             }
    //         }
    //     }
    // }

    // print_var_to_triple();
    fn->m_var_arg_to_triple=std::move(m_var_arg_to_triple);
}

void SSARenamer::copy_propagation(){
    std::set<Operand> worklist;


}
