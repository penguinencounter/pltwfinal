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
    void queue_sfx_command::run(pa_userdata* data) const
    {
        data->running.insert({q->id, q});
    }

    queue_sfx_command::queue_sfx_command(const queue_sfx_command& from): q(from.q)
    {
    }

    queue_sfx_command::queue_sfx_command(queue_sfx_command&& from) noexcept: q(nullptr)
    {
        q = std::move(from.q);
    }

    queue_sfx_command& queue_sfx_command::operator=(const queue_sfx_command& from)
    {
        if (this == &from) return *this;
        q = from.q;
        return *this;
    }

    queue_sfx_command& queue_sfx_command::operator=(queue_sfx_command&& from) noexcept
    {
        q = from.q;
        from.q = nullptr;
        return *this;
    }

    queue_sfx_command::queue_sfx_command(const std::shared_ptr<queueable>& from): q(from)
    {
    }

    queue_sfx_command::~queue_sfx_command() = default;

    force_stop_sfx_command::force_stop_sfx_command(const unsigned long id): id(id)
    {
    }

    force_stop_sfx_command::force_stop_sfx_command(const std::shared_ptr<queueable>& from): id(from->id)
    {
    }

    void force_stop_sfx_command::run(pa_userdata* data) const
    {
        data->running.erase(id);
    }

    req_stop_sfx_command::req_stop_sfx_command(const unsigned long id): id(id)
    {
    }

    req_stop_sfx_command::req_stop_sfx_command(const std::shared_ptr<queueable>& from): id(from->id)
    {
    }

    void req_stop_sfx_command::run(pa_userdata* data) const
    {
        std::shared_ptr<queueable>& qref = data->running.at(id);
        if (qref->capabilities.looping)
            qref->player_state.looping = false;
        else // just delete it, it would play out in full anyway (e.g. stingers, Rick Astley's Never Gonna Give You Up)
            data->running.erase(id);
    }

    set_volume_command::set_volume_command(float volume) : new_volume(volume)
    {
    }

    void set_volume_command::run(pa_userdata* data) const
    {
        data->master_volume = new_volume;
    }

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

    constexpr size_t RMQ_SIZE = 32;

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
            action_queue* q = data->queue_ptr;
            while (!q->empty())
            {
                const auto& a = q->front();
                a->run(data);
                q->pop();
            }
        }

        // because we're stereo, this is interleaved
        auto* out = static_cast<float*>(output_buf);

        for (size_t i = 0; i < frames_per_buf; i++)
        {
            float value = 0.0;
            unsigned long removal[RMQ_SIZE] {};
            size_t remove_cur = 0;
            // ReSharper disable once CppUseElementsView
            for (auto& [_, snd] : data->running)
            {
                const std::shared_ptr<queueable>& q = snd;
                // wow, bindings!!!
                auto& [looping, position, alive] = q->player_state;
                auto& [can_loop, loop_at, loop_to] = q->capabilities;
                if (!alive)
                {
                    if (remove_cur < RMQ_SIZE)
                        removal[remove_cur++] = q->id;
                    continue;
                }
                value += q->buf[static_cast<std::ptrdiff_t>(position)];
                ++position;
                if (looping && position == loop_at) position = loop_to;
                if (position >= q->length)
                {
                    // Prevent overruns
                    alive = false;
                    // queue it for removal if we have space to do that
                    if (remove_cur < RMQ_SIZE)
                        removal[remove_cur++] = q->id;
                }
            }
            while (remove_cur > 0)
            {
                data->running.erase(removal[--remove_cur]);
            }
            if (value > 1.0f) value = 1.0f;
            if (value < -1.0f) value = -1.0f;
            *out++ = value * data->master_volume;
            *out++ = value * data->master_volume;
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

    void AudioDriver::enqueue(const std::shared_ptr<action_command> action)
    {
        actions.push(action);
    }
}
