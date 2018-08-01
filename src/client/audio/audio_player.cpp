#include "audio_player.hpp"

#include <iostream>
#include "../../shared/logger.hpp"

SampleBuffer::SampleBuffer()
    : m_buffer()
{ }

SampleBuffer::SampleBuffer(const std::string& path)
    : m_buffer()
{
    SDL_AudioSpec wav_spec;
    uint8_t *wav_buffer;
    uint32_t wav_length;

    if (SDL_LoadWAV(path.c_str(), &wav_spec, &wav_buffer, &wav_length) == NULL) {
        std::cout << "Failed to load .wav file: " << path << std::endl;
        exit(1);
    }

    SDL_AudioCVT cvt;
    std::vector<uint8_t> convert_buffer(wav_buffer, wav_buffer + wav_length);

    SDL_BuildAudioCVT(&cvt, wav_spec.format, wav_spec.channels, wav_spec.freq, AUDIO_F32SYS, 2, 44100);
    convert_buffer.resize(wav_length * cvt.len_mult);
    cvt.len = wav_length;
    cvt.buf = convert_buffer.data();
    SDL_ConvertAudio(&cvt);

    size_t samples = cvt.len * cvt.len_ratio / sizeof(StereoSample);
    auto sample_buf = reinterpret_cast<StereoSample*>(cvt.buf);
    m_buffer = std::vector<StereoSample>(sample_buf, sample_buf + samples);

    SDL_FreeWAV(wav_buffer);
}

size_t SampleBuffer::size() const
{
    return m_buffer.size();
}

void SampleBuffer::mix_into_buffer(StereoSample *target, int start_sample, int sample_count) const
{
    SDL_MixAudioFormat(
        reinterpret_cast<uint8_t *>(target),
        reinterpret_cast<const uint8_t *>(m_buffer.data() + start_sample),
        AUDIO_F32SYS,
        sample_count * sizeof(StereoSample),
        SDL_MIX_MAXVOLUME
    );
}

OneShotBufferReader::OneShotBufferReader(std::shared_ptr<const SampleBuffer> buffer)
    : m_read_head(0), m_buffer(buffer)
{ }

void OneShotBufferReader::mix_into_buffer(StereoSample *target, int sample_count)
{
    const auto end_sample = m_read_head + sample_count;
    const auto fixed_end_sample = end_sample > m_buffer->size()
        ? m_buffer->size()
        : end_sample;
    const auto fixed_sample_count = fixed_end_sample - m_read_head;

    m_buffer->mix_into_buffer(target, m_read_head, fixed_sample_count);
    m_read_head += fixed_sample_count;
}

bool OneShotBufferReader::is_done() const
{
    return m_read_head >= m_buffer->size();
}

AudioPlayer::AudioPlayer()
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        std::cout << "Failed to init SDL audio subsystem." << std::endl;
        exit(1);
    }

    SDL_AudioSpec spec;
    spec.callback = &audio_callback_dispatch;
    spec.userdata = this;
    spec.channels = 2;
    spec.freq = 44100;
    spec.format = AUDIO_F32SYS;
    spec.samples = 256;

    if (SDL_OpenAudio(&spec, NULL) < 0) {
        std::cout << "Couldn't open audio: " << SDL_GetError() << std::endl;
        exit(1);
    }

    m_sound_ollie = std::make_shared<SampleBuffer>("res/ollie.wav");
    m_sound_land  = std::make_shared<SampleBuffer>("res/land.wav");

    SDL_PauseAudio(0);
}

void AudioPlayer::audio_callback_dispatch(void *instance, uint8_t *stream, int len)
{
    memset(stream, 0, len);

    static_cast<AudioPlayer*>(instance)->audio_callback(
        reinterpret_cast<StereoSample*>(stream),
        len / sizeof(StereoSample)
    );
}

void AudioPlayer::audio_callback(StereoSample *stream, int samples)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& reader : m_readers) {
        reader.mix_into_buffer(stream, samples);
    }

    m_readers.remove_if([](auto& r) { return r.is_done(); });
}

void AudioPlayer::play_ollie()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_readers.push_back(OneShotBufferReader(m_sound_ollie));
}

void AudioPlayer::play_land()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_readers.push_back(OneShotBufferReader(m_sound_land));
}

AudioPlayer::~AudioPlayer()
{
    SDL_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
