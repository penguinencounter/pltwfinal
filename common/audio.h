//
// Created by penguinencounter on 12/13/24.
//

#ifndef AUDIO_H
#define AUDIO_H
#include <portaudio.h>
#include <bits/basic_ios.h>

namespace microsynth {
    class AudioDriver {
    public:
        static constexpr int SAMPLE_RATE = 44100;
        static constexpr int TABLE_SIZE = 200;

    private:
        bool finalized{false};

        struct pa_userdata {
            AudioDriver *that;
            float sine[TABLE_SIZE];
            int left_phase;
            int right_phase;
        };

        pa_userdata data{
            .that = this,
            .sine = {},
            .left_phase = 0,
            .right_phase = 0,
        };
        PaStream *stream{};

        static int pa_callback(
            const void *input_buf,
            void *output_buf,
            unsigned long frames_per_buf,
            const PaStreamCallbackTimeInfo *time_info,
            PaStreamCallbackFlags status_flags,
            void *user_data);

    public:
        AudioDriver();

        void finalize();

        ~AudioDriver();

        AudioDriver(const AudioDriver &) = delete;

        AudioDriver(AudioDriver &&) = delete;

        AudioDriver &operator=(const AudioDriver &) = delete;

        AudioDriver &operator=(AudioDriver &&) = delete;
    };
}

#endif //AUDIO_H
