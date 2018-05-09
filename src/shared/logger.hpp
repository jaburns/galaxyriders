#pragma once

#include <memory>
#include <sstream>

class Logger
{
    static std::shared_ptr<Logger> s_instance;

    bool m_has_new = false;
    std::stringstream m_log_stream;

    Logger();

public:
    static std::shared_ptr<Logger> instance();

    template<typename none = void>
    void log() 
    {
        m_log_stream << std::endl;
        m_has_new = true;
    }

    template<typename T, typename ...Ts>
    void log(T t, Ts... ts) 
    {
        m_log_stream << t << "; ";
        log(ts...);
    }

    bool has_new() const;
    std::string flush();
};