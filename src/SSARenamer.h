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
    // using ArgOrVar = std::variant<Argument*,Variable*>;
    std::map<Operand,Operand> m_var_arg_to_triple;
    Function *m_current_fn;
    std::map<ArgOrVar,int> m_counter;
    std::map<ArgOrVar,std::vector<int>> m_versioning_stack;
    // Versioning
    void insert_phi();
    ArgOrVar get_arg_or_var(const Operand &op);
    void rewrite(Operand &op);
    int new_name(const ArgOrVar &op);
    // int new_name(const Operand &op);
    void rename(BasicBlock* blk);
    void rename_ssa_function(Function* fn);
    void assign_phi_to_variable(Variable *var);
    void print_var_to_triple();
    void substitute_vars_args();
    void copy_propagation();
};
#endif
