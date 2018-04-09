#include "live_edit.hpp"
#include <string>
#include <vector>
#include <sstream>

#include <iostream>

static LiveEdit::Values s_values;

static std::string build_response(const std::string& body)
{
    return "HTTP/1.1 200 OK\n"
        "Access-Control-Allow-Origin: *\n"
        "Content-Type: text/plain\n"
        "Content-Length: " + std::to_string(body.length()) + "\n\n"
        + body;
}

static std::string request_handler(const std::string& message)
{
    std::stringstream message_stream(message);
    std::string line;

    while (std::getline(message_stream, line, '\n'))
    { }

    std::stringstream line_stream(line);
    std::string value;
    std::vector<float> values;

    while (std::getline(line_stream, value, ','))
    {
        float new_value = 0.0f;

        try {
            new_value = std::stof(value);
        }
        catch (std::invalid_argument& e)
        { }

        values.push_back(new_value);
    }

    if (values.size() >= 2)
    {
        s_values.gravity = values[0];
        s_values.ollie = values[1];
    }

    return build_response("Hello world");
}

LiveEdit::LiveEdit()
    : m_server(LiveEdit::PORT)
{ }

void LiveEdit::update()
{
    m_server.listen(request_handler);
}

LiveEdit::Values LiveEdit::get_values()
{
    return s_values;
}
