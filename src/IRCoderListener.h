
#ifndef IRCODERLISTENER_H
#define IRCODERLISTENER_H

#include <vector>
#include <string>
#include "DataStructure.h"
#include <map>

class IRCoderListener{
public:
    virtual void ircoder_error(int line,const std::string &msg)=0;
    virtual ~IRCoderListener()=default;
};

#endif
