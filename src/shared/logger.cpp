#include "logger.hpp"

Logger::Logger()
    : m_log_stream()
{ }

Logger& Logger::instance()
{
    static Logger instance;
    return instance;
}

bool Logger::empty()
{
    return m_log_stream.rdbuf()->in_avail() <= 0;
}

std::string Logger::flush()
{
    const auto result = m_log_stream.str();
    m_log_stream.str("");
    m_log_stream.clear();
    return result;
}