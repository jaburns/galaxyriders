#pragma once

#include <sstream>
#include <iostream>

#define LOG(...) Logger::instance().log(__VA_ARGS__)

class Logger
{
    std::stringstream m_log_stream;
    Logger();

public:
    static Logger& instance();

    template<typename none = void>
    void log() 
    {
        m_log_stream << std::endl;
    }

    template<typename T, typename ...Ts>
    void log(T t, Ts... ts) 
    {
        m_log_stream << t << "; ";
        log(ts...);
    }

    bool empty();
    std::string flush();
};