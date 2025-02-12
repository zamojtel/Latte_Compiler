#ifndef SSA_RENAMER_H
#define SSA_RENAMER_H

#include <map>
#include <vector>
#include <set>
#include "DataStructure.h"
#include "IntermediateProgram.h"
#include "IRCoder.h"

class Operand;
class BasicBlock;
class Function;

class SSARenamer{
public:
    Function *m_current_fn;
    std::map<Operand,int> m_counter;
    std::map<Operand,std::vector<int>> m_versioning_stack;
    // Versioning
    void insert_phi();
    void rewrite(Operand &op);
    int new_name(const Operand &op);
    void rename(BasicBlock* blk);
    void rename_ssa_function(Function* fn);
    void assign_phi_to_variable(Variable *var);
};
#endif
