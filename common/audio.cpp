//
// Created by penguinencounter on 12/13/24.
//

#include "audio.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "helpers.h"

namespace microsynth
{
    void QueueSFXCommand::run(pa_userdata& data)
    {
        data.running.push_back(std::move(q));
    }

    QueueSFXCommand::QueueSFXCommand(const QueueSFXCommand& from): q(from.q)
    {
    }

    QueueSFXCommand::QueueSFXCommand(QueueSFXCommand&& from) noexcept: q(nullptr)
    {
        q = std::move(from.q);
    }

    QueueSFXCommand& QueueSFXCommand::operator=(const QueueSFXCommand& from)
    {
        if (this == &from) return *this;
        q = from.q;
        return *this;
    }

    QueueSFXCommand& QueueSFXCommand::operator=(QueueSFXCommand&& from) noexcept
    {
        q = from.q;
        from.q = nullptr;
        return *this;
    }

    QueueSFXCommand::QueueSFXCommand(const std::shared_ptr<queueable>& from): q(from)
    {
    }

    QueueSFXCommand::~QueueSFXCommand() = default;


    std::string construct_pa_error_message(const PaError pa_error, const char* const tag = nullptr)
    {
        std::stringstream buf{};
        buf << "PortAudio error";
        if (tag) buf << " [" << *tag << "]";
        buf << ": " << Pa_GetErrorText(pa_error);
        return buf.str();
    }

    [[noreturn]] inline void throw_portaudio_error(const PaError pa_error, const char* tag = nullptr)
    {
        throw std::runtime_error(construct_pa_error_message(pa_error, tag));
    }

    inline void pawrap(const PaError result)
    {
        if (result != paNoError) throw_portaudio_error(result);
    }

    inline void pawrap_weak(const PaError result)
    {
        if (result != paNoError) std::cerr << construct_pa_error_message(result) << "\n";
    }

    int AudioDriver::pa_callback(
        [[maybe_unused]] const void* input_buf,
        [[maybe_unused]] void* output_buf,
        [[maybe_unused]] unsigned long frames_per_buf,
        [[maybe_unused]] const PaStreamCallbackTimeInfo* time_info,
        [[maybe_unused]] PaStreamCallbackFlags status_flags,
        [[maybe_unused]] void* user_data
    )
    {
        auto* data = static_cast<pa_userdata*>(user_data);
        // first, process all the actions
        if (data->queue_ptr)
        {
            action_queue q = *data->queue_ptr;
            while (!q.empty())
            {
                ActionCommand& a = q.front();
                a.run(*data);
                q.pop();
            }
        }

        // because we're stereo, this is interleaved
        auto* out = static_cast<float*>(output_buf);

        size_t run_size = data->running.size();

        for (size_t i = 0; i < frames_per_buf; i++)
        {
            float value = 0.0;
            for (size_t j = 0; j < run_size; j++)
            {
                std::shared_ptr<queueable> q = data->running[j];
                value += q->repeat[static_cast<std::ptrdiff_t>(q->position)];
            }
            if (value > 1.0f) value = 1.0f;
            if (value < -1.0f) value = -1.0f;
            *out++ = value;
            *out++ = value;
        }
        return paContinue;
    }

    AudioDriver::AudioDriver()
    {
        pawrap(Pa_Initialize());
        pawrap(Pa_OpenDefaultStream(
            &stream,
            0, // input
            2, // output
            paFloat32, // format
            SAMPLE_RATE,
            256, // frames per buffer
            pa_callback,
            &data
        ));
        pawrap(Pa_StartStream(stream));
    }

    void AudioDriver::finalize()
    {
        if (finalized) return;
        if (Pa_IsStreamActive(stream) == 1)
        {
            pawrap_weak(Pa_StopStream(stream));
        }
        pawrap_weak(Pa_CloseStream(stream));
        pawrap_weak(Pa_Terminate());
        finalized = true;
    }

    AudioDriver::~AudioDriver()
    {
        finalize();
    }
}
