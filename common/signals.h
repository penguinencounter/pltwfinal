//
// Signal generators.
//

#ifndef SIGNALS_H
#define SIGNALS_H
#include <memory>

namespace microsynth
{
    namespace A440
    {
        static constexpr double C4 = 261.6256;
        static constexpr double D4 = 293.6648;
        static constexpr double E4 = 329.6276;
        static constexpr double F4 = 349.2282;
        static constexpr double G4 = 391.9954;
        static constexpr double A4 = 440.0000;
        static constexpr double B4 = 493.8833;
        static constexpr double C5 = 523.2511;
    }

    using signal_fmt = float;
    using signal_ptr = signal_fmt*;
    using signal_buf = signal_fmt[];

    class SignalGenerators
    {
    private:
        size_t sampleRate = 0;
        double secondsPerSample = 0.0;

    public:
        void setSampleRate(int hz);

        void setSampleRate(size_t hz);

        [[nodiscard]] size_t getSampleRate() const;

        [[nodiscard]] std::unique_ptr<signal_buf> sine(double freq, double amplitude = 1.0) const;

        [[nodiscard]] std::unique_ptr<signal_buf> square(double freq, double amplitude = 1.0) const;

        [[nodiscard]] std::unique_ptr<signal_buf> sawtooth(double freq, double amplitude = 1.0) const;
    };
}

#endif //SIGNALS_H
