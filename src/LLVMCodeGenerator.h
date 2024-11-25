
#ifndef LLVM_CODE_GENERATOR_H
#define LLVM_CODE_GENERATOR_H

#include <vector>
#include <string>
#include "DataStructure.h"
#include <map>


class VariableData{
public:
    size_t m_index;
    VariableData(size_t index):m_index{index}{}
    VariableData():m_index{0}{}
};

class TripleData{
public:
    size_t m_number;
};

class FunctionData{
public:
    
};

class LLVMCodeGenerator{
private:
    size_t m_llvm_line_index;
    std::vector<std::string> m_code_lines;
    std::vector<std::string> m_code_lines_after;
    std::map<Variable*,VariableData> m_variable_data;
    std::vector<TripleData> m_triple_data;
    Function * m_current_fn = nullptr;
public:
    void process_triple(Triple * triple);
    void process_function(Function *fn);
    void alloc_variable(Variable *variable);
    void increase(){m_llvm_line_index++;}
    void print_generated_code();
    std::string get_align(DataType type);
    std::string get_size_in_bits(DataType type);
    std::string get_operand_value(const Operand &op);
    std::string get_operand_value_with_load(const Operand &op);
    std::string ir_op_to_llvm_op(Operation op); // intermediate operation to llvm operation
    std::string process_program(const std::vector<Function*> &functions);
    std::string process_argument(Argument * arg,size_t index);
    // add template 
    // finish processing return statement 
    // Generate code for printf() SS
};

#endif 

