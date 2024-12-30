
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

class FunctionData{
public:
    
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
    LLVMCodeGenerator(IntermediateProgram *i_p):m_intermediate_program{i_p}{}
    // add template 
    // finish processing return statement 
    // Generate code for printf() SS
};


// TODO move to another file
// std::string printInt_dec= R"abc(
// @dnl = internal constant [4 x i8] c"%d\0A\00"
// define void @printInt(i32 %x) {
//     %t0 = getelementptr [4 x i8], [4 x i8]* @dnl, i32 0, i32 0
//     call i32 (i8*, ...) @printf(i8* %t0, i32 %x) 
//     ret void
// })abc";

// OK moved
// std::string printInt_dec= R"abc(
// @.format_PrintInt = internal constant [4 x i8] c"%d\0A\00"
// define dso_local void @printInt(i32 noundef %0) {
//   %2 = alloca i32, align 4
//   store i32 %0, ptr %2, align 4
//   %3 = load i32, ptr %2, align 4
//   %4 = call i32 (ptr, ...) @printf(ptr noundef @.format_PrintInt, i32 noundef %3)
//   ret void
// })abc";

// OK moved to functions.ll
// std::string readInt_dec = R"abc(
// @.format_readInt = private unnamed_addr constant [6 x i8] c"%d%*c\00", align 1
// define dso_local noundef i32 @readInt() {
//   %1 = alloca i32, align 4
//   %2 = call i32 (ptr, ...) @scanf(ptr noundef @.format_readInt, ptr noundef %1)
//   %3 = load i32, ptr %1, align 4
//   ret i32 %3
// })abc";

// we dont use it anymore OK
// std::string printString_dec = R"abc(
// declare i32 @puts(i8*)
// define void @printString(i8* %s) {
// entry:  call i32 @puts(i8* %s)
// 	ret void
// }
// )abc";

// std::string readInt_dec = R"abc(
// @d = internal constant [3 x i8] c"%d\00"
// define i32 @readInt() {
// entry:	%res = alloca i32
//     %t1 = getelementptr [3 x i8], [3 x i8]* @d, i32 0, i32 0
// 	call i32 (i8*, ...) @scanf(i8* %t1, i32* %res)
// 	%t2 = load i32, i32* %res
// 	ret i32 %t2
// })abc";

// we have concanate function - OK
// std::string addString_dec = R"abc(
// define dso_local noundef ptr @addStrings(ptr noundef %0, ptr noundef %1) {
//   %3 = alloca ptr, align 8
//   %4 = alloca ptr, align 8
//   %5 = alloca i32, align 4
//   %6 = alloca i32, align 4
//   %7 = alloca i32, align 4
//   %8 = alloca ptr, align 8
//   store ptr %0, ptr %3, align 8
//   store ptr %1, ptr %4, align 8
//   %9 = load ptr, ptr %3, align 8
//   %10 = call i64 @strlen(ptr noundef %9) #7
//   %11 = trunc i64 %10 to i32
//   store i32 %11, ptr %5, align 4
//   %12 = load ptr, ptr %4, align 8
//   %13 = call i64 @strlen(ptr noundef %12) #7
//   %14 = trunc i64 %13 to i32
//   store i32 %14, ptr %6, align 4
//   %15 = load i32, ptr %5, align 4
//   %16 = load i32, ptr %6, align 4
//   %17 = add nsw i32 %15, %16
//   %18 = add nsw i32 %17, 1
//   store i32 %18, ptr %7, align 4
//   %19 = load i32, ptr %7, align 4
//   %20 = sext i32 %19 to i64
//   %21 = mul i64 %20, 1
//   %22 = call noalias ptr @malloc(i64 noundef %21) #8
//   store ptr %22, ptr %8, align 8
//   %23 = load ptr, ptr %8, align 8
//   %24 = load ptr, ptr %3, align 8
//   %25 = load i32, ptr %5, align 4
//   %26 = sext i32 %25 to i64
//   call void @llvm.memcpy.p0.p0.i64(ptr align 1 %23, ptr align 1 %24, i64 %26, i1 false)
//   %27 = load ptr, ptr %8, align 8
//   %28 = load i32, ptr %5, align 4
//   %29 = sext i32 %28 to i64
//   %30 = getelementptr inbounds i8, ptr %27, i64 %29
//   %31 = load ptr, ptr %4, align 8
//   %32 = load i32, ptr %6, align 4
//   %33 = add nsw i32 %32, 1
//   %34 = sext i32 %33 to i64
//   call void @llvm.memcpy.p0.p0.i64(ptr align 1 %30, ptr align 1 %31, i64 %34, i1 false)
//   %35 = load ptr, ptr %8, align 8
//   ret ptr %35
// }

// ; Function Attrs: nounwind willreturn memory(read)
// declare i64 @strlen(ptr noundef) #1

// ; Function Attrs: nounwind allocsize(0)
// declare noalias ptr @malloc(i64 noundef) #2

// ; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
// declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #3
// )abc";

// we have our own -OK
// std::string readString_dec = R"abc(
// declare i64 @getline(ptr noundef, ptr noundef, ptr noundef) #1
// @stdin = external global ptr, align 8

// define dso_local noundef ptr @readString() {
//   %1 = alloca ptr, align 8
//   %2 = alloca i64, align 8
//   %3 = alloca i64, align 8
//   store ptr null, ptr %1, align 8
//   %4 = load ptr, ptr @stdin, align 8
//   %5 = call i64 @getline(ptr noundef %1, ptr noundef %2, ptr noundef %4)
//   store i64 %5, ptr %3, align 8
//   %6 = load i64, ptr %3, align 8
//   %7 = icmp ugt i64 %6, 0
//   br i1 %7, label %8, label %21

// 8:                                                ; preds = %0
//   %9 = load ptr, ptr %1, align 8
//   %10 = load i64, ptr %3, align 8
//   %11 = sub i64 %10, 1
//   %12 = getelementptr inbounds i8, ptr %9, i64 %11
//   %13 = load i8, ptr %12, align 1
//   %14 = sext i8 %13 to i32
//   %15 = icmp eq i32 %14, 10
//   br i1 %15, label %16, label %21

// 16:                                               ; preds = %8
//   %17 = load ptr, ptr %1, align 8
//   %18 = load i64, ptr %3, align 8
//   %19 = sub i64 %18, 1
//   %20 = getelementptr inbounds i8, ptr %17, i64 %19
//   store i8 0, ptr %20, align 1
//   br label %21

// 21:                                               ; preds = %16, %8, %0
//   %22 = load ptr, ptr %1, align 8
//   ret ptr %22
// })abc";

#endif 