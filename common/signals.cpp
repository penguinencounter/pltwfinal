//
// Created by penguinencounter on 1/6/25.
//

#include "signals.h"
#include <cmath>

namespace microsynth {
    void SignalGenerators::setSampleRate(const int hz) {
        sampleRate = static_cast<size_t>(hz);
        secondsPerSample = 1.0 / hz;
    }

    void SignalGenerators::setSampleRate(const size_t hz) {
        sampleRate = hz;
        secondsPerSample = 1.0 / static_cast<int>(hz);
    }

    size_t SignalGenerators::getSampleRate() const {
        return sampleRate;
    }

    static constexpr double TAU = 6.283185307179586;

    std::unique_ptr<signal_fmt[]> SignalGenerators::sine(const double freq, const double amplitude) const {
        auto buf = std::make_unique<signal_fmt[]>(sampleRate);
        const double w = freq * TAU;
        for (size_t i = 0; i < sampleRate; i++) {
            const double t = static_cast<int>(i) * secondsPerSample;
            buf[i] = static_cast<float>(std::sin(t * w) * amplitude);
        }
        return buf;
    }

    std::unique_ptr<signal_buf> SignalGenerators::square(const double freq, const double amplitude) const {
        auto buf = std::make_unique<signal_buf>(sampleRate);
        const double term = 1 / (2 * freq);
        double last_toggle = 0.0;
        bool toggle = false;
        for (size_t i = 0; i < sampleRate; i++) {
            const double t = static_cast<int>(i) * secondsPerSample;
            if (t - last_toggle >= term) {
                toggle = !toggle;
                last_toggle = t;
            }
            buf[i] = static_cast<float>(toggle ? amplitude : -amplitude);
        }
        return buf;
    }

    std::unique_ptr<signal_buf> SignalGenerators::sawtooth(const double freq, const double amplitude) const {
        auto buf = std::make_unique<signal_buf>(sampleRate);
        const int samplesPerPeriod = static_cast<int>(1 / freq * static_cast<int>(sampleRate));
        const double step = 2.0 / samplesPerPeriod;
        double value = -1.0;
        for (size_t i = 0; i < sampleRate; i++) {
            buf[i] = static_cast<float>(value * amplitude);
            value += step;
            if (value >= 1.0) value = -1.0;
        }
        return buf;
    }
}
