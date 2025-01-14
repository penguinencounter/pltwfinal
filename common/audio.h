//
// Created by penguinencounter on 12/13/24.
//

#ifndef AUDIO_H
#define AUDIO_H
#include <unordered_map>
#include <portaudio.h>
#include <queue>
#include <vector>

#include "audio.h"
#include "signalmeta.h"

namespace microsynth
{
    class action_command;
    class AudioDriver;

    using action_queue = std::queue<std::shared_ptr<action_command>>;

    struct pa_userdata
    {
        AudioDriver* that;
        std::unordered_map<unsigned long, std::shared_ptr<generic_clip>> active_clips{};
        action_queue* queue_ptr;

        // Global Controls;
        float master_volume;
    };

    class action_command
    {
    public:
        virtual ~action_command() = default;

        virtual void run([[maybe_unused]] pa_userdata* data, [[maybe_unused]] PaTime now) const
        {
        }
    };

    class queue_sfx_command final : public action_command
    {
    private:
        std::shared_ptr<generic_clip> q;

    public:
        explicit queue_sfx_command(const std::shared_ptr<generic_clip>& from);

        queue_sfx_command& operator=(const queue_sfx_command& from);

        queue_sfx_command& operator=(queue_sfx_command&& from) noexcept;

        queue_sfx_command(const queue_sfx_command& from);

        queue_sfx_command(queue_sfx_command&& from) noexcept;

        void run(pa_userdata* data, PaTime now) const override;

        ~queue_sfx_command() override;
    };

    class force_stop_sfx_command final : public action_command
    {
    private:
        unsigned long id;

    public:
        explicit force_stop_sfx_command(unsigned long id);
        explicit force_stop_sfx_command(const std::shared_ptr<generic_clip>& from);

        void run(pa_userdata* data, PaTime now) const override;
    };

    class req_stop_sfx_command final : public action_command
    {
    private:
        unsigned long id;

    public:
        explicit req_stop_sfx_command(unsigned long id);
        explicit req_stop_sfx_command(const std::shared_ptr<generic_clip>& from);

        void run(pa_userdata* data, PaTime now) const override;
    };

    class set_volume_command final : public action_command
    {
    private:
        float new_volume;

    public:
        explicit set_volume_command(float volume);

        void run(pa_userdata* data, PaTime now) const override;
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
            .queue_ptr = &actions,
            .master_volume = 1.0f
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
