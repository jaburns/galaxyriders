#include "logger.hpp"

Logger::Logger()
    : m_log_stream()
{ }

std::shared_ptr<Logger> Logger::instance()
{
    if (s_instance == nullptr) {
        s_instance = std::make_shared<Logger>();
    }

    return s_instance;
}

bool Logger::has_new() const
{
    return m_has_new;
}

std::string Logger::flush()
{
    const auto result = m_log_stream.str();
    m_log_stream.clear();
    m_has_new = false;
    return result;
}