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

    std::unique_ptr<queueable> SignalGenerators::sine(const double freq, const double amplitude) const {
        const size_t period_steps = static_cast<size_t>(1.0 / freq * static_cast<double>(sampleRate));
        auto buf = std::make_unique<signal_buf>(period_steps);
        for (size_t i = 0; i < period_steps; i++) {
            const double frac = static_cast<double>(i) / static_cast<double>(period_steps);
            buf[i] = static_cast<float>(std::sin(frac * TAU) * amplitude);
        }
        return std::make_unique<queueable>(queueable {
            .repeat = std::move(buf),
            .length = period_steps,
            .position = 0
        });
    }

    std::unique_ptr<queueable> SignalGenerators::square(const double freq, const double amplitude) const {
        const size_t period_steps = static_cast<size_t>(1.0 / freq * static_cast<double>(sampleRate));
        auto buf = std::make_unique<signal_buf>(period_steps);
        for (size_t i = 0; i < period_steps; i++) {
            const double frac = static_cast<double>(i) / static_cast<double>(period_steps);
            buf[i] = static_cast<float>(frac <= 0.5 ? amplitude : -amplitude);
        }
        return std::make_unique<queueable>(queueable {
            .repeat = std::move(buf),
            .length = period_steps,
            .position = 0
        });
    }

    std::unique_ptr<queueable> SignalGenerators::sawtooth(const double freq, const double amplitude) const {
        const size_t period_steps = static_cast<size_t>(1.0 / freq * static_cast<double>(sampleRate));
        auto buf = std::make_unique<signal_buf>(period_steps);
        const double step = 2.0 / static_cast<double>(period_steps);
        double value = -1.0;
        for (size_t i = 0; i < period_steps; i++) {
            buf[i] = static_cast<float>(value * amplitude);
            value += step;
            // if (value >= 1.0) value = -1.0;
        }
        return std::make_unique<queueable>(queueable {
            .repeat = std::move(buf),
            .length = period_steps,
            .position = 0
        });
    }
}
