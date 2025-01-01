
#ifndef LLVM_CODE_GENERATOR_H
#define LLVM_CODE_GENERATOR_H

#include <vector>
#include <string>
#include "DataStructure.h"
#include <map>
#include "IRCoder.h"

class VariableData{
public:
    size_t m_index;
    VariableData(size_t index):m_index{index}{}
    VariableData():m_index{0}{}
};

class ArgumentData{
public:
    size_t m_index;
    ArgumentData(size_t index):m_index{index}{}
    ArgumentData():m_index{0}{}
};

class TripleData{
public:
    // weird number  
    size_t m_number=123456; // line number in llvm
    size_t m_marker_index;
};

class LLVMCodeGenerator{
private:
    bool scanf_added=false;
    bool printf_added=false;
    size_t m_last_marker_number;
    // to generate labels we'll naming convetion Label{number} where number 0 - n
    IntermediateProgram *m_intermediate_program;
    size_t m_llvm_line_index;
    std::vector<std::string> m_code_lines;
    std::vector<std::string> m_code_lines_after;
    std::map<Variable*,VariableData> m_variable_data;
    std::map<Argument*,ArgumentData> m_argument_data;
    std::vector<TripleData> m_triple_data;
    std::vector<std::string> m_string_literals;
    Function * m_current_fn = nullptr;
    std::map<std::string,int> m_string_literal_to_index;
public:
    void process_triple(Triple * triple);
    void process_function(Function *fn);
    void alloc_variable(Variable *variable);
    void increase(){m_llvm_line_index++;}
    void print_generated_code();
    void collect_string_literals();
    void map_string_literals();
    void generate_string_literal_declarations();
    void alloc_arguments_and_variables(const Function *fn);
    void store_arguments(const Function *fn);
    void add_used_predefined_functions();
    void enumerate_all_markers(Function *fn);
    std::string make_alloca_string(DataType type);
    std::string make_store_string(DataType type,size_t from,size_t to);
    std::string get_align(DataType type);
    std::string get_size_in_bits(DataType type);
    std::string get_data_type_name(DataType type);
    std::string get_operand_value(const Operand &op);
    std::string get_operand_value_with_load(const Operand &op);
    std::string ir_op_to_llvm_op(Operation op); // intermediate operation to llvm operation
    std::string process_program();
    std::string process_argument(Argument * arg,size_t index);
    std::string process_argument_list(Triple *triple);
    std::string get_string_literal_name(int index);
    void initialize_variables(const Function *fn);
    LLVMCodeGenerator(IntermediateProgram *i_p):m_intermediate_program{i_p}{}
    // add template 
    // finish processing return statement 
    // Generate code for printf() SS
};
#endif 