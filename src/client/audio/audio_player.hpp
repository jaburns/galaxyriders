#pragma once

#include "../gl.hpp"
#include <cstdint>
#include <memory>
#include <list>
#include <string>
#include <vector>

struct StereoSample
{
    float left;
    float right;
};

class SampleBuffer
{
    std::vector<StereoSample> m_buffer;

public:
    SampleBuffer();
    SampleBuffer(const std::string& path);

    size_t size() const;
    void mix_into_buffer(StereoSample *target, int start_sample, int sample_count) const;
};

class OneShotBufferReader
{
    int m_read_head;
    std::shared_ptr<const SampleBuffer> m_buffer;

public:
    OneShotBufferReader(std::shared_ptr<const SampleBuffer> buffer);
    void mix_into_buffer(StereoSample *target, int sample_count);
    bool is_done() const;
};

class AudioPlayer
{
    std::shared_ptr<SampleBuffer> m_buffer;
    std::list<OneShotBufferReader> m_readers;

    static void audio_callback_dispatch(void *instance, uint8_t *stream, int len);
    void audio_callback(StereoSample *stream, int samples); // L R L R L R ...

public:
    AudioPlayer();
    ~AudioPlayer();

    void play();
};