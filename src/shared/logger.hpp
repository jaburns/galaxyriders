#pragma once

#include <sstream>
#include <iostream>

#define COPY_TO_STDOUT
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
        #ifdef COPY_TO_STDOUT
            std::cout << std::endl;
        #endif
        m_log_stream << std::endl;
    }

    template<typename T, typename ...Ts>
    void log(T t, Ts... ts) 
    {
        #ifdef COPY_TO_STDOUT
            std::cout << t << " ";
        #endif
        m_log_stream << t << " ";

        log(ts...);
    }

    bool empty();
    std::string flush();
};