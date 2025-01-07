//
// Created by penguinencounter on 12/13/24.
//

#ifndef AUDIO_H
#define AUDIO_H
#include <portaudio.h>
#include <queue>
#include <vector>
#include <bits/basic_ios.h>
#include "signals.h"

namespace microsynth {
    class AudioDriver;

    struct queueable {
        unsigned int id;
        std::shared_ptr<signal_buf> repeat;
        size_t length;
        size_t position;
    };

    struct pa_userdata {
        AudioDriver *that;
        std::vector<std::shared_ptr<queueable>> running{};
    };

    class ActionCommand {
    public:
        virtual ~ActionCommand() = default;

        virtual void run([[maybe_unused]] pa_userdata& data) {}
    };

    class QueueSFXCommand final : ActionCommand {
    private:
        std::shared_ptr<queueable> q;

    public:
        explicit QueueSFXCommand(const std::shared_ptr<queueable> &from);

        QueueSFXCommand &operator=(const QueueSFXCommand &from);

        QueueSFXCommand &operator=(QueueSFXCommand &&from) noexcept;

        QueueSFXCommand(const QueueSFXCommand &from);

        QueueSFXCommand(QueueSFXCommand &&from) noexcept;

        void run(pa_userdata& data) override;

        ~QueueSFXCommand() override;
    };

    class AudioDriver {
    public:
        static constexpr int SAMPLE_RATE = 44100;
        static constexpr int TABLE_SIZE = 200;

    private:
        std::queue<ActionCommand> actions{};

        bool finalized{false};

        pa_userdata data{
            .that = this,
        };
        PaStream *stream{nullptr};

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
