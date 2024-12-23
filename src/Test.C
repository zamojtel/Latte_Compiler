#include "Libraries.h"
#include <cstdio>
#include <string>
#include <map>
#include <iostream>
#include "Parser.H"
#include "Printer.H"
#include "Absyn.H"
#include "ParserError.H"
#include "DataStructure.h"
#include "IntermediateProgram.h"
#include "IntermediateProgram.cpp"
#include "LLVMCodeGenerator.h"
#include "LLVMCodeGenerator.cpp"
#include "Skeleton.H"
#include "Skeleton.C"
#include "Logger.h"
#include "IRCoder.h"
#include "IRCoderListener.h"

void usage() {
  printf("usage: Call with one of the following argument combinations:\n");
  printf("\t--help\t\tDisplay this help message.\n");
  printf("\t(no arguments)\tParse stdin verbosely.\n");
  printf("\t(files)\t\tParse content of files verbosely.\n");
  printf("\t-s (files)\tSilent mode. Parse content of files silently.\n");
}

// class IntermediateProgram{
// private:
// public:
//   std::vector<Function*>m_functions;
//   IntermediateProgram(){}
//   bool check_all_returns(Function *fn,size_t start);
//   bool find_path_without_return(Function *fn,size_t start);
//   ~IntermediateProgram(){
//     for(auto *fn: m_functions)
//       delete fn;
//   }
// };


// // this function checks if we can reach the end of the function without finding any return on the way
// bool IntermediateProgram::check_all_returns(Function *fn,size_t start){
//   if(start >= fn->m_triples.size() || fn->m_triples[start]->m_visited){
//     return false;
//   }

//   fn->m_triples[start]->m_visited=true;

//   for(size_t i=start;i<fn->m_triples.size();i++){
//     if(fn->m_triples[i]->m_operation==Operation::RETURN){
//       return true;
//     }

//     if(fn->m_triples[i]->m_operation==Operation::JF || fn->m_triples[i]->m_operation==Operation::JT || fn->m_triples[i]->m_operation==Operation::JMP){
//       bool jump_possible;
//       bool no_jump_possible;
      
//       if(fn->m_triples[i]->m_operation==Operation::JMP){
//         jump_possible=true;
//         no_jump_possible=false;
//       }else{
//         switch (fn->m_triples[i]->m_op_1.m_category)
//         {
//         case OperandCategory::CONSTANT:
//         {
//           jump_possible = (fn->m_triples[i]->m_op_1.m_constant.u.boolean == false && fn->m_triples[i]->m_operation==Operation::JF)
//           || (fn->m_triples[i]->m_op_1.m_constant.u.boolean && fn->m_triples[i]->m_operation==Operation::JT);
//           no_jump_possible = !jump_possible;
//           break;
//         }
//         case OperandCategory::VARIABLE:
//         case OperandCategory::ARGUMENT:
//         case OperandCategory::TRIPLE:
//         {
//           jump_possible=true;
//           no_jump_possible=true;
//           break;
//         }
//         default:
//           break;
//         }
//       }

//       // here we'll jump or not
//       if(jump_possible){
//         // its enough if one path has return at the end
//         size_t index;
//         if(fn->m_triples[i]->m_operation==Operation::JMP)
//           index = fn->m_triples[i]->m_op_1.m_label->m_jump_to->m_index;
//         else
//           index = fn->m_triples[i]->m_op_2.m_label->m_jump_to->m_index;

//         bool result = check_all_returns(fn,index);
//         if(result)
//           return true;
//       }
    
//       if(no_jump_possible)
//         continue;
//       else
//         break;
//     }
//   }
//   return false;
// }

// bool IntermediateProgram::find_path_without_return(Function *fn,size_t start){
//   if(start >= fn->m_triples.size()){
//     return true;
//   }

//   if(fn->m_triples[start]->m_visited){
//     return false;
//   }
  
//   for(size_t i=start;i<fn->m_triples.size();i++){
//     fn->m_triples[start]->m_visited=true;

//     if(fn->m_triples[i]->m_operation==Operation::RETURN)
//       return false;

//     if(fn->m_triples[i]->m_operation==Operation::JF || fn->m_triples[i]->m_operation==Operation::JT || fn->m_triples[i]->m_operation==Operation::JMP){
//       bool jump_possible;
//       bool no_jump_possible;
      
//       if(fn->m_triples[i]->m_operation==Operation::JMP){
//         jump_possible=true;
//         no_jump_possible=false;
//       }else{
//         switch (fn->m_triples[i]->m_op_1.m_category)
//         {
//         case OperandCategory::CONSTANT:
//         {
//           jump_possible = (fn->m_triples[i]->m_op_1.m_constant.u.boolean == false && fn->m_triples[i]->m_operation==Operation::JF)
//           || (fn->m_triples[i]->m_op_1.m_constant.u.boolean && fn->m_triples[i]->m_operation==Operation::JT);
//           no_jump_possible = !jump_possible;
//           break;
//         }
//         case OperandCategory::VARIABLE:
//         case OperandCategory::ARGUMENT:
//         case OperandCategory::TRIPLE:
//         {
//           jump_possible=true;
//           no_jump_possible=true;
//           break;
//         }
//         default:
//           break;
//         }
//       }

//       if(jump_possible){
//         size_t index;
//         if(fn->m_triples[i]->m_operation==Operation::JMP)
//           index = fn->m_triples[i]->m_op_1.m_label->m_jump_to->m_index;
//         else
//           index = fn->m_triples[i]->m_op_2.m_label->m_jump_to->m_index;

//         bool result = find_path_without_return(fn,index);
//         if(result){
//           return true;
//         }
//       }

//       if(no_jump_possible){
//         continue;
//       }else{
//         return false;
//       }
//     }
//   }

//   return true;
// }

class ErrorList {
public:
  std::vector<Error*> m_errors;
  void add_error(int l,const std::string &msg){
    m_errors.push_back(new Error{(size_t)l,msg});
  }

  void print_errors(){
    for(auto *err : m_errors){
      std::string error =  err->m_line !=0 ? fmt::format("Line {}: {}",err->m_line,err->m_msg) : fmt::format("{}",err->m_msg);
      std::cout<<error<<std::endl;
    }
  }

  bool errors_occured(){return m_errors.size()>0;}

  ~ErrorList(){
    for(auto *err : m_errors)
      delete err;
  }
};

class TrueFalseLabel{
public:
  Label *m_false_label;
  Label *m_true_label;
  TrueFalseLabel(Label *f_label,Label * t_label):m_false_label{f_label},m_true_label{t_label}{}
  TrueFalseLabel():m_false_label{nullptr},m_true_label{nullptr}{}
};

class MyVisitor:public Skeleton,public IRCoderListener {
private:
  size_t m_pass_number;
  IntermediateProgram * m_program;
  std::vector<std::string> m_generated_code;
  SymbolTable m_symbol_table;
  std::map<Visitable*,Operand> m_nodes_to_operands;
  std::vector<TrueFalseLabel> m_true_false_label_stack;
  std::vector<std::string> m_string_literals;
  Function * m_current_fn;
  Argument * m_current_arg;
  DataType m_last_visited_type;
  OperandCategory m_last_expr_cat;
  Operation m_current_operation;
  IRCoder m_IRCoder;
public:
  ErrorList m_error_list;

  void ircoder_error(int line,const std::string &msg) override {
    m_error_list.add_error(line,msg);
  }

  void create_variable(const std::string &name , Expr *initializer,int line){

    const SymbolTableEntry * entry = m_symbol_table.get_entry_at_top(name);
    if(entry)
    {
      std::string msg = fmt::format("Variable {} already defined",name);
      m_error_list.add_error(line,msg);
      return ;
    }

    Variable *variable = new Variable{name,m_last_visited_type};
    if(initializer!=nullptr){
      initializer->accept(this);
      LOG_DEBUG("Initialized Identifier: {}",name);
      
      m_nodes_to_operands[initializer] = m_IRCoder.push(line,Operation::ASSIGN,variable,m_nodes_to_operands.at(initializer));
    }

    m_current_fn->m_variables.push_back(variable);
    m_symbol_table.add(name,variable);
  }

  void pass(Program *parse_tree){
    m_pass_number=1;
    
    add_predefined_functions(); 
    parse_tree->accept(this);
    m_pass_number++;

    m_symbol_table.push();
    for(size_t i=0;i<m_program->m_functions.size();i++){
      m_symbol_table.add(m_program->m_functions[i]->m_name,m_program->m_functions[i]);
    }
   
    parse_tree->accept(this);
    m_symbol_table.pop();
  }

  bool errors_occured(){
    return m_error_list.errors_occured();
  }
  void print_errors(){
    m_error_list.print_errors();
  }

  void add_predefined_functions(){
    Function * print_int = new Function{PredefinedFunction::PRINTINT};
    print_int->m_name = "printInt";
    print_int->m_return_type = DataType::VOID;
    print_int->m_arguments.push_back(new Argument{DataType::INT,"value"});

    Function * print_string = new Function{PredefinedFunction::PRINTSTRING};
    print_string->m_name = "printString";
    print_string->m_return_type=DataType::VOID;
    print_string->m_arguments.push_back(new Argument{DataType::STRING,"value"});

    Function * error = new Function{PredefinedFunction::ERROR};
    error->m_name = "error";
    error->m_return_type=DataType::VOID;

    Function * read_int = new Function{PredefinedFunction::READINT};
    read_int->m_name = "readInt";
    read_int->m_return_type = DataType::INT;

    Function * read_string = new Function{PredefinedFunction::READSTRING};
    read_string->m_name="readString";
    read_string->m_return_type=DataType::STRING;


    m_program->m_functions.push_back(print_int);
    m_program->m_functions.push_back(print_string);
    m_program->m_functions.push_back(error);
    m_program->m_functions.push_back(read_int);
    m_program->m_functions.push_back(read_string);

  }

  MyVisitor(IntermediateProgram *prog):m_program{prog}{
    m_IRCoder.set_listener(this);
  }

  void visitProg(Prog *p) override {
    LOG_DEBUG("Begin Program");
    p->listtopdef_->accept(this);
    LOG_DEBUG("End Program");
  }
  
  // we'll process them multiple times 
  void visitFnDef(FnDef *p) override {
    if(m_pass_number==1){
      m_current_fn = new Function;
      LOG_DEBUG("Begin function definition");
      LOG_DEBUG("Function name: {}",p->ident_);
      m_current_fn->m_name=p->ident_;
      p->type_->accept(this);
      m_current_fn->m_return_type=m_last_visited_type;
      p->listarg_->accept(this);
      m_program->m_functions.push_back(m_current_fn);
      LOG_DEBUG("End function definition");
    }else if(m_pass_number==2){
     
      m_current_fn = m_symbol_table.get_function(p->ident_);
      m_IRCoder.set_function(m_current_fn);
      m_symbol_table.push();

      for(size_t i=0;i<m_current_fn->m_arguments.size();i++)
        m_symbol_table.add(m_current_fn->m_arguments[i]->m_identifier,m_current_fn->m_arguments[i]);
    
      p->block_->accept(this);
      m_symbol_table.pop();
    }
  }

  void visitEApp(EApp *p) override{
    Function * fn = m_symbol_table.get_function(p->ident_);
    if(fn==nullptr){
      std::string msg=fmt::format("Undefined function {}",p->ident_); 
      m_error_list.add_error(p->line_number,msg);

      m_nodes_to_operands[p] = Operand::error();
      return;
    }
      
    if(fn->m_arguments.size() != p->listexpr_->size()){
      std::string msg=fmt::format("Incorrect number of arguments, expected: {}, passed {}",fn->m_arguments.size(),p->listexpr_->size());
      m_error_list.add_error(p->line_number,msg);

      m_nodes_to_operands[p] = Operand::error();
      return;
    }
    
    if(p->listexpr_!=nullptr)
      p->listexpr_->accept(this);
    
    for(size_t i=0;i<fn->m_arguments.size();i++){
      DataType fn_argument_type = fn->m_arguments[i]->m_type;
      DataType current_argument_type = m_nodes_to_operands[p->listexpr_->at(i)].get_type();
      int line_number = p->listexpr_->at(i)->line_number;
      // to-do implicit conversion

      if(fn_argument_type!=current_argument_type){
        if(current_argument_type!=DataType::ERROR)
        {
          std::string msg=fmt::format("Mismatched argument type {}, expected type {}, provided {}",i+1,data_type_to_string(fn_argument_type),data_type_to_string(current_argument_type));
          m_error_list.add_error(line_number,msg);
        }
      }
    }

    m_current_fn->m_used=true;
    m_nodes_to_operands[p] = m_IRCoder.push(p->line_number,Operation::CALL,m_symbol_table.get_function(p->ident_),Constant{(int)p->listexpr_->size()});

    for(auto *expr: *p->listexpr_)
      m_nodes_to_operands[p].m_triple->m_call_args.push_back(m_nodes_to_operands.at(expr));
  }
 
  void visitBlk(Blk *p){
    m_symbol_table.push();
    for(auto statement : *p->liststmt_){
      statement->accept(this);
    }
    m_symbol_table.pop();
  }

  void visitInt(Int *x) override{
    m_last_visited_type = DataType::INT;
  }

  void visitVoid(Void *p) override{
    m_last_visited_type = DataType::VOID;
  }

  void visitBool(Bool *p) override{
    m_last_visited_type=DataType::BOOL;
  }

  void visitStr(Str *p) override {
    m_last_visited_type=DataType::STRING; 
  }

  void visitListArg(ListArg *p) override{
    LOG_DEBUG("List of arguments");
    for(auto item : *p){
      item->accept(this);
    }

  }

  void visitAr(Ar *p) override{

    if(m_current_fn->contains_argument(p->ident_)){
      std::string msg = fmt::format("Argument with name {} already defined",p->ident_); 
      m_error_list.add_error(p->line_number,msg);
      return;
    }

    m_current_arg=new Argument;
    LOG_DEBUG("Argument Identifier: {}",p->ident_);
    m_current_arg->m_identifier=p->ident_;
    p->type_->accept(this);
    m_current_arg->m_type=m_last_visited_type;
    m_current_fn->m_arguments.push_back(m_current_arg);
  }

  void visitDecl(Decl *p) override{
    p->type_->accept(this);
    for(auto item : *p->listitem_)
      item->accept(this);
  }
  
  void visitInit(Init *p) override {
    create_variable(p->ident_,p->expr_,p->line_number);
  }
  
  void bind_label(Label *label,Triple *triple){
    label->m_jump_to=triple;
    triple->m_pointing_labels.push_back(label);
  }

  // corrected donzo
  void visitCond(Cond *p) override {
    p->expr_->accept(this);
    Label *jump_to = create_label();
    Triple *jf_triple = m_IRCoder.push(p->line_number,Operation::JF,m_nodes_to_operands.at(p->expr_),{jump_to});
    
    p->stmt_->accept(this);
    Triple *special_triple=m_IRCoder.push(p->line_number,Operation::MARKER);
    // we bind label
    // jf_triple->m_op_2.m_label->m_jump_to = special_triple;
    bind_label(jf_triple->m_op_2.m_label,special_triple);
  }

  // corrected
  void visitCondElse(CondElse *p) override{
    p->expr_->accept(this);
    // 2 labels
    Label *jump_to_else = create_label();
    Label *jump_out_if = create_label();

    Triple *jf_triple = m_IRCoder.push(0,Operation::JF,m_nodes_to_operands.at(p->expr_),{jump_to_else});
    p->stmt_1->accept(this);
    Triple *jmp_triple = m_IRCoder.push(0,Operation::JMP,{jump_out_if});
    Triple* special_marker_before_else =m_IRCoder.push(0,Operation::MARKER);
    p->stmt_2->accept(this);

    // Triple *jmp_out_of_else= m_IRCoder.push(0,Operation::JMP,{jump_out_if});
    Triple* special_marker_end_of_if =m_IRCoder.push(0,Operation::MARKER);

    // 2 binds to do
    // jf_triple->m_op_2.m_label->m_jump_to=special_marker_before_else;
    bind_label(jf_triple->m_op_2.m_label,special_marker_before_else);
  
    // bind_label(jmp_out_of_else->m_op_1.m_label,special_marker_end_of_if);
    // jmp_triple->m_op_1.m_label->m_jump_to=special_marker_end_of_if;
    bind_label(jmp_triple->m_op_1.m_label,special_marker_end_of_if);
  }

  void visitNot(Not *p) override {
    p->expr_->accept(this);
    m_nodes_to_operands[p] = m_IRCoder.push(p->line_number,Operation::NOT,m_nodes_to_operands.at(p->expr_));
  }

  void visitNeg(Neg *p) override {
    p->expr_->accept(this);
    m_nodes_to_operands[p] = m_IRCoder.push(p->line_number,Operation::NEG,m_nodes_to_operands.at(p->expr_));
  }

  // while donzo
  void visitWhile(While *p) override {
    Label * cond_label = create_label();
    Triple * cond_marker = m_IRCoder.push(0,Operation::MARKER);
    // bind label
    // cond_label->m_jump_to=cond_marker;
    bind_label(cond_label,cond_marker);

    p->expr_->accept(this);
    Label *end_label = create_label();
    m_IRCoder.push(0,Operation::JF,m_nodes_to_operands.at(p->expr_),{end_label});

    p->stmt_->accept(this);
    m_IRCoder.push(0,Operation::JMP,{cond_label});
    Triple *end_marker = m_IRCoder.push(0,Operation::MARKER);
    
    // end_label->m_jump_to = end_marker;
    bind_label(end_label,end_marker);
  }

  void visitAss(Ass *ass) override {
    ass->expr_->accept(this);
    const SymbolTableEntry *table_entry = m_symbol_table.get_entry(ass->ident_);

    if(!table_entry){
      std::string msg = fmt::format("Undefined variable | {}",ass->ident_);
      m_error_list.add_error(ass->line_number,msg);
      return;
    }
    
    Operand op_1;
    if(table_entry->m_category==SymbolTableCategory::ARGUMENT)
      op_1 = table_entry->m_argument;
    else if(table_entry->m_category==SymbolTableCategory::VARIABLE)
      op_1 = table_entry->m_variable;
    else{
      std::string msg = fmt::format("Invalid assignment | {}",ass->ident_);
      m_error_list.add_error(ass->line_number,msg);
      return;
    }

    Operand op_ = m_nodes_to_operands.at(ass->expr_);
    m_nodes_to_operands[ass->expr_] = m_IRCoder.push(ass->line_number,Operation::ASSIGN,op_1,m_nodes_to_operands.at(ass->expr_)); 

  }

  void visitNoInit(NoInit *p) override {
    create_variable(p->ident_,nullptr,p->line_number);
  }

  void visitRet(Ret *p) override{
    p->expr_->accept(this);
    m_IRCoder.push(p->line_number,Operation::RETURN,m_nodes_to_operands[p->expr_]);
  }

  void visitVRet(VRet *p) override{ 
    m_IRCoder.push(p->line_number,Operation::RETURN);
  }

  void visitELitInt(ELitInt *p) override{
    Constant constant{p->integer_};
    m_nodes_to_operands[p]=constant;
  }

  void visitELitTrue(ELitTrue *p) override {
    m_nodes_to_operands[p]= {true};
  }

  void visitELitFalse(ELitFalse *p) override{
    m_nodes_to_operands[p]= {false};
  }
  
  void visitEString(EString *p) override {
    m_nodes_to_operands[p]= {p->string_};
  }

  DataType deduce_bool_type_one_argument(DataType op_1_type){
    if(op_1_type==DataType::BOOL)
      return DataType::BOOL;
    else
      return DataType::ERROR;
  }

  bool equal_data_types_or_error(DataType t1,DataType t2){
    return  t1 == DataType::ERROR || t2 == DataType::ERROR || t1==t2;
  }

  Label* create_label(){
    Label *label = new Label{m_current_fn->m_labels.size()};
    m_current_fn->add_label(label);
    
    return label;
  }

  void visitEVar(EVar *p)override{
    const SymbolTableEntry *entry = m_symbol_table.get_entry(p->ident_);

    if(!entry){
      std::string msg=fmt::format("Undefined identifier {}",p->ident_); 
      m_error_list.add_error(p->line_number,msg);
      return;
    }
    switch (entry->m_category)
    {
    case SymbolTableCategory::VARIABLE:
      m_nodes_to_operands[p] = entry->m_variable; 
      return;
    case SymbolTableCategory::ARGUMENT:
      m_nodes_to_operands[p] = entry->m_argument;
      return;
    default:
      break;
    }
  }

  void visitEAdd(EAdd *p) override {
    p->expr_1->accept(this);
    p->expr_2->accept(this);
    p->addop_->accept(this);

    // m_IRCoder.push
    m_nodes_to_operands[p] = m_IRCoder.push(p->line_number,m_current_operation,m_nodes_to_operands.at(p->expr_1),m_nodes_to_operands.at(p->expr_2));
  }

  void visitEMul(EMul *p) override{
    // the order of operations is really important 
    p->expr_1->accept(this);
    p->expr_2->accept(this);
    p->mulop_->accept(this);
    
    m_nodes_to_operands[p] = m_IRCoder.push(0,m_current_operation,m_nodes_to_operands.at(p->expr_1),m_nodes_to_operands.at(p->expr_2));
  }

  void visitLTH(LTH *p) override {
    m_current_operation = Operation::LTH;
  }

  void visitLE(LE *p) override {
    m_current_operation = Operation::LE;
  }

  void visitGTH(GTH *p) override {
    m_current_operation = Operation::GTH;
  }

  void visitGE(GE *p) override {
    m_current_operation = Operation::GE;
  }

  void visitEQU(EQU *p) override{
    m_current_operation = Operation::EQU;
  }

  void visitNE(NE *p) override{
    m_current_operation=Operation::NE;
  }

  // visitERel bez short circuit
  void visitERel(ERel *p) override {
    // here the order is important 
    p->expr_1->accept(this);
    p->expr_2->accept(this);
    p->relop_->accept(this);

    Operand op_1 = m_nodes_to_operands.at(p->expr_1);
    Operand op_2 = m_nodes_to_operands.at(p->expr_2);
    
    m_nodes_to_operands[p]=m_IRCoder.push(p->line_number,m_current_operation,m_nodes_to_operands.at(p->expr_1),m_nodes_to_operands.at(p->expr_2));
  }

  void visitEAnd(EAnd *p) override{
    p->expr_1->accept(this);
    p->expr_2->accept(this);
    
    m_nodes_to_operands[p]=m_IRCoder.push(p->line_number,Operation::AND,m_nodes_to_operands.at(p->expr_1),m_nodes_to_operands.at(p->expr_2));
  }

  void visitEOr(EOr *p){
    p->expr_1->accept(this);
    p->expr_2->accept(this);
    
    m_nodes_to_operands[p]=m_IRCoder.push(p->line_number,Operation::OR,m_nodes_to_operands.at(p->expr_1),m_nodes_to_operands.at(p->expr_2));
  }
  
  void visitPlus(Plus *p) override {
    m_current_operation=Operation::ADD;
  }

  void visitDiv(Div *p) override{
    m_current_operation=Operation::DIV;
  }
  
  void visitMinus(Minus *p) override{
    m_current_operation=Operation::SUB;
  }

  void visitTimes(Times *p) override {
    m_current_operation=Operation::MUL;
  }

  void visitMod(Mod *mod) override {
    m_current_operation=Operation::MOD;
  }

  void print_constant(const Constant &constant){
    switch (constant.m_type)
    {
    case DataType::INT:
      std::cout<<"INT("<<constant.u.integer<<") ";
      break;
    case DataType::BOOL:
      std::cout<<"BOOL("<<constant.u.boolean<<")";
      break;
    case DataType::STRING:
      std::cout<<"STRING("<<constant.u.str<<")";
      break;
    default:
      break;
    }
  }
  
  void print_operand(const Operand &operand){
    switch (operand.m_category)
      {
      case OperandCategory::CONSTANT:
        print_constant(operand.m_constant);
        break;
      case OperandCategory::VARIABLE:
        std::cout<<operand.m_var->m_ident<<" ";
        break;
      case OperandCategory::TRIPLE:
        std::cout<<"t"<<operand.m_triple->m_index<<" ";
        break;
      case OperandCategory::EMPTY:
        std::cout<<"- ";
        break;
      case OperandCategory::LABEL:
        if(operand.m_label->m_jump_to!=nullptr)
          std::cout<<"t"<<operand.m_label->m_jump_to->m_index<<" ";
        else
          std::cout<<"No label set ";
        break;
      case OperandCategory::FUNCTION:
        std::cout<<operand.m_function->m_name<<" ";
        break;
      case OperandCategory::ARGUMENT:
        std::cout<<operand.m_argument->m_identifier<<" ";
        break;
      case OperandCategory::ERROR:
        std::cout<<"error ";
        break;
      default:
        throw 0;
        break;
    }
  }
};

int main(int argc, char ** argv)
{
  FILE *input;
  int quiet = 0;
  char *filename = NULL;

  if (argc > 1) {
    if (strcmp(argv[1], "-s") == 0) {
      quiet = 1;
      if (argc > 2) {
        filename = argv[2];
      } else {
        input = stdin;
      }
    } else {
      filename = argv[1];
    }
  }

  if (filename) {
    input = fopen(filename, "r");
    if (!input) {
      usage();
      exit(1);
    }
  } else input = stdin;
 
  Program *parse_tree = NULL;
  try {
  parse_tree = pProgram(input);
  } catch( parse_error &e) {
     std::cerr << "Parse error on line " << e.getLine() << "\n";
  }

  fclose(input);
  if(!parse_tree)
    return 1;

  std::vector<Function*> functions;
  IntermediateProgram int_program;


  MyVisitor my_visitor{&int_program};
  LLVMCodeGenerator llvm_generator{&int_program};
  
  my_visitor.pass(parse_tree);
  
  for(auto *fn : int_program.m_functions){
    if(fn->m_return_type==DataType::VOID)
      continue;

    if(fn->m_type==PredefinedFunction::USERDEFINED && int_program.find_path_without_return(fn,0)){
        std::string msg = fmt::format("Not all possible paths return values in a function called: {}",fn->m_name);
        my_visitor.m_error_list.add_error(0,msg);
    }
  }

  if(my_visitor.errors_occured()){
    std::cout<<"ERROR"<<std::endl;
    my_visitor.print_errors();
    delete(parse_tree);
    return 1;
  }else{
    // TODO uncomment latter
    // std::cout<<"OK"<<std::endl;
  }
  
  // IntermediateProgramPrinter ipp;

  // ipp.print_program(int_program);

  llvm_generator.process_program();
  llvm_generator.print_generated_code();
  
  quiet=true;
  if (parse_tree)
  {
    if (!quiet) {
      printf("\n[Abstract Syntax]\n");
      ShowAbsyn *s = new ShowAbsyn();
      printf("%s\n\n", s->show(parse_tree));
      printf("[Linearized Tree]\n");
      PrintAbsyn *p = new PrintAbsyn();
      printf("%s\n\n", p->print(parse_tree));
    }
    delete(parse_tree);
    return 0;
  }

  return 1;
}

