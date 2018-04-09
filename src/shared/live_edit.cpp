#include "live_edit.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

static LiveEdit *s_instance;

static std::string build_response(const std::string& body)
{
    return "HTTP/1.1 200 OK\n"
        "Access-Control-Allow-Origin: *\n"
        "Content-Type: text/plain\n"
        "Content-Length: " + std::to_string(body.length()) + "\n\n"
        + body;
}

static std::string serialize(const LiveEdit::ValueMap& value_map)
{
    std::stringstream stream;

    for (const auto& iter : value_map) {
        stream << iter.first << "="
            << iter.second.value << ":"
            << iter.second.min << ":"
            << iter.second.max << ";";
    }

    return stream.str();
}

static void parse_and_maybe_insert(LiveEdit::ValueMap& value_map, const std::string& kvp)
{
    const auto equals_index = kvp.find('=');
    if (equals_index == std::string::npos) return;

    const auto key = kvp.substr(0, equals_index);
    const auto value = kvp.substr(equals_index + 1, kvp.size() - equals_index - 1);

    try {
        float parsed_value = std::stof(value);
        value_map[key].value = parsed_value;
    }
    catch (std::invalid_argument&)
    { }
}

static std::string handle_request(LiveEdit::ValueMap& value_map, const std::string& message)
{
    std::stringstream message_stream(message);
    std::string line;

    while (std::getline(message_stream, line, '\n'))
    { }

    std::stringstream line_stream(line);
    std::string kvp;

    while (std::getline(line_stream, kvp, ';')) {
        // parse_and_maybe_insert(value_map, kvp);
    }

    return build_response(serialize(value_map));
}

LiveEdit::LiveEdit()
    : m_server(LiveEdit::PORT)
{ 
    if (s_instance != nullptr) {
        throw std::exception("Cannot create more than one instance of LiveEdit.");
    }

    s_instance = this;
}

LiveEdit::~LiveEdit()
{
    s_instance = nullptr;
}

void LiveEdit::update()
{
    m_server.listen([this](const std::string& request_body) {
        return handle_request(this->m_values, request_body);
    });
}

float LiveEdit::get_value(const std::string& label, float min, float max)
{
    s_instance->m_values[label].min = min;
    s_instance->m_values[label].max = max;
    return s_instance->m_values[label].value;
}