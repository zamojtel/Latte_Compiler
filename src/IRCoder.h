
#ifndef IRCODER_H
#define IRCODER_H

#include <vector>
#include <string>
#include "DataStructure.h"
#include "IntermediateProgram.h"
#include <map>
#include "IRCoderListener.h"
#include <stdexcept>

class IRCoder{
private:
    bool m_debug=false;
    enum class TypeCompatibility{
        COMPATIBLE,INCOMPATIBLE,CAST_LEFT_TO_RIGHT,CAST_RIGHT_TO_LEFT
    };
public:
    size_t m_position;
    IRCoderListener *m_listener = nullptr;
    Function * m_current_fn=nullptr;
    BasicBlock * m_current_blk=nullptr;
    Triple* push(int line_number,Operation operation,const Operand &op_1={},const Operand &op_2={});
    Triple* push_no_check(int line_number,Operation operation,const Operand &op_1={},const Operand &op_2={});
    void set_listener(IRCoderListener *listener);
    void set_function(Function *fn);
    void set_basic_block(BasicBlock * blk);
    void set_position_after(Function *fn,Triple *triple);
    void set_position_before(Function *fn,Triple *triple);
    void set_position_before(Triple * triple );
    void set_position_after(Triple * triple);
    void check_triple(Triple * triple);
    void analyze_triple(Triple *triple);
    void analyze_array_accesses(Triple * triple);
    void insert_cast_and_replace_op(Triple *triple,Operand &op, DataType type);
    DataType deduce_type(Triple *triple);
    DataType deduce_bool_type_one_argument(DataType op_1_type);
    DataType deduce_bool_type_for_equality(DataType op_1_type,DataType op_2_type);
    DataType deduce_bool_type_for_inequality_operators(DataType op_1_type,DataType op_2_type);
    DataType deduce_arithmetic_type_one_argument(DataType op_1_type);
    DataType deduce_arithmetic_type(Operation operation,DataType op_1_type,DataType op_2_type);
    TypeCompatibility equal_data_types_or_error(DataType t1,DataType t2);
    std::string operation_to_string(Operation operation);
    IRCoder(Function *fn, size_t position):m_position{position},m_current_fn{fn}{}
    IRCoder():m_position{0}{}
};

#endif
