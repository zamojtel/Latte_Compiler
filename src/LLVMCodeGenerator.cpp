#include "Libraries.h"
#include "LLVMCodeGenerator.h"
#include <iostream>

int LLVMCodeGenerator::get_string_length(const std::string str){
    return str.size()+COUNTER_SIZE+MODIFIABLE_FLAG+END_OF_STRING;
}

std::string LLVMCodeGenerator::get_data_type_name(DataType type,bool for_storage){
    std::string size;
    if(type.category==DataTypeCategory::BASIC){
        switch (type.basic_type)
        {
        case BasicType::BOOL:{
            if(for_storage)
                return "i8";
            else
                size = type.dimensions>0 ? "i8" : "i1";
            break;
        }
        case BasicType::INT:
            size= "i32";
            break;
        case BasicType::STRING:
            size= "i8*";
            break;
        case BasicType::VOID:
            size= "void";
            break;
        case BasicType::NULLPTR:
            size = "i8*";
            break;
        default:
            return "error unknown size";
            break;
        }
    }else{
        if(type.dimensions<0)
            size = fmt::format("%class.{}",type.class_type->m_name);
        else
            size = fmt::format("%class.{}*",type.class_type->m_name);
    }

    for(int i=0;i<type.dimensions;i++)
        size+="*";

    return size;
}

std::string LLVMCodeGenerator::get_align(DataType type){
    if(type.category==DataTypeCategory::BASIC){
        if(type.dimensions>=1){
            return "8";
        }else{
            switch (type.basic_type)
            {
            case BasicType::BOOL:
                return "1";
            case BasicType::INT:
                return "4";
            case BasicType::STRING:
                return "8";
            default:
                throw std::runtime_error("LLVMCodeGenerator::get_align(DataType type)");
                break;
            }
        }
    }else
        return "8";

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

std::string LLVMCodeGenerator::get_operand_value_with_load(const Operand &op){

    switch (op.m_category)
    {
    case OperandCategory::CONSTANT:
    {
        if(op.m_constant.m_type!=BasicType::STRING)
            return op.m_constant.get_value_as_string();
        else{
            int string_index = m_string_literal_to_index.at(op.m_constant.u.str);
            if(m_string_literal_to_line.count(string_index)>0){
                return m_string_literal_to_line.at(string_index);
            }else{
                int string_size = get_string_length(op.m_constant.u.str);
                // int string_index = m_string_literal_to_index.at(op.m_constant.u.str);
                std::string line = fmt::format("%{} = getelementptr inbounds [{} x i8], [{} x i8]* @.str.{}, i32 0, i32 0",m_llvm_line_index,string_size,string_size,string_index);
                m_code_lines.push_back(line);
                increase();
                return fmt::format("%{}",m_llvm_line_index-1);
            }
        }
    }
    case OperandCategory::VARIABLE:{
        return get_operand_value_with_load(m_current_fn->m_var_arg_to_triple.at(op));
        break;
    }
    case OperandCategory::ARGUMENT:
    {
        if(op.m_version==0)
            return fmt::format("%{}",m_argument_data.at(op.m_argument).m_index);
        else
            return get_operand_value_with_load(m_current_fn->m_var_arg_to_triple.at(op));

        break;
    }
    case OperandCategory::TRIPLE:{
        // if(op.m_triple->m_operation==Operation::MEMEBER_ACCESS){
        //     DataType type = op.get_type();
        //     std::string op_val = "%"+std::to_string(m_triple_data[op.m_triple->m_index].m_number);
        //     std::string item_size = get_data_type_name(type,true);
        //     std::string alignment = get_align(type);
        //     std::string line = fmt::format("%{} = load {}, {}* {}, align {}",m_llvm_line_index,item_size,item_size,op_val,alignment);
        //     m_code_lines.push_back(line);
        //     increase();
        //     if(op.m_triple->m_data_type==BasicType::BOOL){
        //         m_triple_data[op.m_triple->m_index].m_number = m_llvm_line_index;
        //         std::string line_2 = fmt::format("%{} = trunc i8 %{} to i1",m_llvm_line_index,m_llvm_line_index-1);
        //         m_code_lines.push_back(line_2);
        //         increase();
        //     }

        //     std::string ret = fmt::format("%{}",m_llvm_line_index-1);
        //     return ret;
        // }
        // else
        return "%"+std::to_string(m_triple_data[op.m_triple->m_index].m_number);
    }
    default:
        throw std::runtime_error("LLVMCodeGenerator::get_operand_value_with_load(const Operand &op)");
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
    case Operation::ACCESS_ARRAY:
        return "accarr";
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
            if(arg.get_type()!=BasicType::STRING){
                argument = fmt::format("{} noundef {}",get_data_type_name(triple->m_op_1.m_function->m_arguments[i]->m_type),arg.m_constant.get_value_as_string());
            }else{
                // + 4 for counter +1 \0
                // DOPRZEJRZENIA
                // int size = arg.m_constant.get_value_as_string().size()+4+1+1;
                int size = get_string_length(arg.m_constant.get_value_as_string());
                argument = fmt::format("{} noundef getelementptr ([{} x i8], [{} x i8]* @.str.{}, i32 0, i32 0)",get_data_type_name(arg.get_type()),size,size,m_string_literal_to_index.at(arg.m_constant.get_value_as_string()));
            }
            argument_list+=argument;
            break;
        }
        case OperandCategory::VARIABLE:{
            std::string op = get_operand_value_with_load(arg.m_var);
            argument = fmt::format("{} noundef {}",get_data_type_name(arg.get_type()),op);
            argument_list+=argument;
            break;
        }
        case OperandCategory::ARGUMENT:{
            std::string op = get_operand_value_with_load(arg.m_argument);
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

int LLVMCodeGenerator::get_storage_size(DataType type){
    if(type.category==DataTypeCategory::BASIC){
        if(type.dimensions==0){
            switch (type.basic_type)
            {
            case BasicType::INT:
                return 4;
            case BasicType::BOOL:
                return 1;
            case BasicType::STRING:
                return 8;
            default:
                throw std::runtime_error("No storage size of that type");
            }
        }else
            return 8;
    }else
        return 8;
}

int LLVMCodeGenerator::calculate_instance_size(MyClass * cl){
    if(cl==nullptr)
        return 0;

    int instance_size=0;
    for(size_t i=0;i<cl->m_fields.size();i++){
        int size = 8;
        instance_size+=size;
    }

    instance_size += calculate_instance_size(cl->m_base_class);

    return instance_size;
}

std::string LLVMCodeGenerator::process_phi_argument(const PHIArgument &arg){
    std::string argument="";
    Operand op = arg.m_operand;

    std::string op_value = get_operand_value_with_load(m_current_fn->m_var_arg_to_triple.at(op));
    argument = fmt::format("[ {}, %Label{}]",op_value,arg.m_basic_block->m_index);

    return argument;
}

void LLVMCodeGenerator::process_triple(Triple * triple){
    if(triple==nullptr)
        return;

    switch (triple->m_operation)
    {
    case Operation::PHI:{

        m_triple_to_phi[triple] = m_code_lines.size();
        m_triple_data[triple->m_index].m_number=m_llvm_line_index;
        increase();
        std::string line="";
        m_code_lines.push_back(line);
        break;
    }
    case Operation::ACCESS_ARRAY:{

        // wyrzucic wyjatek
        // DataType type_op_1 = triple->m_op_1.get_type();
        // std::string op_value_1 = get_operand_value_with_load(triple->m_op_1);
        // std::string op_value_2 = get_operand_value_with_load(triple->m_op_2);
        // std::string item_type = get_data_type_name(type_op_1.decrement_dimensions(),true); // one because its with access array

        // std::string alignment = get_align(type_op_1);
        // m_triple_data[triple->m_index].m_number = m_llvm_line_index;
        // std::string line_1 = fmt::format("%{} = getelementptr inbounds {}, {}* {}, i32 {}",m_llvm_line_index,item_type,item_type,op_value_1,op_value_2);
        // increase();
        // m_code_lines.push_back(line_1);

        // a[0] = a[1]+a[2];
        // {
        //     Operand op = triple;
        //     DataType type = triple->m_data_type;
        //     std::string op_val = "%"+std::to_string(m_triple_data[op.m_triple->m_index].m_number);
        //     std::string item_size = get_data_type_name(type,true);
        //     std::string alignment = get_align(type);
        //     m_triple_data[triple->m_index].m_number = m_llvm_line_index;
        //     std::string line = fmt::format("%{} = load {}, {}* {}, align {}",m_llvm_line_index,item_size,item_size,op_val,alignment);
        //     m_code_lines.push_back(line);
        //     increase();

        //     if(op.m_triple->m_data_type==BasicType::BOOL){
        //         m_triple_data[op.m_triple->m_index].m_number = m_llvm_line_index;
        //         std::string line_2 = fmt::format("%{} = trunc i8 %{} to i1",m_llvm_line_index,m_llvm_line_index-1);
        //         m_code_lines.push_back(line_2);
        //         increase();
        //     }

        // }

        break;
    }
    case Operation::ACCESS_ARRAY_FOR_READ:{
        DataType type_op_1 = triple->m_op_1.get_type();
        std::string op_value_1 = get_operand_value_with_load(triple->m_op_1);
        std::string op_value_2 = get_operand_value_with_load(triple->m_op_2);
        std::string item_type = get_data_type_name(type_op_1.decrement_dimensions(),true); // one because its with access array

        std::string alignment = get_align(type_op_1);
        m_triple_data[triple->m_index].m_number = m_llvm_line_index;
        std::string line_1 = fmt::format("%{} = getelementptr inbounds {}, {}* {}, i32 {}",m_llvm_line_index,item_type,item_type,op_value_1,op_value_2);
        increase();
        m_code_lines.push_back(line_1);

        DataType type = triple->m_data_type;
        std::string op_val = "%"+std::to_string(m_triple_data[triple->m_index].m_number);
        std::string item_size = get_data_type_name(type,true);
        alignment = get_align(type);
        std::string line_2 = fmt::format("%{} = load {}, {}* {}, align {}",m_llvm_line_index,item_size,item_size,op_val,alignment);
        m_code_lines.push_back(line_2);
        increase();
        if(triple->m_data_type==BasicType::BOOL){
            m_triple_data[triple->m_index].m_number = m_llvm_line_index;
            std::string line_2 = fmt::format("%{} = trunc i8 %{} to i1",m_llvm_line_index,m_llvm_line_index-1);
            m_code_lines.push_back(line_2);
            increase();
        }
        m_triple_data[triple->m_index].m_number = m_llvm_line_index-1;
        break;
    }
    case Operation::ACCESS_ARRAY_FOR_WRITE:{
        DataType type_op_1 = triple->m_op_1.get_type();
        std::string op_value_1 = get_operand_value_with_load(triple->m_op_1);
        std::string op_value_2 = get_operand_value_with_load(triple->m_op_2);
        std::string item_type = get_data_type_name(type_op_1.decrement_dimensions(),true); // one because its with access array

        std::string alignment = get_align(type_op_1);
        m_triple_data[triple->m_index].m_number = m_llvm_line_index;
        std::string line_1 = fmt::format("%{} = getelementptr inbounds {}, {}* {}, i32 {}",m_llvm_line_index,item_type,item_type,op_value_1,op_value_2);
        increase();
        m_code_lines.push_back(line_1);
        break;
    }
    case Operation::ARRAY_LENGTH:{
        DataType type_1 = triple->m_op_1.get_type();
        std::string op_value_1 = get_operand_value_with_load(triple->m_op_1);
        std::string type_size = get_data_type_name(type_1);
        std::string line_1 = fmt::format("%{} = bitcast {} {} to i8*",m_llvm_line_index,type_size,op_value_1);
        increase();
        m_triple_data[triple->m_index].m_number=m_llvm_line_index;
        std::string line_2 = fmt::format("%{} = call noundef i32 @getArrayLength(i8* noundef %{})",m_llvm_line_index,m_llvm_line_index-1);
        increase();
        m_code_lines.push_back(line_1);
        m_code_lines.push_back(line_2);
        break;
    }
    case Operation::NEW_ARRAY:{
        int item_size = get_storage_size(triple->m_data_type_for_new);
        std::string op_value_1 = get_operand_value_with_load(triple->m_op_1);

        std::string line = fmt::format("%{} = call noundef i8* @allocArray(i32 noundef {},i32 noundef {})",m_llvm_line_index,op_value_1,item_size);
        increase();
        m_code_lines.push_back(line);
        if(triple->m_data_type_for_new!=BasicType::BOOL){
            std::string item_type = get_data_type_name(triple->m_data_type_for_new);
            std::string line_2 = fmt::format("%{} = bitcast i8* %{} to {}*",m_llvm_line_index,m_llvm_line_index-1,item_type);
            increase();
            m_code_lines.push_back(line_2);
        }
        m_triple_data[triple->m_index].m_number = m_llvm_line_index-1;

        break;
    }
    case Operation::MEMEBER_ACCESS:{
        // DataType type = triple->m_op_1.get_type();
        // MyClass *cl = type.class_type;
        // Field * field = triple->m_op_2.m_field;

        // std::string op_1_value = get_operand_value_with_load(triple->m_op_1);
        // std::string data_type_name = get_data_type_name(type.decrement_dimensions());
        // MyClass *field_class = field->get_class();

        // if(cl == field_class){
        //     m_triple_data[triple->m_index].m_number = m_llvm_line_index;
        //     int add_one = cl->m_base_class == nullptr ? 0 : 1;
        //     std::string line = fmt::format("%{} = getelementptr inbounds {}, {}* {}, i32 0, i32 {}",m_llvm_line_index,data_type_name,data_type_name,op_1_value,field->get_index()+add_one);
        //     m_code_lines.push_back(line);
        //     increase();
        // }else{
        //     std::string bitcast = fmt::format("%{} = bitcast %class.{}* {} to %class.{}*",m_llvm_line_index,cl->m_name,op_1_value,field_class->m_name);
        //     m_code_lines.push_back(bitcast);
        //     increase();
        //     m_triple_data[triple->m_index].m_number = m_llvm_line_index;
        //     int add_one = field_class->m_base_class == nullptr ? 0 : 1;
        //     std::string line = fmt::format("%{} = getelementptr inbounds %class.{}, %class.{}* %{}, i32 0, i32 {}",m_llvm_line_index,field_class->m_name,field_class->m_name,m_llvm_line_index-1,field->get_index()+add_one);
        //     m_code_lines.push_back(line);
        //     increase();
        // }
        break;
    }
    case Operation::MEMBER_FOR_READ:{
        DataType type = triple->m_op_1.get_type();
        MyClass *cl = type.class_type;
        Field * field = triple->m_op_2.m_field;

        std::string op_1_value = get_operand_value_with_load(triple->m_op_1);
        std::string data_type_name = get_data_type_name(type.decrement_dimensions());
        MyClass *field_class = field->get_class();

        if(cl == field_class){
            m_triple_data[triple->m_index].m_number = m_llvm_line_index;
            int add_one = cl->m_base_class == nullptr ? 0 : 1;
            std::string line = fmt::format("%{} = getelementptr inbounds {}, {}* {}, i32 0, i32 {}",m_llvm_line_index,data_type_name,data_type_name,op_1_value,field->get_index()+add_one);
            m_code_lines.push_back(line);
            increase();
        }else{
            std::string bitcast = fmt::format("%{} = bitcast %class.{}* {} to %class.{}*",m_llvm_line_index,cl->m_name,op_1_value,field_class->m_name);
            m_code_lines.push_back(bitcast);
            increase();
            m_triple_data[triple->m_index].m_number = m_llvm_line_index;
            int add_one = field_class->m_base_class == nullptr ? 0 : 1;
            std::string line = fmt::format("%{} = getelementptr inbounds %class.{}, %class.{}* %{}, i32 0, i32 {}",m_llvm_line_index,field_class->m_name,field_class->m_name,m_llvm_line_index-1,field->get_index()+add_one);
            m_code_lines.push_back(line);
            increase();
        }

        DataType type_2 = triple->m_data_type;
        std::string op_val = "%"+std::to_string(m_triple_data[triple->m_index].m_number);
        std::string item_size = get_data_type_name(type_2,true);
        std::string alignment = get_align(type_2);
        std::string line = fmt::format("%{} = load {}, {}* {}, align {}",m_llvm_line_index,item_size,item_size,op_val,alignment);
        m_code_lines.push_back(line);
        increase();
        if(triple->m_data_type==BasicType::BOOL){
            m_triple_data[triple->m_index].m_number = m_llvm_line_index;
            std::string line_2 = fmt::format("%{} = trunc i8 %{} to i1",m_llvm_line_index,m_llvm_line_index-1);
            m_code_lines.push_back(line_2);
            increase();
        }
        m_triple_data[triple->m_index].m_number=m_llvm_line_index-1;

        break;
    }
    case Operation::MEMBER_FOR_WRITE:{
        DataType type = triple->m_op_1.get_type();
        MyClass *cl = type.class_type;
        Field * field = triple->m_op_2.m_field;

        std::string op_1_value = get_operand_value_with_load(triple->m_op_1);
        std::string data_type_name = get_data_type_name(type.decrement_dimensions());
        MyClass *field_class = field->get_class();

        if(cl == field_class){
            m_triple_data[triple->m_index].m_number = m_llvm_line_index;
            int add_one = cl->m_base_class == nullptr ? 0 : 1;
            std::string line = fmt::format("%{} = getelementptr inbounds {}, {}* {}, i32 0, i32 {}",m_llvm_line_index,data_type_name,data_type_name,op_1_value,field->get_index()+add_one);
            m_code_lines.push_back(line);
            increase();
        }else{
            std::string bitcast = fmt::format("%{} = bitcast %class.{}* {} to %class.{}*",m_llvm_line_index,cl->m_name,op_1_value,field_class->m_name);
            m_code_lines.push_back(bitcast);
            increase();
            m_triple_data[triple->m_index].m_number = m_llvm_line_index;
            int add_one = field_class->m_base_class == nullptr ? 0 : 1;
            std::string line = fmt::format("%{} = getelementptr inbounds %class.{}, %class.{}* %{}, i32 0, i32 {}",m_llvm_line_index,field_class->m_name,field_class->m_name,m_llvm_line_index-1,field->get_index()+add_one);
            m_code_lines.push_back(line);
            increase();
        }
        break;
    }
    case Operation::NEW_INSTANCE:{
        int instance_size=0;
        MyClass * cl = triple->m_data_type_for_new.class_type;
        instance_size = calculate_instance_size(cl);

        std::string tableName = fmt::format("@.vtable.{}",cl->m_name);
        std::string first_arg = fmt::format("i8** noundef bitcast ([{} x i8*]* {} to i8**)",cl->m_vtable.size(),tableName);
        std::string line = fmt::format("%{} = call i8* @allocateInstance({},i32 noundef {})",m_llvm_line_index,first_arg,instance_size);
        m_code_lines.push_back(line);
        increase();
        m_triple_data[triple->m_index].m_number = m_llvm_line_index;
        std::string class_type = get_data_type_name(triple->m_data_type_for_new);
        std::string line_2 = fmt::format("%{} = bitcast i8* %{} to {}",m_llvm_line_index,m_llvm_line_index-1,class_type);
        m_code_lines.push_back(line_2);
        increase();
        break;
    }
    case Operation::INIT:
    case Operation::ASSIGN:{
        DataType type_op_1 = triple->m_op_1.get_type();
        std::string op_value_1=get_operand_value(triple->m_op_1);
        std::string op_value_2=get_operand_value_with_load(triple->m_op_2);
        std::string alignment = get_align(type_op_1);

        if(type_op_1.dimensions==0){
            if(type_op_1.category==DataTypeCategory::BASIC){
                switch (type_op_1.basic_type)
                {
                case BasicType::INT:{
                    if(triple->m_op_1.m_category==OperandCategory::TRIPLE){
                        std::string line = fmt::format("store i32 {}, i32* {}, align {}",op_value_2,op_value_1,alignment);
                        m_code_lines.push_back(line);
                    }
                    break;
                }
                case BasicType::BOOL:
                {
                    if(triple->m_op_1.m_category==OperandCategory::TRIPLE){
                        m_triple_data[triple->m_index].m_number = m_llvm_line_index;
                        std::string new_line = fmt::format("%{} = zext i1 {} to i8",m_llvm_line_index,op_value_2);
                        increase();
                        m_code_lines.push_back(new_line);
                        std::string line = fmt::format("store i8 %{}, i8* {}, align {}",m_triple_data[triple->m_index].m_number,op_value_1,alignment);
                        m_code_lines.push_back(line);
                    }
                    break;
                }
                case BasicType::STRING:{
                    if(triple->m_op_1.m_category==OperandCategory::TRIPLE){
                        DataType type = triple->m_op_1.get_type();
                        std::string data_type = get_data_type_name(type);
                        std::string line = fmt::format("store {} {}, {}* {}, align {}",data_type,op_value_2,data_type,op_value_1,alignment);
                        m_code_lines.push_back(line);
                    }else if(triple->m_op_2.m_category==OperandCategory::CONSTANT){
                        if(triple->m_op_2.get_type()!=BasicType::NULLPTR){
                            m_string_literal_to_line[m_string_literal_to_index.at(triple->m_op_2.m_constant.get_value_as_string())] = op_value_2;
                        }
                    }
                    break;
                }
                default:
                    break;
                }
            }else{
                if(triple->m_op_1.m_category==OperandCategory::TRIPLE){
                    std::string class_type = get_data_type_name(type_op_1);
                    std::string line = fmt::format("store {} {}, {}* {}, align {}",class_type,op_value_2,class_type,op_value_1,alignment);
                    m_code_lines.push_back(line);
                }
            }
            break;
        }else{
            if(triple->m_op_1.m_category==OperandCategory::TRIPLE){
                DataType type = triple->m_op_1.get_type();
                std::string data_type = get_data_type_name(type);
                std::string line = fmt::format("store {} {}, {}* {}, align {}",data_type,op_value_2,data_type,op_value_1,alignment);
                m_code_lines.push_back(line);
            }
        }
        break;
    }
    case Operation::ADD:
    case Operation::SUB:
    case Operation::MUL:{
        if(triple->m_data_type!=BasicType::STRING){
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

            std::string line_operand_1;
            if(triple->m_op_1.m_category==OperandCategory::CONSTANT){
                int size = get_string_length(triple->m_op_1.m_constant.get_value_as_string());
                line_operand_1 = fmt::format("i8* noundef getelementptr ([{} x i8], [{} x i8]* @.str.{}, i32 0, i32 0)",size,size,m_string_literal_to_index.at(triple->m_op_1.m_constant.get_value_as_string()));
            }else
                line_operand_1 = fmt::format("i8* noundef {}",op_1_val);

            std::string line_operand_2;
            if(triple->m_op_2.m_category==OperandCategory::CONSTANT){
                int size = get_string_length(triple->m_op_2.m_constant.get_value_as_string());
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
        if(m_current_fn->m_return_type.category==DataTypeCategory::BASIC){
            if(m_current_fn->m_return_type!=BasicType::VOID)
            {
                //TODO increase to be removed
                std::string ref = get_operand_value_with_load(triple->m_op_1);
                m_code_lines.push_back(fmt::format("ret {} {}",get_data_type_name(triple->m_op_1.get_type()),ref));
            }else
                m_code_lines.push_back(fmt::format("ret {}",get_data_type_name(triple->m_op_1.get_type())));
            increase();
        }else{
            std::string op_1 = get_operand_value_with_load(triple->m_op_1);
            std::string type = get_data_type_name(m_current_fn->m_return_type);
            std::string line = fmt::format("ret {} {}",type,op_1);
            m_code_lines.push_back(line);
            increase();
        }
        break;
    }
    case Operation::CAST:{
        DataType type_op1 = triple->m_op_1.get_type();
        std::string type_to_be_casted = get_data_type_name(type_op1);
        std::string casted_type_str = get_data_type_name(triple->m_cast_type);
        std::string op_1_val = get_operand_value_with_load(triple->m_op_1);
        m_triple_data[triple->m_index].m_number = m_llvm_line_index;
        std::string bitcast = fmt::format("%{} = bitcast {} {} to {}",m_llvm_line_index,type_to_be_casted,op_1_val,casted_type_str);
        m_code_lines.push_back(bitcast);
        increase();
        break;
    }
    case Operation::CALL:
    {
        Function *fn = triple->m_op_1.m_function;
        std::string argument_list = process_argument_list(triple);
        std::string function_method = fn->m_class != nullptr ? fmt::format("{}.{}",fn->m_class->m_name,fn->m_name) : fmt::format("{}",fn->m_name);
        if(fn->m_class==nullptr){

            if(fn->m_return_type!=BasicType::VOID){
                m_code_lines.push_back(fmt::format("%{} = call noundef {} @{}({})",m_llvm_line_index,get_data_type_name(fn->m_return_type),function_method,argument_list));
                increase();
            }
            else
                m_code_lines.push_back(fmt::format("call {} @{}({})",get_data_type_name(fn->m_return_type),function_method,argument_list));

        }else{

            std::string op1_val = get_operand_value_with_load(triple->m_call_args[0]);
            std::string line = fmt::format("%{} = bitcast {} {} to i8*",m_llvm_line_index,get_data_type_name(fn->m_arguments.at(0)->m_type),op1_val);
            m_code_lines.push_back(line);
            increase();

            line = fmt::format("%{} = call noundef i8** @get_vtable(i8* %{})",m_llvm_line_index,m_llvm_line_index-1);
            m_code_lines.push_back(line);
            increase();

            line = fmt::format("%{} = getelementptr inbounds i8*, i8** %{}, i32 {}",m_llvm_line_index,m_llvm_line_index-1,fn->m_vtable_index);
            m_code_lines.push_back(line);
            increase();

            line = fmt::format("%{} = load i8*, i8** %{}, align 16",m_llvm_line_index,m_llvm_line_index-1);
            m_code_lines.push_back(line);
            increase();

            std::string arguments = "(";
            for(size_t i=0;i<fn->m_arguments.size();i++){
                if(i>0)
                    arguments+=",";
                arguments+=get_data_type_name(fn->m_arguments.at(i)->m_type);
            }

            arguments+=")";
            line = fmt::format("%{} = bitcast i8* %{} to {} {}*",m_llvm_line_index,m_llvm_line_index-1,get_data_type_name(fn->m_return_type),arguments);
            m_code_lines.push_back(line);
            increase();
            int function_address = m_llvm_line_index-1;

            if(fn->m_return_type!=BasicType::VOID){
                m_code_lines.push_back(fmt::format("%{} = call noundef {} %{}({})",m_llvm_line_index,get_data_type_name(fn->m_return_type),function_address,argument_list));
                increase();
            }
            else
                m_code_lines.push_back(fmt::format("call {} %{}({})",get_data_type_name(fn->m_return_type),function_address,argument_list));

        }
        m_triple_data[triple->m_index].m_number = m_llvm_line_index-1;
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
        if(type!=BasicType::STRING){
            std::string op_1_val = get_operand_value_with_load(triple->m_op_1);
            std::string op_2_val = get_operand_value_with_load(triple->m_op_2);
            m_triple_data[triple->m_index].m_number = m_llvm_line_index;
            m_code_lines.push_back(fmt::format("%{} = icmp {} {} {}, {}",m_llvm_line_index,operation,get_data_type_name(type),op_1_val,op_2_val));
            increase();
        }else{
            std::string function_name = triple->m_operation == Operation::EQU ? "stringsEqual" : "stringsNotEqual";
            std::string op_1_val = get_operand_value_with_load(triple->m_op_1);
            std::string op_2_val = get_operand_value_with_load(triple->m_op_2);
            m_triple_data[triple->m_index].m_number = m_llvm_line_index;
            std::string line = fmt::format("%{} = call zeroext i1 @{}(i8* noundef {}, i8* noundef {})",m_llvm_line_index,function_name,op_1_val,op_2_val);
            m_code_lines.push_back(line);
            increase();
            break;
        }

        break;
    }
    case Operation::MARKER:
        break;
    case Operation::JT:
    case Operation::JF:
    {
        std::string condition = get_operand_value_with_load(triple->m_op_1);
        int label_false;
        int label_true;
        Triple * next_triple = m_current_fn->m_triples[triple->m_index+1];
        label_true = triple->m_op_2.m_label->m_jump_to->m_basic_block->m_index;
        label_false = next_triple->m_basic_block->m_index;

        if(triple->m_operation==Operation::JF)
            std::swap(label_false,label_true);

        m_code_lines.push_back(fmt::format("br i1 {}, label %Label{}, label %Label{}",condition,label_true,label_false));
        break;
    }
    case Operation::JMP:{
        m_code_lines.push_back(fmt::format("br label %Label{}",triple->m_op_1.m_label->m_jump_to->m_basic_block->m_index));
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
        increase();
        break;
    }
    case Operation::DEC:
    {
        std::string op_1 =get_operand_value_with_load(triple->m_op_1);
        m_code_lines.push_back(fmt::format("%{} = add nsw {} {}, -1",m_llvm_line_index,get_data_type_name(triple->m_op_1.get_type()),op_1));
        std::string from = "%"+std::to_string(m_llvm_line_index);
        std::string to = get_operand_value(triple->m_op_1);
        increase();
        break;
    }
    default:
        break;
    }
}

std::string LLVMCodeGenerator::process_argument(Argument * arg,size_t index){
    std::string type_size = get_data_type_name(arg->m_type);
    if(arg->m_type.category==DataTypeCategory::BASIC){
        switch (arg->m_type.basic_type)
        {
        case BasicType::INT:
            return fmt::format("{} noundef %{}",type_size,index);
        case BasicType::BOOL:
            return fmt::format("{} noundef zeroext %{}",type_size,index);
        case BasicType::STRING:
            return fmt::format("{} noundef %{}",type_size,index);
        default:
            throw std::runtime_error("LLVMCodeGenerator::process_argument(Argument * arg,size_t index)");
        }
    }else{
        std::string type = get_data_type_name(arg->m_type);
        return fmt::format("{} noundef %{}",type,index);
    }
}

void LLVMCodeGenerator::collect_strings_from_function(Function * fn){
    for(const auto &triple : fn->m_triples){
        if(triple->m_op_1.m_category==OperandCategory::CONSTANT && triple->m_op_1.get_type()==BasicType::STRING)
            m_string_literals.push_back(triple->m_op_1.m_constant.get_value_as_string());

        if(triple->m_op_2.m_category==OperandCategory::CONSTANT && triple->m_op_2.get_type()==BasicType::STRING)
            m_string_literals.push_back(triple->m_op_2.m_constant.get_value_as_string());

        for(auto &op : triple->m_call_args){
            if(op.m_category==OperandCategory::CONSTANT && op.get_type()==BasicType::STRING)
                m_string_literals.push_back(op.m_constant.get_value_as_string());
        }
    }

}


void LLVMCodeGenerator::collect_string_literals() {
    for(size_t i=0;i<m_intermediate_program->m_functions.size();i++){
        collect_strings_from_function(m_intermediate_program->m_functions[i]);
    }

    for(size_t i=0;i<m_intermediate_program->m_classes.size();i++){
        for(auto method : m_intermediate_program->m_classes[i]->m_methods)
            collect_strings_from_function(method);
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
        // DO PRZEJRZENIA
        std::string line = fmt::format("@.str.{} = private unnamed_addr constant [{} x i8] c\"\\00\\00\\00\\00\\00{}\", align 1",
            // value,key.size()+4+1+1,str+"\\00");
            value,get_string_length(key),str+"\\00");

        m_code_lines.push_back(line);
    }
}

std::string LLVMCodeGenerator::make_alloca_string(DataType type){

    std::string alloc="";
    if(type.category==DataTypeCategory::BASIC){
        if(type.dimensions>=1){
            alloc = fmt::format("%{} = alloca {}, align 8",m_llvm_line_index,get_data_type_name(type));
        }else{
            switch (type.basic_type)
            {
            case BasicType::INT:
                alloc = fmt::format("%{} = alloca i32, align 4",m_llvm_line_index);
                break;
            case BasicType::BOOL:
                alloc = fmt::format("%{} = alloca i8, align 1",m_llvm_line_index);
                break;
            case BasicType::STRING:
                alloc = fmt::format("%{} = alloca i8*, align 8",m_llvm_line_index);
                break;
            default:
                break;
            }
        }
    }else
        alloc = fmt::format("%{} = alloca {}, align 8",m_llvm_line_index,get_data_type_name(type));

    increase();
    return alloc;
}

std::string LLVMCodeGenerator::make_store_string(Argument *arg,int from,int to){
    std::string store = "";
    if(arg->m_type.category==DataTypeCategory::BASIC){
        if(arg->m_type.dimensions==0){
            switch (arg->m_type.basic_type)
            {
            case BasicType::INT:{
                if(arg->m_type.dimensions>=1)
                    store = fmt::format("store i32* %{}, i32** %{}, align 4",from,to);
                else
                    store = fmt::format("store i32 %{}, i32* %{}, align 4",from,to);
                break;
            }
            case BasicType::BOOL:{
                std::string new_line = fmt::format("%{} = zext i1 %{} to i8",m_llvm_line_index,from);
                m_code_lines.push_back(new_line);
                store = fmt::format("store i8 %{}, i8* %{}, align 1",m_llvm_line_index,to);
                increase();
                break;
            }
            case BasicType::STRING:{
                std::string type_name = get_data_type_name(arg->m_type);
                store = fmt::format("store {} %{}, {}* %{}, align {}",type_name,from,type_name,to,get_align(arg->m_type));
                break;
            }
            default:
                throw std::runtime_error("LLVMCodeGenerator::make_store_string(Argument *arg,int from,int to)");
            }
        }else{
            std::string type = get_data_type_name(arg->m_type);
            store = fmt::format("store {} %{}, {}* %{}, align {}",type,from,type,to,get_align(arg->m_type));
        }
    }else{
        std::string type = get_data_type_name(arg->m_type);
        store = fmt::format("store {} %{}, {}* %{}, align {}",type,from,type,to,get_align(arg->m_type));
    }

    return store;
}

void LLVMCodeGenerator::alloc_arguments_and_variables(const Function *fn){
    size_t index = 0;
    for(auto arg : fn->m_arguments){
        m_argument_data[arg]=ArgumentData{index};
        index++;
    }

    m_llvm_line_index=index;
}

void LLVMCodeGenerator::store_arguments(const Function *fn){
}

void LLVMCodeGenerator::process_function(Function *fn){
    m_current_fn=fn;
    m_triple_to_phi.clear();
    m_string_literal_to_line.clear();
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
    std::string function_method_name;
    if(!fn->m_class)
        function_method_name = fmt::format("@{}",fn->m_name);
    else
        function_method_name = fmt::format("@{}.{}",fn->m_class->m_name,fn->m_name);

    // All methods will be prefixed with their respective class names
    if(fn->m_return_type!=BasicType::VOID)
        fn_dec=fmt::format("define dso_local noundef {} {}({}) {{",get_data_type_name(fn->m_return_type),function_method_name,argument_list);
    else
        fn_dec=fmt::format("define dso_local {} {}({}) {{",get_data_type_name(fn->m_return_type),function_method_name,argument_list);

    m_code_lines.push_back(fn_dec);
    // increase();

    m_llvm_line_index=fn->m_arguments.size();
    // store i alloc teraz sa juz niepotrzebne
    alloc_arguments_and_variables(fn);
    // wnetrze zakomentowane
    // store_arguments(fn);

    if(m_llvm_line_index==fn->m_arguments.size() && fn->m_basic_blocks.size()>0){
        m_code_lines.push_back("StartingBlock:");
        m_code_lines.push_back("br label %Label0");
    }

    if(m_llvm_line_index!=fn->m_arguments.size() && fn->m_basic_blocks.size()>0)
        m_code_lines.push_back("br label %Label0");

    for(auto blk : fn->m_basic_blocks){
        std::string label = fmt::format("Label{}:",blk->m_index);
        m_code_lines.push_back(label);
        for(auto triple : *blk){
            process_triple(triple);
        }

        Triple* last_triple = nullptr;
        last_triple = blk->get_last_triple();
        // jesli operacja rozna od jump to checmy dodac jump do nastepnego bloku pod nami
        if(last_triple){
            Operation op = last_triple->m_operation;
            if(op!=Operation::JT && op!=Operation::JF && op!=Operation::JMP){
                // bo skaczemy do nastepnego bloku
                std::string jmp = fmt::format("br label %Label{}",blk->m_index+1);
                m_code_lines.push_back(jmp);
            }
        }

    }

    if(fn->m_return_type==BasicType::VOID && fn->m_triples.size()==0)
        m_code_lines.push_back("ret void");
    else if(fn->m_return_type==BasicType::VOID && fn->m_triples.back()->m_operation!=Operation::RETURN)
        m_code_lines.push_back("ret void");
    else if(fn->m_return_type==BasicType::INT && fn->m_triples.back()->m_operation!=Operation::RETURN)
        m_code_lines.push_back("ret i32 0");
    else if(fn->m_return_type.category==DataTypeCategory::CLASS && m_code_lines.back().substr(0,5)=="Label")
    {
        std::string type = get_data_type_name(fn->m_return_type);
        std::string line = fmt::format("ret {} null",type);
        m_code_lines.push_back(line);
    }
    else if(fn->m_return_type==BasicType::BOOL && m_code_lines.back().substr(0,5)=="Label")
        m_code_lines.push_back("ret i1 0");

    m_code_lines.push_back("}");

    for(auto &[triple,line_index] : m_triple_to_phi){
        std::string phi_argument_list;
        int i=0;
        for(auto phi_arg : triple->m_phi_arguments){
            if(i>0)
                phi_argument_list+=",";
            phi_argument_list+=process_phi_argument(phi_arg);
            i++;
        }

        std::string line_content = fmt::format("%{} = phi {} {}",m_triple_data.at(triple->m_index).m_number,get_data_type_name(triple->m_op_1.get_type(),false),phi_argument_list);
        m_code_lines.at(line_index)=line_content;
    }
}

void LLVMCodeGenerator::add_used_predefined_functions(){
    m_code_lines.push_back("declare dso_local noundef i8* @addStrings(i8* noundef %0, i8* noundef %1)");
    m_code_lines.push_back("declare dso_local void @printInt(i32 noundef %0)");
    m_code_lines.push_back("declare dso_local void @printString(i8* noundef %0)");
    m_code_lines.push_back("declare dso_local noundef i8* @readString()");
    m_code_lines.push_back("declare dso_local noundef i32 @readInt()");
    m_code_lines.push_back("declare dso_local void @increaseStringCounter(i8* noundef %0)");
    m_code_lines.push_back("declare dso_local void @decreaseStringCounter(i8* noundef %0)");
    m_code_lines.push_back("declare dso_local i32 @getArrayLength(i8* noundef %0)");
    m_code_lines.push_back("declare dso_local i8* @allocArray(i32 noundef %0, i32 noundef %1)");
    m_code_lines.push_back("declare dso_local zeroext i1 @isConstant(i8* noundef %0)");
    m_code_lines.push_back("declare dso_local i32 @getReferenceCount(i8* noundef %0)");
    m_code_lines.push_back("declare dso_local zeroext i1 @stringsEqual(i8* noundef %0, i8* noundef %1)");
    m_code_lines.push_back("declare dso_local zeroext i1 @stringsNotEqual(i8* noundef %0, i8* noundef %1)");
    m_code_lines.push_back("declare dso_local i8* @allocateInstance(i8** noundef %0, i32 noundef %1)");
    m_code_lines.push_back("declare dso_local i8* @getField(i8* noundef %0, i32 noundef %1)");
    m_code_lines.push_back("declare dso_local i8** @get_vtable(i8* noundef %0)");
    m_code_lines.push_back("declare dso_local void @error()");
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

std::string LLVMCodeGenerator::get_all_data_types(MyClass *cl){
    std::string types="";

    for(size_t i=0;i<cl->m_fields.size();i++){
        if(i>0)
            types+=",";
        std::string type = get_data_type_name(cl->m_fields[i]->m_type,true);
        types+=type;
    }

    return types;
}


void LLVMCodeGenerator::add_classes_declarations(){
    std::string class_decl;
    for(auto c : m_intermediate_program->m_classes){
        std::string all_data_types = get_all_data_types(c);
        if(c->m_base_class!=nullptr){
            class_decl = all_data_types!="" ?
            fmt::format("%class.{} = type {{%class.{},{}}}",c->m_name,c->m_base_class->m_name,all_data_types) :
            fmt::format("%class.{} = type {{%class.{}}}",c->m_name,c->m_base_class->m_name);
            m_code_lines.push_back(class_decl);
        }else{
            class_decl = fmt::format("%class.{} = type {{{}}}",c->m_name,all_data_types);
            m_code_lines.push_back(class_decl);
        }
    }
}

std::string LLVMCodeGenerator::generate_llvm_function_to_vtable(Function *fn){
    std::string arguments = "(";

    for(size_t i=0;i<fn->m_arguments.size();i++){
        if(i>0)
            arguments+=",";
        arguments+=get_data_type_name(fn->m_arguments.at(i)->m_type);
    }

    arguments+=")";
    std::string fn_str = fmt::format("i8* bitcast ({} {}* @{}.{} to i8*)", get_data_type_name(fn->m_return_type),arguments,fn->m_class->m_name,fn->m_name);

    return fn_str;
}

std::string LLVMCodeGenerator::generate_llvm_vtable(MyClass * cl){

    std::string functions = "[";

    for(size_t i=0; i < cl->m_vtable.size();i++){
        if(i>0)
            functions+=",";
        functions+= generate_llvm_function_to_vtable(cl->m_vtable[i]);
    }
    functions+="]";
    std::string vtab = fmt::format("@.vtable.{} = dso_local global [{} x i8*] {}, align 16",cl->m_name,cl->m_vtable.size(),functions);

    return vtab;
}

void LLVMCodeGenerator::add_virtual_tables(){
    for(auto *cl : m_intermediate_program->m_classes){
        std::string vt = generate_llvm_vtable(cl);
        m_code_lines.push_back(vt);
    }
}

std::string LLVMCodeGenerator::process_program(){
    add_classes_declarations();
    collect_string_literals();
    map_string_literals();
    generate_string_literal_declarations();
    add_virtual_tables();

    for(auto &fn : m_intermediate_program->m_functions)
    {
        if(fn->m_type==PredefinedFunction::USERDEFINED)
            process_function(fn);
    }

    for(auto &cl : m_intermediate_program->m_classes){
        for(auto &meth : cl->m_methods )
            process_function(meth);
    }

    add_used_predefined_functions();

    std::string program_ll;
    for(size_t i =0;i<m_code_lines.size();i++)
        program_ll+=m_code_lines[i]+"\n";

    return program_ll;
}


void LLVMCodeGenerator::print_generated_code() {
    for(auto line:m_code_lines)
        std::cout<<line<<std::endl;
}
