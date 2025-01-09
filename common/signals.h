//
// Signal generators.
//

#ifndef SIGNALS_H
#define SIGNALS_H
#include <memory>
#include "signalmeta.h"

namespace microsynth
{
    template <typename T>
    constexpr T ipow(T value, int times)
    {
        return times <= 0 ? 1 : (value * ipow(value, times - 1));
    }

    template <typename T>
    constexpr T factor(T value, T scaleBy, int times)
    {
        if (times < 0) return value / ipow(scaleBy, -times);
        return value * ipow(scaleBy, times);
    }

    namespace A440
    {
        static constexpr double SEMITONE = 1.0594630943592952646;
        static constexpr double OCTAVE = 2.0;

#define tone_offset(name, offset) static constexpr double name = factor(A4, SEMITONE, offset)
#define octaves(note_name) \
    static constexpr double note_name##0 = factor(note_name##4, OCTAVE, -4); \
    static constexpr double note_name##1 = factor(note_name##4, OCTAVE, -3); \
    static constexpr double note_name##2 = factor(note_name##4, OCTAVE, -2); \
    static constexpr double note_name##3 = factor(note_name##4, OCTAVE, -1); \
    static constexpr double note_name##5 = factor(note_name##4, OCTAVE, +1); \
    static constexpr double note_name##6 = factor(note_name##4, OCTAVE, +2); \
    static constexpr double note_name##7 = factor(note_name##4, OCTAVE, +3);

        static constexpr double A4 = 440.0000;

        tone_offset(C4, -9);
        tone_offset(CSharp4, -8);
        tone_offset(D4, -7);
        tone_offset(DSharp4, -6);
        tone_offset(E4, -5);
        tone_offset(F4, -4);
        tone_offset(FSharp4, -3);
        tone_offset(G4, -2);
        tone_offset(GSharp4, -1);
        // A4 would be here
        tone_offset(ASharp4, 1);
        tone_offset(B4, 2);

        octaves(C);
        octaves(CSharp);
        octaves(D);
        octaves(DSharp);
        octaves(E);
        octaves(F);
        octaves(FSharp);
        octaves(G);
        octaves(GSharp);
        octaves(A);
        octaves(ASharp);
        octaves(B);
#undef tone_offset
#undef octaves
    }

    class SignalGenerators
    {
    private:
        size_t sampleRate = 0;
        double secondsPerSample = 0.0;

    public:
        void setSampleRate(int hz);

        void setSampleRate(size_t hz);

        [[nodiscard]] size_t getSampleRate() const;

        [[nodiscard]] std::unique_ptr<queueable> sine(double freq, double amplitude = 1.0) const;

        [[nodiscard]] std::unique_ptr<queueable> square(double freq, double amplitude = 1.0) const;

        [[nodiscard]] std::unique_ptr<queueable> sawtooth(double freq, double amplitude = 1.0) const;
        [[nodiscard]] std::unique_ptr<queueable> add_tail(const std::unique_ptr<const queueable>& from,
                                                          double tail = 0.02) const;
    };
}

#endif //SIGNALS_H
