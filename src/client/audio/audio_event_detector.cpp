#include "audio_event_detector.hpp"

AudioEventDetector::AudioEventDetector()
    : m_audio_player()
{ }

void AudioEventDetector::detect_and_play_sounds(const ClientState& previous_state, const ClientState& current_state)
{
    if (!previous_state.world.players.at(0).grounded && current_state.world.players.at(0).grounded) {
        m_audio_player.play_land();
    }

    if (previous_state.world.players.at(0).grounded && !current_state.world.players.at(0).grounded) {
        m_audio_player.play_ollie();
    }
}
