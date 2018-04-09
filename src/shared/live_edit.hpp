#pragma once
#include <cstdint>
#include "lang_utils.hpp"
#include "network.hpp"

class LiveEdit : public NoCopy
{
    TCPServer m_server;

public:
    struct Values
    {
        float gravity = 1.0f;
        float ollie = 1.0f;
    };

    static const uint16_t PORT = 1337;

    LiveEdit();
    void update();

    static Values get_values();
};