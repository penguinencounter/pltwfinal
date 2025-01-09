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
    class action_command;
    class AudioDriver;

    using action_queue = std::queue<std::shared_ptr<action_command>>;

    struct pa_userdata
    {
        AudioDriver* that;
        std::unordered_map<unsigned long, std::shared_ptr<queueable>> running{};
        action_queue* queue_ptr;
    };

    class action_command
    {
    public:
        virtual ~action_command() = default;

        virtual void run([[maybe_unused]] pa_userdata* data) const
        {
        }
    };

    class queue_sfx_command final : public action_command
    {
    private:
        std::shared_ptr<queueable> q;

    public:
        explicit queue_sfx_command(const std::shared_ptr<queueable>& from);

        queue_sfx_command& operator=(const queue_sfx_command& from);

        queue_sfx_command& operator=(queue_sfx_command&& from) noexcept;

        queue_sfx_command(const queue_sfx_command& from);

        queue_sfx_command(queue_sfx_command&& from) noexcept;

        void run(pa_userdata* data) const override;

        ~queue_sfx_command() override;
    };

    class force_stop_sfx_command final : public action_command
    {
    private:
        unsigned long id;

    public:
        explicit force_stop_sfx_command(unsigned long id);
        explicit force_stop_sfx_command(const std::shared_ptr<queueable>& from);

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

        void enqueue(std::shared_ptr<action_command> action);

        pa_userdata& getuserdata() { return data; }
    };
}

#endif //AUDIO_H
