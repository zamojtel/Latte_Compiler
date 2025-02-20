#ifndef SSA_RENAMER_H
#define SSA_RENAMER_H

#include <map>
#include <vector>
#include <set>
#include "DataStructure.h"
#include "IntermediateProgram.h"
#include "IRCoder.h"
#include <variant>

class Operand;
class BasicBlock;
class Function;

class SSARenamer{
public:
    std::map<Operand,Operand> m_var_arg_to_triple;
    Function *m_current_fn;
    std::map<ArgOrVar,int> m_counter;
    std::map<ArgOrVar,std::vector<int>> m_versioning_stack;
    ArgOrVar get_arg_or_var(const Operand &op);
    int new_name(const ArgOrVar &op);
    void insert_phi();
    void rewrite(Operand &op);
    void rename(BasicBlock* blk);
    void rename_ssa_function(Function* fn);
    void print_var_to_triple();
    void substitute_vars_args();
};
#endif
