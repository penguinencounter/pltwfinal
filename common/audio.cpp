//
// Created by penguinencounter on 12/13/24.
//

#include "audio.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "helpers.h"

namespace microsynth {
    static constexpr auto TAG_DESTRUCTOR = "~AudioDriver";

    std::string construct_pa_error_message(const PaError pa_error, const char *const tag = nullptr) {
        std::stringstream buf{};
        buf << "PortAudio error";
        if (tag) buf << " [" << *tag << "]";
        buf << ": " << Pa_GetErrorText(pa_error);
        return buf.str();
    }

    [[noreturn]] inline void throw_portaudio_error(const PaError pa_error, const char *tag = nullptr) {
        throw std::runtime_error(construct_pa_error_message(pa_error, tag));
    }

    inline void pawrap(const PaError result) {
        if (result != paNoError) throw_portaudio_error(result);
    }

    inline void pawrap_weak(const PaError result) {
        if (result != paNoError) std::cerr << construct_pa_error_message(result) << "\n";
    }

    int AudioDriver::pa_callback(
        [[maybe_unused]] const void *input_buf,
        [[maybe_unused]] void *output_buf,
        [[maybe_unused]] unsigned long frames_per_buf,
        [[maybe_unused]] const PaStreamCallbackTimeInfo *time_info,
        [[maybe_unused]] PaStreamCallbackFlags status_flags,
        void *user_data
    ) {
        auto *data = static_cast<pa_userdata *>(user_data);
        auto *out = static_cast<float *>(output_buf);

        for (size_t i = 0; i < frames_per_buf; i++) {
            *out++ = data->sine[data->left_phase];
            *out++ = data->sine[data->right_phase];
            data->left_phase += 3;
            if (data->left_phase >= TABLE_SIZE) data->left_phase -= TABLE_SIZE;
            data->right_phase += 3;
            if (data->right_phase >= TABLE_SIZE) data->right_phase -= TABLE_SIZE;
        }
        return paContinue;
    }

    AudioDriver::AudioDriver() {
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

        for (size_t i = 0; i < TABLE_SIZE; i++) {
            data.sine[i] = static_cast<float>(std::sin(
                static_cast<double>(i) / static_cast<double>(TABLE_SIZE) * M_PI * 2.
            ));
        }
        data.left_phase = data.right_phase = 0;
    }

    void AudioDriver::finalize() {
        if (finalized) return;
        if (Pa_IsStreamActive(stream) == 1) {
            pawrap_weak(Pa_StopStream(stream));
        }
        pawrap_weak(Pa_CloseStream(stream));
        pawrap_weak(Pa_Terminate());
        finalized = true;
    }

    AudioDriver::~AudioDriver() {
        finalize();
    }
}
