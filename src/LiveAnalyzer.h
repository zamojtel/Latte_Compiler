#ifndef LIVE_ANALYZER_H
#define LIVE_ANALYZER_H

#include <set>
#include <vector>
#include <variant>
#include "DataStructure.h"
#include "IntermediateProgram.h"

class Operand;
class BasicBlock;
class Function;

class LiveAnalyzer{
private:
  // std::map<BasicBlock*,std::set<Operand>> m_UEVar;
  // std::map<BasicBlock*,std::set<Operand>> m_AllVariablesBlk;
  // std::map<BasicBlock*,std::set<Operand>> m_UEKill;
  // std::map<BasicBlock*,std::set<Operand>> LiveOut;
  Function *m_current_fn=nullptr;
  IntermediateProgram *m_int_program=nullptr;
public:
  void init(BasicBlock *blk);
  void analyze(Function *fn);
  void run();
  void print_live_variables(Function *fn);
  void print_use_def(BasicBlock * blk);
  std::vector<Operand> collect_args_and_vars(Triple *triple);
  LiveAnalyzer(IntermediateProgram *ip):m_int_program{ip}{}
};
#endif
