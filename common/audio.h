//
// Created by penguinencounter on 12/13/24.
//

#ifndef AUDIO_H
#define AUDIO_H
#include <unordered_map>
#include <portaudio.h>
#include <queue>
#include <vector>
#include "signalmeta.h"

namespace microsynth
{
    class ActionCommand;
    class AudioDriver;

    using action_queue = std::queue<std::shared_ptr<ActionCommand>>;

    struct pa_userdata
    {
        AudioDriver* that;
        std::unordered_map<unsigned long, std::shared_ptr<queueable>> running{};
        action_queue* queue_ptr;
    };

    class ActionCommand
    {
    public:
        virtual ~ActionCommand() = default;

        virtual void run([[maybe_unused]] pa_userdata* data) const
        {
        }
    };

    class QueueSFXCommand final : public ActionCommand
    {
    private:
        std::shared_ptr<queueable> q;

    public:
        explicit QueueSFXCommand(const std::shared_ptr<queueable>& from);

        QueueSFXCommand& operator=(const QueueSFXCommand& from);

        QueueSFXCommand& operator=(QueueSFXCommand&& from) noexcept;

        QueueSFXCommand(const QueueSFXCommand& from);

        QueueSFXCommand(QueueSFXCommand&& from) noexcept;

        void run(pa_userdata* data) const override;

        ~QueueSFXCommand() override;
    };

    class StopSFXCommand final : public ActionCommand
    {
    private:
        unsigned long id;

    public:
        explicit StopSFXCommand(unsigned long id);
        explicit StopSFXCommand(const std::shared_ptr<queueable>& from);

        void run(pa_userdata* data) const override;
    };

    class AudioDriver
    {
    public:
        static constexpr int SAMPLE_RATE = 44100;
        static constexpr int TABLE_SIZE = 200;

    private:
        action_queue actions{};

        bool finalized{false};

        pa_userdata data{
            .that = this,
            .queue_ptr = &actions
        };
        PaStream* stream{nullptr};

        static int pa_callback(
            const void* input_buf,
            void* output_buf,
            unsigned long frames_per_buf,
            const PaStreamCallbackTimeInfo* time_info,
            PaStreamCallbackFlags status_flags,
            void* user_data);

    public:
        AudioDriver();

        void finalize();

        ~AudioDriver();

        AudioDriver(const AudioDriver&) = delete;

        AudioDriver(AudioDriver&&) = delete;

        AudioDriver& operator=(const AudioDriver&) = delete;

        AudioDriver& operator=(AudioDriver&&) = delete;

        void enqueue(std::shared_ptr<ActionCommand> action);

        pa_userdata& getuserdata() { return data; }
    };
}

#endif //AUDIO_H
