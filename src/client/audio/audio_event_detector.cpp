#include "audio_event_detector.hpp"

AudioEventDetector::AudioEventDetector()
    : m_audio_player()
{ }

#define MIN(x, y) ((x) < (y) ? (x) : (y))

void AudioEventDetector::detect_and_play_sounds(const ClientState& previous_state, const ClientState& current_state)
{
    for (const auto& kvp : current_state.world.players) 
    {
        if (previous_state.world.players.find(kvp.first) == previous_state.world.players.end())
            continue;

        const auto& prev = previous_state.world.players.at(kvp.first);
        const auto& cur = kvp.second;

        if (!prev.grounded && cur.grounded)
            m_audio_player.play_land();

        if (prev.grounded && !cur.grounded)
            m_audio_player.play_ollie();
    }
}
