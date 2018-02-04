#pragma once

#include <vector>

struct SharedInputState
{
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;

    SharedInputState() { }
    SharedInputState(const uint8_t *serialized, int serialized_length);
    std::vector<uint8_t> serialize() const;
};
