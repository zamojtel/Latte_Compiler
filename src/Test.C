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
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <cctype>
#include "LiveAnalyzer.h"

class TrueFalseLabel{
public:
  Label *m_false_label;
  Label *m_true_label;
  TrueFalseLabel(Label *f_label,Label * t_label):m_false_label{f_label},m_true_label{t_label}{}
  TrueFalseLabel():m_false_label{nullptr},m_true_label{nullptr}{}
};

class CheckSingleDeclarationVisitor:public Skeleton{
private:
  bool m_is_single_decl_stmt=false;
public:
  bool is_single_decl(){
    return m_is_single_decl_stmt;
  }

  void visitDecl(Decl *p) override {
    m_is_single_decl_stmt=true;
  }

  void visitBStmt(BStmt *b_stmt) override {}
};

class MyVisitor:public Skeleton,public IRCoderListener {
private:
  DataType m_new_variable_type;
  size_t m_special_variable_index;
  size_t m_pass_number;
  IntermediateProgram * m_program;
  std::vector<std::string> m_generated_code;
  SymbolTable m_symbol_table;
  std::map<Visitable*,Operand> m_nodes_to_operands;
  std::vector<TrueFalseLabel> m_true_false_label_stack;
  std::vector<std::string> m_string_literals;
  MyClass * m_current_class;
  Function * m_current_fn;
  Argument * m_current_arg;
  DataType m_last_visited_type;
  OperandCategory m_last_expr_cat;
  Operation m_current_operation;
  IRCoder m_IRCoder;
public:
  ErrorList m_error_list;

  bool contains_special_symbols(std::string &name){
    for (char c : name) {
      if (!std::isalnum(c) && c!='_')
        return true;
    }

    return false;
  }

  void visitNewArray(NewArray *p) override {
    p->type_->accept(this);
    DataType type = m_last_visited_type;
    p->expr_->accept(this);

    Triple * triple = m_IRCoder.push_no_check(p->line_number,Operation::NEW_ARRAY,m_nodes_to_operands.at(p->expr_));
    triple->m_data_type_for_new = type;
    m_IRCoder.analyze_triple(triple);

    m_nodes_to_operands[p] = triple;
  }

  void visitFor(For *p){
    m_symbol_table.push();

    Variable * index_var = create_variable(BasicType::INT,"i.i",nullptr,p->line_number);
    p->type_->accept(this);

    Variable * x = create_variable(m_last_visited_type,p->ident_,nullptr,p->line_number);
    p->expr_->accept(this);
    Triple * arr_length = m_IRCoder.push(0,Operation::ARRAY_LENGTH,m_nodes_to_operands.at(p->expr_));

    Label * cond_label = create_label();
    bind_label(cond_label,m_IRCoder.push(0,Operation::MARKER));

    Label *end_label = create_label();

    Triple *condition = m_IRCoder.push(0,Operation::LTH,index_var,arr_length);
    m_IRCoder.push(0,Operation::JF,condition,{end_label});

    Triple * arr_elem = m_IRCoder.push(0,Operation::ACCESS_ARRAY,m_nodes_to_operands.at(p->expr_),index_var);
    m_IRCoder.push(0,Operation::ASSIGN,x,arr_elem);

    p->stmt_->accept(this);

    Triple *triple = m_IRCoder.push(0,Operation::ADD,index_var,Constant{1});
    m_IRCoder.push(0,Operation::ASSIGN,index_var,triple);

    m_IRCoder.push(0,Operation::JMP,{cond_label});

    Triple *after_for = m_IRCoder.push(0,Operation::MARKER);

    bind_label(end_label,after_for);

    m_symbol_table.pop();
  }

  void visitArray(Array *p) override {
    p->type_->accept(this);

    if(m_last_visited_type==BasicType::VOID)
    {
      std::string msg = fmt::format("Can't define arrays of type void");
      m_error_list.add_error(p->line_number,msg);

      m_last_visited_type=BasicType::ERROR;
      return;
    }

    if(m_last_visited_type!=BasicType::ERROR)
      m_last_visited_type.dimensions++;
  }

  void visitArrAcc(ArrAcc *p) override{
    p->expr_1->accept(this);
    p->expr_2->accept(this);

    m_nodes_to_operands[p] = m_IRCoder.push(p->line_number,Operation::ACCESS_ARRAY,m_nodes_to_operands.at(p->expr_1),m_nodes_to_operands.at(p->expr_2));
  }

  void visitClass(Class *p) override{
    if(!m_program->has_class(p->ident_)){
      MyClass * new_class = new MyClass{p->ident_,m_program};
      m_program->m_classes.push_back(new_class);
      new_class->m_implicit_declaration_line = p->line_number;
    }

    m_last_visited_type = m_program->get_class(p->ident_);
  }

  void visitMemberAcc(MemberAcc *p){
    p->expr_->accept(this);
    Operand op_1 = m_nodes_to_operands.at(p->expr_);
    DataType type = op_1.get_type();

    if(type.dimensions>=1){
      if(p->ident_!="length"){
        std::string msg = fmt::format("Invalid member access {}",p->ident_);
        m_error_list.add_error(p->line_number,msg);
        return;
      }
      m_nodes_to_operands[p] = m_IRCoder.push(p->line_number,Operation::ARRAY_LENGTH,op_1);
    }else{
      if(type.category==DataTypeCategory::CLASS){
        Field *field = type.class_type->get_field_considering_inheritance(p->ident_);
        if(!field){
          std::string msg = fmt::format("Invalid member access {} on class {}",p->ident_,type.class_type->m_name);
          m_error_list.add_error(p->line_number,msg);
          return;
        }else
          m_nodes_to_operands[p] = m_IRCoder.push(p->line_number,Operation::MEMEBER_ACCESS,op_1,field);

      }else{
        std::string msg = fmt::format("Basic type does not have member {}",p->ident_);
        m_error_list.add_error(p->line_number,msg);
        return;
      }
    }
  }

  void ircoder_error(int line,const std::string &msg) override {
    m_error_list.add_error(line,msg);
  }

  // bool check_predifined_function(const std::string &name){
  //   if(
  //     name == "printInt" || name == "printString" ||
  //     name == "readInt"  || name == "readString"  ||
  //     name == "error"    || name == "error"
  //     ){
  //       return true;
  //     }

  //   return false;
  // }

  Variable* create_special_boolean_variable(){
    Variable *variable = new Variable{fmt::format("${}",m_special_variable_index),BasicType::BOOL};
    m_special_variable_index++;
    m_current_fn->m_variables.push_back(variable);
    return variable;
  }

  Variable* create_variable(DataType type,const std::string &name ,Expr *initializer,int line){
    const SymbolTableEntry * entry = m_symbol_table.get_entry_at_top(name);
    if(entry)
    {
      std::string msg = fmt::format("Variable {} already defined",name);
      m_error_list.add_error(line,msg);
      return nullptr;
    }

    if(type==BasicType::VOID){
      m_error_list.add_error(line,"Variable can't be of type void");
      return nullptr;
    }

    Variable *variable = new Variable{name,type};
    if(initializer!=nullptr){
      initializer->accept(this);
      LOG_DEBUG("Initialized Identifier: {}",name);

      m_nodes_to_operands[initializer] = m_IRCoder.push(line,Operation::INIT,variable,m_nodes_to_operands.at(initializer));
    }else{
      if(variable->m_type.dimensions==0){
        if(variable->m_type.category==DataTypeCategory::BASIC){
          switch (variable->m_type.basic_type)
          {
          case BasicType::STRING:
            m_IRCoder.push(line,Operation::INIT,variable,Constant{nullptr});
            break;
          case BasicType::INT:
            m_IRCoder.push(line,Operation::INIT,variable,Constant{0});
            break;
          case BasicType::BOOL:
            m_IRCoder.push(line,Operation::INIT,variable,Constant{false});
            break;
          case BasicType::ERROR:
            break;
          default:
            throw std::runtime_error("create_variable(const std::string &name , Expr *initializer,int line)");
          }
        }else
          m_IRCoder.push(line,Operation::INIT,variable,Constant{nullptr});
      }else
        m_IRCoder.push(line,Operation::INIT,variable,Constant{nullptr});

    }

    m_current_fn->m_variables.push_back(variable);
    m_symbol_table.add(name,variable);

    return variable;
  }

  bool is_single_declaration(Stmt *stmt){
    CheckSingleDeclarationVisitor decl_visitor;
    stmt->accept(&decl_visitor);
    return decl_visitor.is_single_decl();
  }

  void pass(Program *parse_tree){
    m_pass_number=1;

    add_predefined_functions();
    parse_tree->accept(this);
    if(!m_program->has_function("main"))
      m_error_list.add_error(parse_tree->line_number,"The 'main' function is required but not found.");
    else
    {
      auto *main = m_program->get_function("main");
      if(main->m_return_type != BasicType::INT || main->m_arguments.size()>0)
        m_error_list.add_error(parse_tree->line_number,"Invalid 'main' function definition.");
    }

    if(errors_occured())
      return;

    m_pass_number++;
    m_symbol_table.push();

    for(size_t i=0;i<m_program->m_functions.size();i++){
      auto fn = m_program->m_functions[i];
      m_symbol_table.add(fn->m_name,fn);
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
    Function * print_int = new Function{m_program,PredefinedFunction::PRINTINT};
    print_int->m_name = "printInt";
    print_int->m_return_type = BasicType::VOID;
    print_int->m_arguments.push_back(new Argument{BasicType::INT,"value"});

    Function * print_string = new Function{m_program,PredefinedFunction::PRINTSTRING};
    print_string->m_name = "printString";
    print_string->m_return_type=BasicType::VOID;
    print_string->m_arguments.push_back(new Argument{BasicType::STRING,"value"});

    Function * error = new Function{m_program,PredefinedFunction::ERROR};
    error->m_name = "error";
    error->m_return_type=BasicType::VOID;

    Function * read_int = new Function{m_program,PredefinedFunction::READINT};
    read_int->m_name = "readInt";
    read_int->m_return_type = BasicType::INT;

    Function * read_string = new Function{m_program,PredefinedFunction::READSTRING};
    read_string->m_name="readString";
    read_string->m_return_type=BasicType::STRING;

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

  void visitFnDef(FnDef *p) override {
    if(m_pass_number==1){
      LOG_DEBUG("Begin function definition");
      LOG_DEBUG("Function name: {}",p->ident_);

      if(contains_special_symbols(p->ident_)){
        m_error_list.add_error(p->line_number,fmt::format("The function name {} can't contain special sybmols",p->ident_));
        return;
      }

      if(m_program->has_function(p->ident_)){
        auto *fn = m_program->get_function(p->ident_);

        if(fn->m_type!=PredefinedFunction::USERDEFINED)
          m_error_list.add_error(p->line_number,fmt::format("The predefined function '{}' cannot be overridden.",p->ident_));
        else
          m_error_list.add_error(p->line_number,fmt::format("Function {} already defined",p->ident_));

        return;
      }

      m_current_fn = new Function{m_program};
      m_current_fn->m_name=p->ident_;
      p->type_->accept(this);
      m_current_fn->m_return_type=m_last_visited_type;
      p->listarg_->accept(this);
      m_program->m_functions.push_back(m_current_fn);
      LOG_DEBUG("End function definition");
    }else if(m_pass_number==2){
      m_current_fn = m_symbol_table.get_function(p->ident_);
      m_special_variable_index=0;
      m_IRCoder.set_function(m_current_fn);
      m_symbol_table.push();

      for(size_t i=0;i<m_current_fn->m_arguments.size();i++)
          m_symbol_table.add(m_current_fn->m_arguments[i]->m_identifier,m_current_fn->m_arguments[i]);

      p->block_->accept(this);
      m_symbol_table.pop();
    }
  }

  void processClassDef(int line_number,const std::string &cl_name,ListClassDecl *listclassdecl_){
    MyClass *cl_obj = m_program->get_class(cl_name);

    if(cl_obj){
      if(cl_obj->m_defined){
        m_error_list.add_error(line_number,fmt::format("The class with name '{}' already defined.",cl_name));
        return;
      }
    }else{
      cl_obj = new MyClass{cl_name,m_program};
      m_program->m_classes.push_back(cl_obj);
    }

    cl_obj->m_defined=true;
    m_current_class=cl_obj;
    listclassdecl_->accept(this);
  }

  void visitClassDef(ClassDef *p) override {
    if(m_pass_number==1){
      processClassDef(p->line_number,p->ident_,p->listclassdecl_);
    }else if(m_pass_number==2){
      m_current_class = m_program->get_class(p->ident_);
      m_symbol_table.push();
      for(auto *field : m_current_class->m_fields)
        m_symbol_table.add(field->m_name,field);

      for(auto *method : m_current_class->m_methods)
        m_symbol_table.add(method->m_name,method);
      p->listclassdecl_->accept(this);
      m_symbol_table.pop();
    }
  }

  void visitClassExtDef(ClassExtDef *p) override{
    if(m_pass_number==1){
      processClassDef(p->line_number,p->ident_1,p->listclassdecl_);
    }else if(m_pass_number==2){

      m_current_class = m_program->get_class(p->ident_1);
      m_current_class->m_base_class = m_program->get_class(p->ident_2);

      // Cycle detection in an inheritence line
      if(m_program->get_class(p->ident_2)==nullptr){
        m_error_list.add_error(p->line_number,fmt::format("Class '{}' does not exits",p->ident_2));
        return;
      }

      if(m_current_class==m_current_class->m_base_class){
        m_error_list.add_error(p->line_number,fmt::format("Class '{}' can't inherit from ",p->ident_1));
        return;
      }

      if(m_current_class->m_base_class!= nullptr && m_current_class==m_current_class->m_base_class->m_base_class){
        m_error_list.add_error(p->line_number,fmt::format("Recursive inheritence '{}' can't inherit from itself ",p->ident_1));
        return;
      }

      m_symbol_table.push();
      MyClass *cl = m_current_class;
      std::vector<MyClass*> cl_stack;
      cl_stack.push_back(cl);

      while(cl->m_base_class!=nullptr){
        cl_stack.push_back(cl->m_base_class);
        cl=cl->m_base_class;
      }

      while(!cl_stack.empty()){
        auto base_cl = cl_stack.back();
        cl_stack.pop_back();
        for(auto * method : base_cl->m_methods)
          m_symbol_table.add_override(method->m_name,method);

        for(auto * field : base_cl->m_fields)
          m_symbol_table.add_override(field->m_name,field);
      }

      p->listclassdecl_->accept(this);
      m_symbol_table.pop();

    }
  }

  void visitMethodDef(MethodDef *p) override {
    if(m_pass_number==1){
      if(contains_special_symbols(p->ident_)){
        m_error_list.add_error(p->line_number,fmt::format("The method name {} can't contain special sybmols",p->ident_));
        return;
      }

      if(m_current_class->has_method(p->ident_)){
        m_error_list.add_error(p->line_number,fmt::format("The method with name '{}' has been already defined in the class {}.",p->ident_,m_current_class->m_name));
        return;
      }

      m_current_fn = new Function{m_program};
      m_current_fn->m_name=p->ident_;
      m_current_fn->m_class=m_current_class;
      m_current_fn->m_decl_line=p->line_number;

      p->type_->accept(this);
      m_current_fn->m_return_type=m_last_visited_type;
      m_current_fn->m_arguments.push_back(new Argument{m_current_class,"self"});
      p->listarg_->accept(this);
      m_current_class->add_method(m_current_fn);
    }else if(m_pass_number==2){

      Function * method = m_current_class->get_method(p->ident_);
      m_IRCoder.set_function(method);
      m_current_fn=method;
      m_symbol_table.push();

      for(size_t i=0;i<method->m_arguments.size();i++)
        m_symbol_table.add(method->m_arguments[i]->m_identifier,method->m_arguments[i]);

      p->block_->accept(this);
      m_symbol_table.pop();
      m_current_fn = nullptr;
    }
  }

  void visitEMethod(EMethod *p) override {
    p->expr_->accept(this);
    DataType type = m_nodes_to_operands.at(p->expr_).get_type();

    if(type.category!=DataTypeCategory::CLASS){
      m_error_list.add_error(p->line_number,fmt::format("Basic types cant be acces with dot operator",p->ident_));
      return;
    }

    Function * fn = type.class_type->get_method_considering_inheritance(p->ident_);

    if(!fn){
      m_error_list.add_error(p->line_number,fmt::format("Class {} has no method {}",type.class_type->m_name,p->ident_));
      return;
    }

    p->listexpr_->accept(this);
    m_nodes_to_operands[p] = m_IRCoder.push_no_check(p->line_number,Operation::CALL,fn);

    m_nodes_to_operands[p].m_triple->m_call_args.push_back(m_nodes_to_operands.at(p->expr_));


    for(auto *expr: *p->listexpr_){
      Operand op = m_nodes_to_operands.at(expr);
      m_nodes_to_operands[p].m_triple->m_call_args.push_back(op);
    }

    m_IRCoder.analyze_triple(m_nodes_to_operands.at(p).m_triple);
  }

  void visitFieldDef(FieldDef *p) override{
    if(m_pass_number==1){
      p->type_->accept(this);
      if(m_current_class->get_field(p->ident_)){
        m_error_list.add_error(p->line_number,fmt::format("The field with name '{}' already defined.",p->ident_));
        return;
      }
      m_current_class->add_field(m_last_visited_type,p->ident_);
    }
  }

  void visitEClass(EClass *p) override{

    if(!m_program->has_class(p->ident_)){
      m_error_list.add_error(p->line_number,fmt::format("The class with name {} does not exist",p->ident_));
      return;
    }

    Triple * triple = m_IRCoder.push_no_check(p->line_number,Operation::NEW_INSTANCE);
    triple->m_data_type_for_new = m_program->get_class(p->ident_);
    m_IRCoder.analyze_triple(triple);

    m_nodes_to_operands[p] = triple;
  }


  void visitEApp(EApp *p) override{
    Function * fn = m_symbol_table.get_function(p->ident_);
    if(fn==nullptr){
      std::string msg=fmt::format("Undefined function {}",p->ident_);
      m_error_list.add_error(p->line_number,msg);

      m_nodes_to_operands[p] = Operand::error();
      return;
    }

    if(p->listexpr_!=nullptr)
      p->listexpr_->accept(this);

    m_current_fn->m_used=true;

    m_nodes_to_operands[p] = m_IRCoder.push_no_check(p->line_number,Operation::CALL,m_symbol_table.get_function(p->ident_));

    if(fn->m_class)
      m_nodes_to_operands.at(p).m_triple->m_call_args.push_back(m_current_fn->m_arguments[0]);

    for(auto *expr: *p->listexpr_){
      Operand op = m_nodes_to_operands.at(expr);
      m_nodes_to_operands[p].m_triple->m_call_args.push_back(op);
    }

    m_IRCoder.analyze_triple(m_nodes_to_operands.at(p).m_triple);
  }

  void visitBlk(Blk *p){
    m_symbol_table.push();
    for(auto statement : *p->liststmt_){
      statement->accept(this);
    }
    m_symbol_table.pop();
  }

  void visitInt(Int *x) override{
    m_last_visited_type = BasicType::INT;
  }

  void visitVoid(Void *p) override{
    m_last_visited_type = BasicType::VOID;
  }

  void visitBool(Bool *p) override{
    m_last_visited_type=BasicType::BOOL;
  }

  void visitStr(Str *p) override {
    m_last_visited_type=BasicType::STRING;
  }

  void visitListArg(ListArg *p) override{
    LOG_DEBUG("List of arguments");
    for(auto item : *p)
      item->accept(this);
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

    if(m_last_visited_type==BasicType::VOID){
      m_error_list.add_error(p->line_number,"Arguments can't be of type void");
      return;
    }

    m_current_arg->m_type=m_last_visited_type;
    m_current_fn->m_arguments.push_back(m_current_arg);
  }

  void visitDecl(Decl *p) override{
    p->type_->accept(this);
    m_new_variable_type = m_last_visited_type;

    for(auto item : *p->listitem_)
      item->accept(this);

  }

  void visitInit(Init *p) override {
    create_variable(m_new_variable_type,p->ident_,p->expr_,p->line_number);
  }

  void bind_label(Label *label,Triple *triple){
    label->m_jump_to=triple;
    triple->m_pointing_labels.push_back(label);
  }

  void visitCond(Cond *p) override {
    p->expr_->accept(this);

    Label *jump_to = create_label();
    Triple *jf_triple = m_IRCoder.push(p->line_number,Operation::JF,m_nodes_to_operands.at(p->expr_),{jump_to});

    if(is_single_declaration(p->stmt_)){
      std::string msg = fmt::format("Single unused declaration inside if");
      m_error_list.add_error(p->stmt_->line_number,msg);
    }

    p->stmt_->accept(this);


    Triple *special_triple=m_IRCoder.push(p->line_number,Operation::MARKER);
    bind_label(jf_triple->m_op_2.m_label,special_triple);
  }

  void visitCondElse(CondElse *p) override{
    p->expr_->accept(this);

    Label *jump_to_else = create_label();
    Label *jump_out_if = create_label();

    if(is_single_declaration(p->stmt_1)){
      std::string msg = fmt::format("Single unused declaration inside if");
      m_error_list.add_error(p->stmt_1->line_number,msg);
    }

    if(is_single_declaration(p->stmt_2)){
      std::string msg = fmt::format("Single unused declaration inside else");
      m_error_list.add_error(p->stmt_2->line_number,msg);
    }

    Triple *jf_triple = m_IRCoder.push(0,Operation::JF,m_nodes_to_operands.at(p->expr_),{jump_to_else});
    p->stmt_1->accept(this);
    Triple *jmp_triple = m_IRCoder.push(0,Operation::JMP,{jump_out_if});
    Triple* special_marker_before_else =m_IRCoder.push(0,Operation::MARKER);
    p->stmt_2->accept(this);

    Triple* special_marker_end_of_if =m_IRCoder.push(0,Operation::MARKER);

    bind_label(jf_triple->m_op_2.m_label,special_marker_before_else);
    bind_label(jmp_triple->m_op_1.m_label,special_marker_end_of_if);
  }

  void visitNeg(Neg *p) override {
    p->expr_->accept(this);
    m_nodes_to_operands[p] = m_IRCoder.push(p->line_number,Operation::NEG,m_nodes_to_operands.at(p->expr_));
  }

  void visitIncr(Incr *incr){
    incr->expr_->accept(this);

    Operand op = m_nodes_to_operands.at(incr->expr_);

    if(op.m_category==OperandCategory::TRIPLE){
      Triple *copy = m_IRCoder.push(incr->line_number,op.m_triple->m_operation,op.m_triple->m_op_1,op.m_triple->m_op_2);
      Triple *triple = m_IRCoder.push(incr->line_number,Operation::ADD,copy,Constant{1});
      m_IRCoder.push(incr->line_number,Operation::ASSIGN,op,triple);
    }else{
      Triple *triple = m_IRCoder.push(incr->line_number,Operation::ADD,op,Constant{1});
      m_IRCoder.push(incr->line_number,Operation::ASSIGN,op,triple);
    }
  }

  void visitDecr(Decr *decr){
    decr->expr_->accept(this);

    Operand op = m_nodes_to_operands.at(decr->expr_);

    if(op.m_category==OperandCategory::TRIPLE){
      Triple *copy = m_IRCoder.push(decr->line_number,op.m_triple->m_operation,op.m_triple->m_op_1,op.m_triple->m_op_2);
      Triple *triple = m_IRCoder.push(decr->line_number,Operation::ADD,copy,Constant{-1});
      m_IRCoder.push(decr->line_number,Operation::ASSIGN,op,triple);
    }else{
      Triple *triple = m_IRCoder.push(decr->line_number,Operation::ADD,op,Constant{-1});
      m_IRCoder.push(decr->line_number,Operation::ASSIGN,op,triple);
    }
  }

  void visitWhile(While *p) override {
    Label * cond_label = create_label();
    Triple * cond_marker = m_IRCoder.push(0,Operation::MARKER);

    bind_label(cond_label,cond_marker);

    p->expr_->accept(this);
    Label *end_label = create_label();
    m_IRCoder.push(0,Operation::JF,m_nodes_to_operands.at(p->expr_),{end_label});

    if(is_single_declaration(p->stmt_)){
      std::string msg = fmt::format("Single unused declaration inside while");
      m_error_list.add_error(p->stmt_->line_number,msg);
    }

    p->stmt_->accept(this);
    m_IRCoder.push(0,Operation::JMP,{cond_label});
    Triple *end_marker = m_IRCoder.push(0,Operation::MARKER);

    bind_label(end_label,end_marker);
  }

  void visitAss(Ass *ass) override {
    ass->expr_1->accept(this);
    ass->expr_2->accept(this);

    if(m_error_list.errors_occured())
      return;

    Operand op_1 = m_nodes_to_operands.at(ass->expr_1);
    Operand op_2 = m_nodes_to_operands.at(ass->expr_2);

    m_nodes_to_operands[ass->expr_1] = m_IRCoder.push(ass->line_number,Operation::ASSIGN,op_1,op_2);
  }

  void visitNoInit(NoInit *p) override {
    create_variable(m_new_variable_type,p->ident_,nullptr,p->line_number);
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

  void visitELitNull(ELitNull *p) override {
    m_nodes_to_operands[p] = Constant{nullptr};
  }

  void visitENull(ENull *p) override {
    m_nodes_to_operands[p] = Constant{nullptr};
  }

  DataType deduce_bool_type_one_argument(DataType op_1_type){
    if(op_1_type==BasicType::BOOL)
      return BasicType::BOOL;
    else
      return BasicType::ERROR;
  }

  bool equal_data_types_or_error(DataType t1,DataType t2){
    return  t1 == BasicType::ERROR || t2 == BasicType::ERROR || t1==t2;
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
    case SymbolTableCategory::FIELD:{
      Triple * triple = m_IRCoder.push(p->line_number,Operation::MEMEBER_ACCESS,m_IRCoder.m_current_fn->m_arguments[0],entry->m_field);
      m_nodes_to_operands[p] = triple;
      return;
    }
    default:
      break;
    }
  }

  void visitEAdd(EAdd *p) override {
    p->expr_1->accept(this);
    p->expr_2->accept(this);
    p->addop_->accept(this);
    auto r1 = m_nodes_to_operands.at(p->expr_1);
    auto r2 = m_nodes_to_operands.at(p->expr_2);

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

  void visitERel(ERel *p) override {
    // the order is important
    p->expr_1->accept(this);
    p->expr_2->accept(this);
    p->relop_->accept(this);

    Operand op_1 = m_nodes_to_operands.at(p->expr_1);
    Operand op_2 = m_nodes_to_operands.at(p->expr_2);

    m_nodes_to_operands[p]=m_IRCoder.push(p->line_number,m_current_operation,m_nodes_to_operands.at(p->expr_1),m_nodes_to_operands.at(p->expr_2));
  }

  void visitEAnd(EAnd *p) override{
    Label* label_1 = create_label();
    Label* label_2 = create_label();
    Variable* boolean_1 = create_special_boolean_variable();
    p->expr_1->accept(this);

    m_IRCoder.push(p->line_number,Operation::JF,m_nodes_to_operands.at(p->expr_1),label_1);
    p->expr_2->accept(this);
    m_IRCoder.push(p->line_number,Operation::ASSIGN,boolean_1,m_nodes_to_operands.at(p->expr_2));

    m_IRCoder.push(p->line_number,Operation::JMP,label_2);
    Triple * marker_1 = m_IRCoder.push(p->line_number,Operation::MARKER);

    m_IRCoder.push(p->line_number,Operation::ASSIGN,boolean_1,{false});

    Triple * marker_2 = m_IRCoder.push(p->line_number,Operation::MARKER);

    bind_label(label_1,marker_1);
    bind_label(label_2,marker_2);

    m_nodes_to_operands[p] = boolean_1;
  }

  void visitEOr(EOr *p){
    Label *label_1 = create_label();
    Label *label_2 = create_label();

    Variable *boolean_1 = create_special_boolean_variable();

    p->expr_1->accept(this);
    m_IRCoder.push(p->line_number,Operation::JT,m_nodes_to_operands.at(p->expr_1),label_1);
    p->expr_2->accept(this);
    m_IRCoder.push(p->line_number,Operation::ASSIGN,boolean_1,m_nodes_to_operands.at(p->expr_2));
    m_IRCoder.push(p->line_number,Operation::JMP,label_2);
    Triple * marker_1 = m_IRCoder.push(p->line_number,Operation::MARKER);
    m_IRCoder.push(p->line_number,Operation::ASSIGN,boolean_1,{true});
    Triple * marker_2 = m_IRCoder.push(p->line_number,Operation::MARKER);

    bind_label(label_1,marker_1);
    bind_label(label_2,marker_2);

    m_nodes_to_operands[p] = boolean_1;
  }

  void visitNot(Not *p) override {
    Label * label_1 = create_label();
    Label * label_2 = create_label();
    p->expr_->accept(this);

    Variable * boolean_var = create_special_boolean_variable();
    m_IRCoder.push(p->line_number,Operation::JF,m_nodes_to_operands.at(p->expr_),label_1);
    m_IRCoder.push(p->line_number,Operation::ASSIGN,boolean_var,{false});
    m_IRCoder.push(p->line_number,Operation::JMP,label_2);
    Triple * marker_1 = m_IRCoder.push(p->line_number,Operation::MARKER);
    m_IRCoder.push(p->line_number,Operation::ASSIGN,boolean_var,{true});
    Triple * marker_2 = m_IRCoder.push(p->line_number,Operation::MARKER);

    bind_label(label_1,marker_1);
    bind_label(label_2,marker_2);
    m_nodes_to_operands[p] = boolean_var;
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
    case BasicType::INT:
      std::cout<<"INT("<<constant.u.integer<<") ";
      break;
    case BasicType::BOOL:
      std::cout<<"BOOL("<<constant.u.boolean<<")";
      break;
    case BasicType::STRING:
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
        throw std::runtime_error("print_operand(const Operand &operand)");
    }
  }
};

int main(int argc, char ** argv)
{
  FILE *input;
  char *filename = NULL;

  if (argc > 1) {
    if (strcmp(argv[1], "-s") == 0) {
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
      exit(1);
    }
  } else input = stdin;

  std::string path_to_file = argv[1];
  path_to_file = path_to_file.substr(0,path_to_file.length()-4); // to remove a file extension

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
  int_program.create_virtual_tables();

  // IntermediateProgramPrinter int_program_printer;
  int_program.create_basic_blocks();
  // int_program_printer.print_program(int_program);
  LiveAnalyzer analyzer{&int_program};
  analyzer.run();
  // analyzer.print_live_variables();
  int_program.analyze_array_accesses();
  int_program.analyze_member_accesses();
  int_program.optimize();
  int_program.substitute_all_vars_and_args();

  // std::cout<<"-----------------------"<<std::endl;
  // std::cout<<"After substituting: "<<std::endl;
  // std::cout<<"-----------------------"<<std::endl;
  // int_program_printer.print_program(int_program);

  for(auto *fn : int_program.m_functions){
    if(fn->m_return_type==BasicType::VOID)
      continue;

    if(fn->m_type==PredefinedFunction::USERDEFINED && !my_visitor.errors_occured() && int_program.find_path_without_return(fn,0)){
        std::string msg = fmt::format("Not all possible paths return values in a function called: {}",fn->m_name);
        my_visitor.m_error_list.add_error(0,msg);
    }
  }

  int_program.check_all_methods(my_visitor.m_error_list);

  if(my_visitor.errors_occured()){
    std::cout<<"ERROR"<<std::endl;
    my_visitor.print_errors();
    delete(parse_tree);
    return 1;
  }else{
    std::cout<<"OK"<<std::endl;

    std::string program_ll = llvm_generator.process_program();
    std::ofstream myfile_llvm;
    std::string path_to_baz_file = path_to_file+".ll";
    myfile_llvm.open(path_to_baz_file);
    myfile_llvm << program_ll;
    myfile_llvm.close();

    // preparing files
    std::string file_name = std::filesystem::path(path_to_file).filename();
    std::string path_to_intermediate_file = path_to_file+"_intermediate.bc";
    std::string run_llvm = "llvm-as-14 "+path_to_baz_file+" -o "+path_to_intermediate_file;
    std::string output_file_bc = path_to_file+".bc";
    system(run_llvm.data());

    // loading lib
    std::string cwd = std::filesystem::current_path();
    std::string lib = fmt::format("{}/lib/runtime.bc",cwd);

    // linking
    std::string llvm_link = fmt::format("llvm-link-14 {} {} -o {}",path_to_intermediate_file,lib,output_file_bc);
    system(llvm_link.data());

    return 0;
  }

  return 1;
}
