#pragma once

#include <vector>

struct PlayerInput
{
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;

    PlayerInput() { }
    PlayerInput(const uint8_t *serialized, int serialized_length);
    std::vector<uint8_t> serialize() const;
};
