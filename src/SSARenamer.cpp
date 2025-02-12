#include "SSARenamer.h"

int SSARenamer::new_name(const Operand &op){
  int i = m_counter.at(op);
  m_counter.at(op)++;
  m_versioning_stack.at(op).push_back(i);
  return i;
}

void SSARenamer::rewrite(Operand &op){
    if(op.m_category==OperandCategory::ARGUMENT || op.m_category==OperandCategory::VARIABLE){
        op.m_version = m_versioning_stack.at(op).back();
    }
}

void SSARenamer::assign_phi_to_variable(Variable *var){
    // for(auto ){

    // }
}

void SSARenamer::insert_phi(){
    IRCoder ir_coder;
    // zmieniamy first_triple danego bloku
    for(auto blk: m_current_fn->m_basic_blocks){
        if(blk->m_variable_has_phi.size()>0){
            ir_coder.set_position_before(m_current_fn,blk->m_first_triple);
            for(auto var : blk->m_variable_has_phi)
                ir_coder.push(0,Operation::PHI,var);
        }
    }
}

void SSARenamer::rename(BasicBlock* blk){

    for(auto triple : *blk){
        if(triple->m_operation==Operation::PHI){
            int new_version = new_name(triple->m_op_1);
            triple->m_op_1.m_version = new_version;
        }
    }

    for(auto triple : *blk){
        Operation operation = triple->m_operation;
        Operand &op_1 = triple->m_op_1;
        Operand &op_2 = triple->m_op_2;

        if (operation==Operation::ASSIGN || operation == Operation::INIT){
            if(op_1.m_category==OperandCategory::ARGUMENT || op_1.m_category==OperandCategory::VARIABLE){
                int new_version = new_name(triple->m_op_1);
                op_1.m_version = new_version;
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
                if(m_versioning_stack.at(triple->m_op_1).size()>0){
                    op.m_version=m_versioning_stack.at(triple->m_op_1).back();
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
                m_versioning_stack.at(op_1).pop_back();
            }
        }
        else if(triple->m_operation==Operation::PHI){
            m_versioning_stack.at(triple->m_op_1).pop_back();
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

    for(auto arg : fn->m_arguments){
        m_counter[arg]=0;
        m_versioning_stack[arg];
        // add versions for arguments
        Operand op{arg};
        new_name(op);
    }

    rename(fn->m_basic_blocks[0]);

}
