
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
    size_t m_number=123456; // line number in llvm
    size_t m_marker_index;
};

class FunctionData{
public:
    
};

class LLVMCodeGenerator{
private:
    int COUNTER_SIZE=4;
    int MODIFIABLE_FLAG=1;
    int END_OF_STRING=1;
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
    void increase(){m_llvm_line_index++;}
    void print_generated_code();
    void collect_strings_from_function(Function * fn);
    void collect_string_literals();
    void map_string_literals();
    void generate_string_literal_declarations();
    void alloc_arguments_and_variables(const Function *fn);
    void store_arguments(const Function *fn);
    void add_used_predefined_functions();
    void add_classes_declarations();
    void enumerate_all_markers(Function *fn);
    void add_virtual_tables();
    int calculate_instance_size(MyClass *cl);
    int get_string_length(const std::string str);
    std::string generate_llvm_function_to_vtable(Function *fn);
    std::string generate_llvm_vtable(MyClass * cl);
    std::string get_all_data_types(MyClass *cl);
    std::string make_alloca_string(DataType type);
    std::string make_store_string(Argument *arg,int from,int to);
    std::string get_align(DataType type);
    std::string get_data_type_name(DataType type,bool for_storage=false);
    std::string get_operand_value(const Operand &op);
    int get_storage_size(DataType type);
    std::string get_operand_value_with_load(const Operand &op);
    std::string ir_op_to_llvm_op(Operation op); // intermediate operation to llvm operation
    std::string process_program();
    std::string process_argument(Argument * arg,size_t index);
    std::string process_argument_list(Triple *triple);
    std::string get_string_literal_name(int index);
    void initialize_variables(const Function *fn);
    LLVMCodeGenerator(IntermediateProgram *i_p):m_intermediate_program{i_p}{}
};

std::string struct_decl =R"abc(
%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %struct._IO_marker*, %struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, %struct._IO_codecvt*, %struct._IO_wide_data*, %struct._IO_FILE*, i8*, i64, i32, [20 x i8] }
%struct._IO_marker = type opaque
%struct._IO_codecvt = type opaque
%struct._IO_wide_data = type opaque)abc";

#endif 