#ifndef INTERMEDIATE_PROGRAM_PRINTER_STRUCTURE_H
#define INTERMEDIATE_PROGRAM_PRINTER_STRUCTURE_H

class Triple;
class Function;
class BasicBlock;
class IntermediateProgram;

class IntermediateProgramPrinter{
private:
    IntermediateProgram m_ip;
public:
  IntermediateProgramPrinter(IntermediateProgram &ip);
  std::string operation_to_string(Operation operation);
  void print_arguments(Triple *triple);
  void print_triples(Function * fn);
  void print_function(Function *fn);
  void print_program(IntermediateProgram &ip);
};

#endif
