
#include "Libraries.h"
template<typename... Args>
std::string dyna_print(std::string_view rt_fmt_str, Args&&... args)
{
    return fmt::vformat(rt_fmt_str, fmt::make_format_args(args...));
}

// #define ENABLE_LOGGING
#ifndef ENABLE_LOGGING
    #define LOG_DEBUG(...)
#else
    #define LOG_DEBUG(...)  std::cout<<dyna_print(__VA_ARGS__)<<std::endl;
#endif
