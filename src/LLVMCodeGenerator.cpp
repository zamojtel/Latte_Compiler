#include "Libraries.h"
#include "LLVMCodeGenerator.h"
#include <iostream>

void LLVMCodeGenerator::alloc_variable(Variable *variable){
    std::string code;

    switch (variable->m_type)
    {
    case DataType::INT:{
        m_code_lines.push_back(fmt::format("%{} = alloca i32, align 4",m_llvm_line_index));
        m_variable_data[variable]=VariableData{m_llvm_line_index};
        increase();
        return;
    }
    case DataType::BOOL:{
        m_code_lines.push_back(fmt::format("%{} = alloca i8, align 1",m_llvm_line_index));
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
        return "1";
    case DataType::INT:
        return "32";
    case DataType::STRING:
        return "";
    default:
        return "error";
    }
}
//  %7 = zext i1 %6 to i8
//   store i8 %7, i8* %3, align 1
std::string LLVMCodeGenerator::get_data_type_name(DataType type){
    switch (type)
    {
    case DataType::BOOL:
        // moze przywrocic
        return "i1";
        // return "i8";
    case DataType::INT:
        return "i32";
    case DataType::STRING:
        // moze byc zle  
        // return "ptr";
        return "i8*";
    case DataType::VOID:
        return "void";
    default:
        return "error unknown size";
    }
}

std::string LLVMCodeGenerator::get_align(DataType type){
    switch (type)
    {
    case DataType::BOOL:
        return "1";
    case DataType::INT:
        return "4";
    case DataType::STRING:
        return "8";
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
        return "%"+std::to_string(m_variable_data[op.m_var].m_index);
    case OperandCategory::TRIPLE:
        return "%"+std::to_string(m_triple_data[op.m_triple->m_index].m_number);
    case OperandCategory::ARGUMENT:
        return "%"+std::to_string(m_argument_data[op.m_argument].m_index);
    default:
        return "error";
    }
}

std::string LLVMCodeGenerator::get_string_literal_name(int index){ 
    return fmt::format("@.str.{}",index);
}

// Poprawiane moze byc zle 
std::string LLVMCodeGenerator::get_operand_value_with_load(const Operand &op){
    switch (op.m_category)
    {
    case OperandCategory::CONSTANT:
    {
        if(op.m_constant.m_type!=DataType::STRING)
            return op.m_constant.get_value_as_string();
        else
            return  get_string_literal_name(m_string_literal_to_index.at(op.m_constant.u.str));
        
    }
    // moze byc zle 
    case OperandCategory::VARIABLE:{
        DataType type = op.get_type();
        std::string data_type_name = get_data_type_name(type);
        
        // Rozmiar boola kiedy przekazywany jest do porownania warunkowego
        // store i8 %7, i8* %3, align 1
        // %8 = load i8, i8* %3, align 1
        // %9 = trunc i8 %8 to i1
        // %10 = zext i1 %9 to i32
        // if(type==DataType::BOOL)
        //     data_type_name="i8";
        // std::string line = fmt::format("%{} = load {}, ptr %{}, align {}",m_llvm_line_index,get_data_type_name(type),m_variable_data.at(op.m_var).m_index,get_align(type));
        if(type==DataType::BOOL)
            data_type_name="i8";

        std::string line = fmt::format("%{} = load {}, {}* %{}, align {}",m_llvm_line_index,data_type_name,data_type_name,m_variable_data.at(op.m_var).m_index,get_align(type));
        size_t loaded_variable_index = m_llvm_line_index;
        increase();
        m_code_lines.push_back(line);

        if(type==DataType::BOOL){
            std::string line = fmt::format("%{} = trunc i8 %{} to i1",m_llvm_line_index,loaded_variable_index);
            m_code_lines.push_back(line);
            increase();
            loaded_variable_index++;
        }
        return "%"+std::to_string(loaded_variable_index);
    }
    // moze byc zle
    case OperandCategory::ARGUMENT:
    {
        DataType type = op.get_type();
        std::string data_type_name = get_data_type_name(type);


        if(type==DataType::BOOL)
            data_type_name="i8";
        
        // std::string line = fmt::format("%{} = load {}, ptr %{}, align {}",m_llvm_line_index,get_data_type_name(type),m_argument_data.at(op.m_argument).m_index,get_align(type));
        std::string line = fmt::format("%{} = load {}, {}* %{}, align {}",m_llvm_line_index,data_type_name,data_type_name,m_argument_data.at(op.m_argument).m_index,get_align(type));
        size_t loaded_argument_index = m_llvm_line_index;
        increase();
        m_code_lines.push_back(line);
           // %9 = trunc i8 %8 to i1
        if(type==DataType::BOOL){
            std::string line = fmt::format("%{} = trunc i8 %{} to i1",m_llvm_line_index,loaded_argument_index);
            m_code_lines.push_back(line);
            increase();
            loaded_argument_index++;
        }
        
        return "%"+std::to_string(loaded_argument_index);
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
    case Operation::LTH:
        return "slt";
    case Operation::LE:
        return "sle";
    case Operation::GTH:
        return "sgt";
    case Operation::GE:
        return "sge";
    case Operation::EQU:
        return "eq";
    case Operation::NE:
        return "ne";
    default:
        return "error";
    }
}

std::string LLVMCodeGenerator::process_argument_list(Triple *triple){
    std::string argument_list;
    std::string argument;
    int i=0;
    for(const auto &arg : triple->m_call_args){
        if(i>0)
            argument_list+=",";
        switch (arg.m_category)
        {
        case OperandCategory::CONSTANT:
        {   
            if(arg.get_type()!=DataType::STRING){
                argument = fmt::format("{} noundef {}",get_data_type_name(arg.get_type()),arg.m_constant.get_value_as_string());
            }else{
                // %16 = call noundef i8* @repStr(i8* noundef getelementptr ([6 x i8], [6 x i8]* @.str.2, i32 0, i32 0),i32 noundef 60)
                // + 4 for counter +1 \0
                // wsadzic obliczanie dlugosci str w jedna metode
                int size = arg.m_constant.get_value_as_string().size()+4+1+1;
                // int true_size = 
                argument = fmt::format("{} noundef getelementptr ([{} x i8], [{} x i8]* @.str.{}, i32 0, i32 0)",get_data_type_name(arg.get_type()),size,size,m_string_literal_to_index.at(arg.m_constant.get_value_as_string()));
                // argument = fmt::format("{} noundef @.str.{} ",get_data_type_name(arg.get_type()),m_string_literal_to_index.at(arg.m_constant.get_value_as_string()));
            }
        
            argument_list+=argument;
            break;
        }
        case OperandCategory::VARIABLE:{

            std::string op =get_operand_value_with_load(arg.m_var);
            argument = fmt::format("{} noundef {}",get_data_type_name(arg.get_type()),op);
            
            argument_list+=argument;
            break;
        }
        case OperandCategory::ARGUMENT:{
            std::string op =get_operand_value_with_load(arg.m_argument);
            argument = fmt::format("{} noundef {}",get_data_type_name(arg.get_type()),op);
            argument_list+=argument;
            break;
        }
        case OperandCategory::TRIPLE:
        {
            argument = fmt::format("{} noundef {}",get_data_type_name(arg.get_type()),get_operand_value_with_load(arg.m_triple));
            argument_list+=argument;
            break;
        }
        default:
            break;
        }
        i++;
    }

    return argument_list;
}

void LLVMCodeGenerator::process_triple(Triple * triple){
    if(triple==nullptr)
        return;

    switch (triple->m_operation)
    {
    case Operation::INIT:
    {
        DataType type_op_1 = triple->m_op_1.get_type();
        std::string op_value_1 = get_operand_value(triple->m_op_1);
        std::string op_value_2 = get_operand_value_with_load(triple->m_op_2);
        std::string alignment = get_align(type_op_1);
        switch (type_op_1)
        {
        case DataType::INT:{
            std::string line = fmt::format("store i32 {}, i32* {}, align {}",op_value_2,op_value_1,alignment);
            m_code_lines.push_back(line);
            break;
        }
        case DataType::BOOL:
        {
            m_triple_data[triple->m_index].m_number = m_llvm_line_index;
            std::string new_line = fmt::format("%{} = zext i1 {} to i8",m_llvm_line_index,op_value_2);
            increase();
            m_code_lines.push_back(new_line);
            std::string line = fmt::format("store i8 %{}, i8* {}, align {}",m_triple_data[triple->m_index].m_number,op_value_1,alignment);
            m_code_lines.push_back(line);
            break;
        }
        case DataType::STRING:{
            // + 4 for counter
            if(triple->m_op_2.m_category==OperandCategory::CONSTANT){
                // to samo tu przeniesc do metody
                int size = triple->m_op_2.m_constant.get_value_as_string().size()+1+4+1;
                std::string line = fmt::format("store i8* getelementptr inbounds ([{} x i8], [{} x i8]* {}, i32 0, i32 0), {}* {}, align {}",size,size,op_value_2,get_data_type_name(type_op_1),op_value_1,alignment);
                m_code_lines.push_back(line);
            }else{
                std::string line = fmt::format("store i8* {}, i8** {}, align {}",op_value_2,op_value_1,alignment);
                m_code_lines.push_back(line);
            }
            break;
        } 
        default:
            break;
        }
        break;
    }
    case Operation::ASSIGN:{
        // type must be the same
        DataType type_op_1 = triple->m_op_1.get_type();
        std::string op_value_1=get_operand_value(triple->m_op_1);
        std::string op_value_2=get_operand_value_with_load(triple->m_op_2);
        std::string alignment = get_align(type_op_1);

        switch (type_op_1)
        {
        case DataType::INT:{
            std::string line = fmt::format("store i32 {}, i32* {}, align {}",op_value_2,op_value_1,alignment);
            m_code_lines.push_back(line);
            break;
        }
        case DataType::BOOL:
        {
            // tutaj poprawka
            m_triple_data[triple->m_index].m_number = m_llvm_line_index;
            std::string new_line = fmt::format("%{} = zext i1 {} to i8",m_llvm_line_index,op_value_2);
            increase();
            m_code_lines.push_back(new_line);
            std::string line = fmt::format("store i8 %{}, i8* {}, align {}",m_triple_data[triple->m_index].m_number,op_value_1,alignment);
            m_code_lines.push_back(line);
            break;
        }
        case DataType::STRING:{
            // tu jeszcze modyfikacja dotyczaca zliczania referencji
            // + 4 for counter
            if(triple->m_op_2.m_category==OperandCategory::CONSTANT){
                int size = triple->m_op_2.m_constant.get_value_as_string().size()+1+4+1;
                std::string line = fmt::format("store i8* getelementptr inbounds ([{} x i8], [{} x i8]* {}, i32 0, i32 0), {}* {}, align {}",size,size,op_value_2,get_data_type_name(type_op_1),op_value_1,alignment);
                m_code_lines.push_back(line);
            }else{
                std::string line = fmt::format("store i8* {}, i8** {}, align {}",op_value_2,op_value_1,alignment);
                m_code_lines.push_back(line);
            }

            break;
        } 
        default:
            break;
        }
        break;
    }
    case Operation::ADD:
    case Operation::SUB:
    case Operation::MUL:{
        if(triple->m_data_type!=DataType::STRING){
            std::string op_1_val = get_operand_value_with_load(triple->m_op_1);
            std::string op_2_val = get_operand_value_with_load(triple->m_op_2);
            m_triple_data[triple->m_index].m_number = m_llvm_line_index;
            std::string operation = ir_op_to_llvm_op(triple->m_operation);
            std::string line = fmt::format("%{} = {} nsw {} {}, {}",m_llvm_line_index,operation,get_data_type_name(triple->m_op_1.get_type()),op_1_val,op_2_val);
            m_code_lines.push_back(line);
            increase();
        }else{
            std::string op_1_val = get_operand_value_with_load(triple->m_op_1);
            std::string op_2_val = get_operand_value_with_load(triple->m_op_2);
            m_triple_data[triple->m_index].m_number = m_llvm_line_index;

            // zamienic na petle 
            std::string line_operand_1;
            if(triple->m_op_1.m_category==OperandCategory::CONSTANT){
                //  4 counter + 1 flag
                int size = triple->m_op_1.m_constant.get_value_as_string().size()+1+4+1;
                line_operand_1 = fmt::format("i8* noundef getelementptr ([{} x i8], [{} x i8]* @.str.{}, i32 0, i32 0)",size,size,m_string_literal_to_index.at(triple->m_op_1.m_constant.get_value_as_string()));
            }else
                line_operand_1 = fmt::format("i8* noundef {}",op_1_val);
            
            std::string line_operand_2; 
            if(triple->m_op_2.m_category==OperandCategory::CONSTANT){
                //  4 counter + 1 flag
                int size = triple->m_op_2.m_constant.get_value_as_string().size()+1+4+1;
                line_operand_2 = fmt::format("i8* noundef getelementptr ([{} x i8], [{} x i8]* @.str.{}, i32 0, i32 0)",size,size,m_string_literal_to_index.at(triple->m_op_2.m_constant.get_value_as_string()));
            }else
                line_operand_2 = fmt::format("i8* noundef {}",op_2_val);


            m_code_lines.push_back(fmt::format("%{} = call noundef i8* @addStrings({},{})",m_llvm_line_index,line_operand_1,line_operand_2));
            increase();
        }
        break;
    }
    case Operation::MOD:
    case Operation::DIV:{
        std::string op_1_val = get_operand_value_with_load(triple->m_op_1);
        std::string op_2_val = get_operand_value_with_load(triple->m_op_2);
        m_triple_data[triple->m_index].m_number = m_llvm_line_index;
        std::string operation = ir_op_to_llvm_op(triple->m_operation);
        std::string line = fmt::format("%{} = {} {} {}, {}",m_llvm_line_index,operation,get_data_type_name(triple->m_op_1.get_type()),op_1_val,op_2_val);
        m_code_lines.push_back(line);
        increase();
        break;
    }
    case Operation::RETURN:{
        if(m_current_fn->m_return_type!=DataType::VOID)
        {
            std::string ref = get_operand_value_with_load(triple->m_op_1);
            m_code_lines.push_back(fmt::format("ret {} {}",get_data_type_name(triple->m_op_1.get_type()),ref));
            increase();
        }else{
            m_code_lines.push_back(fmt::format("ret {}",get_data_type_name(triple->m_op_1.get_type())));
            increase();
        }
        break;
    }
    case Operation::CALL:
    {
        Function *fn = triple->m_op_1.m_function;
        std::string argument_list = process_argument_list(triple);
        m_triple_data[triple->m_index].m_number = m_llvm_line_index;
        if(fn->m_return_type!=DataType::VOID){
            m_code_lines.push_back(fmt::format("%{} = call noundef {} @{}({})",m_llvm_line_index,get_data_type_name(fn->m_return_type),fn->m_name,argument_list));
            increase();
        }
        else
            m_code_lines.push_back(fmt::format("call {} @{}({})",get_data_type_name(fn->m_return_type),fn->m_name,argument_list));
        
        break;
    }
    case Operation::OR:
    case Operation::AND:
    {
        // tak ma byc
        break;
    }
    case Operation::LTH:
    case Operation::LE:
    case Operation::GTH:
    case Operation::GE:
    case Operation::EQU:
    case Operation::NE:
    {
        std::string operation = ir_op_to_llvm_op(triple->m_operation);
        DataType type=triple->m_op_1.get_type();
        std::string op_1_val = get_operand_value_with_load(triple->m_op_1);
        std::string op_2_val = get_operand_value_with_load(triple->m_op_2);
        m_triple_data[triple->m_index].m_number = m_llvm_line_index;
        m_code_lines.push_back(fmt::format("%{} = icmp {} {} {}, {}",m_llvm_line_index,operation,get_data_type_name(type),op_1_val,op_2_val));
        increase();
        break;
    }
    case Operation::MARKER:
        // we check if a previous triple at the end of a block was branch
        if(m_code_lines.size()>0 && m_code_lines[m_code_lines.size()-1].substr(0,3)!="br ")
            m_code_lines.push_back(fmt::format("br label %Label{}",m_triple_data.at(triple->m_index).m_marker_index));
        m_code_lines.push_back(fmt::format("Label{}:",m_triple_data.at(triple->m_index).m_marker_index));
        break;
    case Operation::JT:
    case Operation::JF:
    {
        std::string condition = get_operand_value_with_load(triple->m_op_1);
        int label_false;
        int label_true;
        bool added_marker = false;
        if(m_current_fn->m_triples[triple->m_index+1]->m_operation==Operation::MARKER){
            Triple * next_triple = m_current_fn->m_triples[triple->m_index+1];
            label_false = m_triple_data[next_triple->m_index].m_marker_index;
        }
        else{
            label_false = m_last_marker_number+1;
            m_last_marker_number++;
            added_marker=true;
        }

        label_true = m_triple_data[triple->m_op_2.m_label->m_jump_to->m_index].m_marker_index;
        
        if(triple->m_operation==Operation::JF)
            std::swap(label_false,label_true);

        m_code_lines.push_back(fmt::format("br i1 {}, label %Label{}, label %Label{}",condition,label_true,label_false));
        
        if(added_marker)
            m_code_lines.push_back(fmt::format("Label{}:", triple->m_operation == Operation::JT ? label_false : label_true));
        break;
    }
    case Operation::JMP:{
        m_code_lines.push_back(fmt::format("br label %Label{}",m_triple_data[triple->m_op_1.m_label->m_jump_to->m_index].m_marker_index));
        break;
    }
    case Operation::NEG:{
        std::string op_1 = get_operand_value_with_load( triple->m_op_1);
        m_triple_data[triple->m_index].m_number = m_llvm_line_index;
        m_code_lines.push_back(fmt::format("%{} = sub nsw {} 0, {}",m_llvm_line_index,get_data_type_name(triple->m_op_1.get_type()),op_1));
        increase();
        break;
    }
    case Operation::INC:
    {
        std::string op_1 =get_operand_value_with_load(triple->m_op_1);
        m_code_lines.push_back(fmt::format("%{} = add nsw {} {}, 1",m_llvm_line_index,get_data_type_name(triple->m_op_1.get_type()),op_1));
        std::string from = "%"+std::to_string(m_llvm_line_index);
        std::string to = get_operand_value(triple->m_op_1);
        std::string store = fmt::format("store i32 {}, i32* {}, align 4",from,to);
        m_code_lines.push_back(store);
        increase();
        break;
    }
    case Operation::DEC:
    {
        std::string op_1 =get_operand_value_with_load(triple->m_op_1);
        m_code_lines.push_back(fmt::format("%{} = add nsw {} {}, -1",m_llvm_line_index,get_data_type_name(triple->m_op_1.get_type()),op_1));
        std::string from = "%"+std::to_string(m_llvm_line_index);
        std::string to = get_operand_value(triple->m_op_1);
        // we can decrement only integer values
        std::string store = fmt::format("store i32 {}, i32* {}, align 4",from,to);
        m_code_lines.push_back(store);
        increase();
        break;
    }
    default:
        break;
    }
}

std::string LLVMCodeGenerator::process_argument(Argument * arg,size_t index){
    switch (arg->m_type)
    {
    case DataType::INT:
        return fmt::format("i32 noundef %{}",index);
    case DataType::BOOL:
        return fmt::format("i1 noundef zeroext %{}",index);
    case DataType::STRING:
        // pointer to int  
        return fmt::format("i8* noundef %{}",index); 
    default:
        throw 0;
    }
}

void LLVMCodeGenerator::collect_string_literals() {
    for(size_t i=0;i<m_intermediate_program->m_functions.size();i++){
        for(const auto &triple : m_intermediate_program->m_functions[i]->m_triples){
            if(triple->m_op_1.m_category==OperandCategory::CONSTANT && triple->m_op_1.get_type()==DataType::STRING)
                m_string_literals.push_back(triple->m_op_1.m_constant.get_value_as_string());

            if(triple->m_op_2.m_category==OperandCategory::CONSTANT && triple->m_op_2.get_type()==DataType::STRING)
                m_string_literals.push_back(triple->m_op_2.m_constant.get_value_as_string());
            
            for(auto &op : triple->m_call_args){
                if(op.m_category==OperandCategory::CONSTANT && op.get_type()==DataType::STRING)
                    m_string_literals.push_back(op.m_constant.get_value_as_string());
            }
        }
    }
}

void LLVMCodeGenerator::map_string_literals(){
    for(size_t i=0;i<m_string_literals.size();i++){
        if(m_string_literal_to_index.count(m_string_literals[i])==0)
            m_string_literal_to_index[m_string_literals[i]] = i+1;
    }

}

// to-do correct the order of string literals
void LLVMCodeGenerator::generate_string_literal_declarations(){
    for(const auto& [key, value] : m_string_literal_to_index){

        std::string str = "";
        for(size_t i = 0;i<key.size();i++){
            switch (key[i])
            {
            case '\n':
                str += "\\0A";
                break;
            case '\"':
                str+= "\\22";
                break;
            case '\t':
                str+= "\\09";
                break;
            case '\a':
                str+= "\\07";
                break;
            case '\\':
                str+= "\\5C";
                break;
            case '\b':
                str+= "\\08";
                break;
            case '\f':
                str+= "\\0C";
                break;
            case '\v':
                str+= "\\0B";
                break;
            case '\0':
                str+= "\\00";
                break;
            default:
                str += key[i];
                break;
            }
        }
        
        // an inserted counter for calculating references to the string 
        // +4 for a counter + for flag
        // std::string line = fmt::format("@.str.{} = private unnamed_addr constant [{} x i8] c\"\\00\\01\\00\\00\\00{}\", align 1",
        //     value,key.size()+4+1+1,str+"\\00");
        std::string line = fmt::format("@.str.{} = private unnamed_addr constant [{} x i8] c\"\\00\\00\\00\\00\\00{}\", align 1",
            value,key.size()+4+1+1,str+"\\00");
        
        m_code_lines.push_back(line);
    }
}

std::string LLVMCodeGenerator::make_alloca_string(DataType type){

    std::string alloc="";
    switch (type)
    {
    case DataType::INT:
        alloc = fmt::format("%{} = alloca i32, align 4",m_llvm_line_index);
        break;
    case DataType::BOOL:
        // w allocach zostaje i8 
        alloc = fmt::format("%{} = alloca i8, align 1",m_llvm_line_index);
        break;
    case DataType::STRING:
        alloc = fmt::format("%{} = alloca i8*, align 8",m_llvm_line_index);
        break;
    default:
        break;
    }

    increase();
    return alloc;
}
//  plik w  ktorym zmieniam!!!!!!!!!!!!!!!!
// changed
// std::string LLVMCodeGenerator::make_store_string(DataType type,int from,int to){
std::string LLVMCodeGenerator::make_store_string(Argument *arg,int from,int to){
    std::string store = "";
    switch (arg->m_type)
    {
    case DataType::INT:
        store = fmt::format("store i32 %{}, i32* %{}, align 4",from,to);
        break;
    case DataType::BOOL:{
        std::string new_line = fmt::format("%{} = zext i1 %{} to i8",m_llvm_line_index,from);
        m_code_lines.push_back(new_line);
        store = fmt::format("store i8 %{}, i8* %{}, align 1",m_llvm_line_index,to); 
        increase();
        break;
    }
    case DataType::STRING:{
        // store = fmt::format("store ptr %{}, ptr %{}, align 8",from,to);
        // m_string_literals
                                                                // type 
        // store = fmt::format("store i8* getelementptr inbounds ([{} x i8],[{} x i8]* @.str.{}, i32 0, i32 0), i8** {}",size,size,from,to);
        // store = fmt::format("store i8* getelementptr inbounds ([{} x i8],[{} x i8]* {}, i32 0, i32 0), i8** {}",size,size,from,to);
        std::string type_name = get_data_type_name(arg->m_type);
        std::string line = fmt::format("store {} %{}, {}* %{}, align {}",type_name,from,type_name,to,get_align(arg->m_type));
        m_code_lines.push_back(line);
        break;
    }
    default:
        throw 0;
    }

    return store;
}

void LLVMCodeGenerator::alloc_arguments_and_variables(const Function *fn){
    for( auto arg : fn->m_arguments ){
        m_argument_data[arg]=ArgumentData{m_llvm_line_index};
        m_code_lines.push_back(make_alloca_string(arg->m_type));
    }
    
    for(auto var : fn->m_variables){
        m_variable_data[var]=VariableData{m_llvm_line_index};
        m_code_lines.push_back(make_alloca_string(var->m_type));
    }
}

void LLVMCodeGenerator::store_arguments(const Function *fn){
    
    size_t to = fn->m_arguments.size()+1;
    for( size_t i=0; i < fn->m_arguments.size() ; i++){
        // moze byc zle 
        // m_code_lines.push_back(make_store_string(fn->m_arguments[i]->m_type,i,to));
        m_code_lines.push_back(make_store_string(fn->m_arguments[i],i,to));
        to++;
    }

}


void LLVMCodeGenerator::process_function(Function *fn){
    m_current_fn=fn;
    m_triple_data.resize(fn->m_triples.size());
    enumerate_all_markers(fn);
    m_llvm_line_index=0;

    std::string argument_list;
    for(size_t i=0;i<fn->m_arguments.size();i++){
        if(i>0)
            argument_list+=", ";
        argument_list+=process_argument(fn->m_arguments[i],m_llvm_line_index);
        increase();
    }

    std::string fn_dec;
    if(fn->m_return_type!=DataType::VOID)
        fn_dec=fmt::format("define dso_local noundef {} @{}({}) {{",get_data_type_name(fn->m_return_type),fn->m_name,argument_list);
    else
        fn_dec=fmt::format("define dso_local {} @{}({}) {{",get_data_type_name(fn->m_return_type),fn->m_name,argument_list);

    m_code_lines.push_back(fn_dec);
    increase();

    alloc_arguments_and_variables(fn);
    store_arguments(fn);

    for(auto triple : fn->m_triples)
        process_triple(triple);
    
    if(fn->m_return_type==DataType::VOID && fn->m_triples.size()==0)
        m_code_lines.push_back("ret void");
    else if(fn->m_return_type==DataType::VOID && fn->m_triples.back()->m_operation!=Operation::RETURN)
        m_code_lines.push_back("ret void");
    else if(fn->m_return_type==DataType::INT && m_code_lines.back().substr(0,5)=="Label")
        m_code_lines.push_back("ret i32 0");

    m_code_lines.push_back("}");
}

void LLVMCodeGenerator::add_used_predefined_functions(){
    m_code_lines.push_back("declare dso_local noundef i8* @addStrings(i8* noundef %0, i8* noundef %1)");
    m_code_lines.push_back("declare dso_local void @printInt(i32 noundef %0)");
    m_code_lines.push_back("declare dso_local void @printString(i8* noundef %0)");
    m_code_lines.push_back("declare dso_local noundef i8* @readString()");
    m_code_lines.push_back("declare dso_local noundef i32 @readInt()");
    m_code_lines.push_back("declare dso_local void @increaseStringCounter(i8* noundef %0)");
    m_code_lines.push_back("declare dso_local void @decreaseStringCounter(i8* noundef %0)");
    // define dso_local zeroext i1 @isConstant(i8* noundef %0) 
    m_code_lines.push_back("declare dso_local zeroext i1 @isConstant(i8* noundef %0)");
    // define dso_local i32 @getReferenceCount(i8* noundef %0) #2 
    m_code_lines.push_back("declare dso_local i32 @getReferenceCount(i8* noundef %0)");
}

void LLVMCodeGenerator::enumerate_all_markers(Function *fn){
    size_t marker_index=0;
    for(auto triple : fn->m_triples){
        if(triple->m_operation == Operation::MARKER){
            m_triple_data.at(triple->m_index).m_marker_index=marker_index;
            marker_index++;
        }
    }
    m_last_marker_number = marker_index;
}

std::string LLVMCodeGenerator::process_program(){
    
    collect_string_literals();
    map_string_literals();
    generate_string_literal_declarations();

    for(auto &fn : m_intermediate_program->m_functions)
    {   
        if(fn->m_type==PredefinedFunction::USERDEFINED)
            process_function(fn);
    }

    add_used_predefined_functions();

    return "";
}


void LLVMCodeGenerator::print_generated_code() {
    for(auto line:m_code_lines)
        std::cout<<line<<std::endl;
}