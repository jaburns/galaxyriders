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
    int og = m_log_stream.tellg();
    m_log_stream.seekg(0, std::ios::end);
    int stream_size = m_log_stream.tellg();
    m_log_stream.seekg(og, std::ios::beg);

    return stream_size < 1;
}

std::string Logger::flush()
{
    const auto result = m_log_stream.str();
    m_log_stream.str("");
    m_log_stream.clear();
    return result;
}
