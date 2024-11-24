
#include "LLVMCodeGenerator.h"
#include <iostream>

void LLVMCodeGenerator::alloc_variable(Variable *variable){
    std::string code;

    switch (variable->m_type)
    {
    case DataType::INT:{
        m_code_lines.push_back(std::format("%{} = alloca i32, align 4",m_llvm_line_index));
        m_variable_data[variable]=VariableData{m_llvm_line_index};
        increase();
        return;
    }
    case DataType::BOOL:{
        m_code_lines.push_back(std::format("%{} = alloca i8, align 1",m_llvm_line_index));
        m_variable_data[variable]=VariableData{m_llvm_line_index};
        increase();
        return;
    }
    default:
        break;
    }
}

std::string LLVMCodeGenerator::get_size_in_bits(DataType type){
    switch (type)
    {
    case DataType::BOOL:
        return "8";
    case DataType::INT:
        return "32";
    default:
        return "error";
    }
}

std::string LLVMCodeGenerator::get_align(DataType type){
    switch (type)
    {
    case DataType::BOOL:
        return "1";
    case DataType::INT:
        return "4";
    default:
        return "error";
    }
}

std::string LLVMCodeGenerator::get_operand_value(const Operand &op){
    switch (op.m_category)
    {
    case OperandCategory::CONSTANT:
        return op.m_constant.get_value_as_string();
    case OperandCategory::VARIABLE:
        return std::to_string(m_variable_data[op.m_var].m_index);
    case OperandCategory::TRIPLE:
        return std::to_string(m_triple_data[op.m_triple->m_index].m_number);
    default:
        return "error";
    }
}

std::string LLVMCodeGenerator::get_operand_value_with_load(const Operand &op){
    switch (op.m_category)
    {
    case OperandCategory::CONSTANT:
        return op.m_constant.get_value_as_string();
    case OperandCategory::VARIABLE:{
        DataType type = op.get_type();
        std::string line = std::format("%{} = load i{}, ptr %{}, align {}",m_llvm_line_index,get_size_in_bits(type),m_variable_data[op.m_var].m_index,get_align(type));
        size_t loaded_variable_index = m_llvm_line_index;
        increase();
        m_code_lines.push_back(line);
        return "%"+std::to_string(loaded_variable_index);
    }
    case OperandCategory::TRIPLE:
        return "%"+std::to_string(m_triple_data[op.m_triple->m_index].m_number);
    default:
        return "error";
    }
}

std::string LLVMCodeGenerator::ir_op_to_llvm_op(Operation op){
    switch (op)
    {
    case Operation::ADD:
        return "add";
    case Operation::SUB:
        return "sub";
    case Operation::DIV:
        return "sdiv";
    case Operation::MUL:
        return "mul";
    case Operation::MOD:
        return "srem";
    default:
        return "error";
    }
}

void LLVMCodeGenerator::process_triple(Triple * triple){
    
    switch (triple->m_operation)
    {
    case Operation::ASSIGN:{
        DataType type_op_1 = triple->m_op_1.get_type();
        triple->m_op_2.get_type();
        triple->m_op_2.m_constant.get_value_as_string();
        m_code_lines.push_back(std::format("store i{} {}, ptr %{}, align {}",get_size_in_bits(type_op_1),get_operand_value(triple->m_op_2),m_variable_data[triple->m_op_1.m_var].m_index,get_align(type_op_1)));
        break;
    }
    case Operation::ADD:
    case Operation::DIV:
    case Operation::SUB:
    case Operation::MUL:{
        std::string op_1_val = get_operand_value_with_load(triple->m_op_1);
        std::string op_2_val = get_operand_value_with_load(triple->m_op_2);
        m_triple_data[triple->m_index] = {m_llvm_line_index};
        std::string operation = ir_op_to_llvm_op(triple->m_operation);
        std::string line = std::format("%{} = {} nsw i{} {}, {}",m_llvm_line_index,operation,get_size_in_bits(triple->m_op_1.get_type()),op_1_val,op_2_val);
        m_code_lines.push_back(line);
        increase();
        break;
    }
    case Operation::RETURN:{
        std::string ref = get_operand_value_with_load(triple->m_op_1);
        m_code_lines.push_back(std::format("ret i{} {}",get_size_in_bits(triple->m_op_1.get_type()),ref));
        break;
    }
    case Operation::CALL:
    {
        switch (m_current_fn->m_type)
        {
            // PRINTINT,
            // PRINTSTRING,
            // ERROR,
            // READINT,
            // READSTRING,
            // USERDEFINED
        case PredefinedFunction::PRINTINT:
        {
            // std::string printInt_str = "declare i32 @printInt(ptr noundef, ...) #1"
            // %25 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %24)
        }
        default:
            break;
        }
    } 
    default:
        break;
    }
}

std::string LLVMCodeGenerator::process_argument(Argument * arg,size_t index){
    // i32 noundef %0, double noundef %1, i32 noundef %2, ptr noundef %3
    // INT,BOOL,STRING,VOID,ERROR
    std::string processed_arg = "";
    switch (arg->m_type)
    {
    case DataType::INT:
        processed_arg+= get_size_in_bits(arg->m_type);
        break;   
    case DataType::BOOL:
        processed_arg+= get_size_in_bits(arg->m_type);
        break;
    case DataType::STRING:
        processed_arg+= get_size_in_bits(arg->m_type);
        break;
    case DataType::VOID: 
    default:
        throw 0;
    }

    processed_arg+=" noundef"+index;
    return processed_arg;
}

void LLVMCodeGenerator::process_function(Function *fn){
    
    m_current_fn = fn;
    m_triple_data.resize(fn->m_triples.size());
    m_llvm_line_index=0;
    std::string argument_list;

    for(size_t i=0;i<fn->m_arguments.size();i++){
        if(i>0)
            argument_list+=", ";
        argument_list+=process_argument(fn->m_arguments[i],i);
    }
    m_code_lines.push_back(std::format("define dso_local noundef i{} @{}({}) #0 {{",get_size_in_bits(fn->m_return_type),fn->m_name,argument_list));

    for(size_t i=0;i<fn->m_variables.size();i++){
        alloc_variable(fn->m_variables[i]);
    }

    for(auto triple : fn->m_triples)
        process_triple(triple);

}

std::string LLVMCodeGenerator::process_program(const std::vector<Function*> &functions){
    
    for(size_t i=0;i<functions.size();i++){
        process_function(functions[i]);
    }

    for(size_t j=0;j<functions.size();j++){
        if(functions[j]->m_used){
            switch (functions[j]->m_type)
            {
            case PredefinedFunction::PRINTINT:
            case PredefinedFunction::PRINTSTRING:
                break;
            case PredefinedFunction::READINT:
            case PredefinedFunction::READSTRING:
                break;
            default:
                break;
            }
        }
    }
    
    return "";
}


void LLVMCodeGenerator::print_generated_code() {
    for(auto line:m_code_lines){
        std::cout<<line<<std::endl;
    }
}