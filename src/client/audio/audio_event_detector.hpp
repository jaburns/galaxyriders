#pragma once

#include "../../shared/lang_utils.hpp"
#include "audio_player.hpp"
#include "../client_state.hpp"

class AudioEventDetector : public NoCopy
{
    AudioPlayer m_audio_player;

public:
    AudioEventDetector();
    void detect_and_play_sounds(const ClientState& previous_state, const ClientState& current_state);
};