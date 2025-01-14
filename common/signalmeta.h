#ifndef SIGNALMETA_H
#define SIGNALMETA_H
#include <functional>
#include <memory>
#include <portaudio.h>
#include <variant>

namespace microsynth
{
    using signal_fmt = float;
    using signal_ptr = signal_fmt*;
    using signal_buf = signal_fmt[];
    using size_t = std::size_t;

    static unsigned long _next_ID = 1;

    class generic_clip;

    class exact_clip {
    public:
        std::function<signal_fmt(const std::shared_ptr<generic_clip>&, double)> getPCM;
        double time_basis = 0.0;
    };

    class sampled_clip {
    public:
        std::shared_ptr<signal_buf> buf;
        size_t length;

        bool looping;
        size_t loop_at;
        size_t loop_to;
        size_t position = 0;
    };

    class generic_clip {
    public:
        enum class clip_type_t {
            EXACT = 0, // mathematical functions
            SAMPLES = 1, // exactly 44100 hz PCM
        };
        using clip_type_v = std::variant<sampled_clip, exact_clip>;
        clip_type_t clip_type;
        clip_type_v data;

        std::function<void(const std::shared_ptr<generic_clip>&, PaTime, std::size_t)> sample_post {};
        std::function<void(const std::shared_ptr<generic_clip>&, PaTime, PaTime, std::size_t)> stop_sample_post {};

        PaTime start_time = 0.0;
        PaTime stop_time = 0.0;
        double volume = 1.0;
        bool alive = true;
        bool stopping = false;
        unsigned long id = _next_ID++;

        ~generic_clip();

        [[nodiscard]] std::unique_ptr<generic_clip> copy() const;
        friend std::ostream& operator<<(std::ostream& os, generic_clip const& m);

    };

    class queueable
    {
    public:
        class capabilities_t
        {
        public:
            // Looping-related things
            bool looping = true;
            /**
             * When to start looping. Looping only occurs if `use_loop` is true.
             * Otherwise, running off the end of the buffer will erase the queueable.
             */
            size_t loop_at = 0;
            /**
             * When reaching the 'loop_at' position, go to 'loop_to'. Set to 0 to loop the entire thing.
             */
            size_t loop_to = 0;
        };
        class player_state_t
        {
        public:
            // if the clip is currently looping or not
            // always 'false' if capabilities.looping is 'false'
            bool looping;

            // "playhead"
            size_t position;

            // if false, don't do anything with it other than queue it for deletion
            bool alive;
        };

        [[nodiscard]] std::unique_ptr<queueable> copy() const;
        friend std::ostream& operator<<(std::ostream& os, queueable const& m);

        unsigned long id = _next_ID++;
        std::shared_ptr<signal_buf> buf;
        size_t length;

        capabilities_t capabilities;

        player_state_t player_state {
            .looping = capabilities.looping,
            .position = 0,
            .alive = true
        };
    };

    /*
     * NOT IMPLEMENTED
     */
    queueable mix(const queueable& first, const queueable& second);
    void reid(queueable& target);
}

#endif //SIGNALMETA_H
