#pragma once
#include "global.hpp"

#include <cstdint>

namespace Config
{
    static constexpr float MILLIS_PER_TICK = 1000.0f / 60.0f;

    static constexpr uint32_t MAX_PACKET_SIZE = 512;
    static constexpr uint16_t DEFAULT_PORT = 12345;
}
