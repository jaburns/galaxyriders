#include "player_input.hpp"

#include "serialization.hpp"

std::vector<uint8_t> PlayerInput::serialize() const
{
    SerializationBuffer buf;
    uint32_t bit_flags = 0;

    if (left)  bit_flags |= (1 << 0);
    if (right) bit_flags |= (1 << 1);
    if (up)    bit_flags |= (1 << 2);
    if (down)  bit_flags |= (1 << 3);

    buf.write_val32(bit_flags);

    return buf.buffer;
}

PlayerInput::PlayerInput(const uint8_t *serialized, int serialized_length)
{
    DeserializationBuffer buf(serialized, serialized_length);
    const auto bit_flags = buf.read_val32<uint32_t>();

    left  = bit_flags & (1 << 0);
    right = bit_flags & (1 << 1);
    up    = bit_flags & (1 << 2);
    down  = bit_flags & (1 << 3);
}