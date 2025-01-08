
#ifndef IRCODER_H
#define IRCODER_H

#include <vector>
#include <string>
#include "DataStructure.h"
#include <map>
#include "IRCoderListener.h"

class IRCoder{
public:
    size_t m_position;
    IRCoderListener *m_listener = nullptr;
    Function * m_current_fn=nullptr;
    Triple* push(int line_number,Operation operation,const Operand &op_1={},const Operand &op_2={});
    void set_listener(IRCoderListener *listener);
    void set_function(Function *fn);
    void set_position_after(Function *fn,Triple *triple);
    void set_position_before(Function *fn,Triple *triple);
    void check_triple(Triple * triple);
    DataType deduce_type(Triple *triple);
    DataType deduce_bool_type_one_argument(DataType op_1_type);
    DataType deduce_bool_type(DataType op_1_type,DataType op_2_type);
    DataType deduce_arithmetic_type_one_argument(DataType op_1_type);
    DataType deduce_arithmetic_type(Operation operation,DataType op_1_type,DataType op_2_type);
    bool equal_data_types_or_error(DataType t1,DataType t2);
    std::string operation_to_string(Operation operation);
    // std::string type_to_string(DataType dt);
    IRCoder(Function *fn, size_t position):m_position{position},m_current_fn{fn}{}
    IRCoder():m_position{0}{}
};

#endif