#pragma once

#include <cstdint>

namespace Config
{
    static constexpr float MILLIS_PER_TICK = 33.333333f;

    static constexpr uint32_t MAX_PACKET_SIZE = 2048;
    static constexpr uint16_t DEFAULT_PORT = 12345;
}
