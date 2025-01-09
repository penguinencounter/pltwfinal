//
// Created by penguinencounter on 1/6/25.
//

#include "signals.h"
#include <cmath>

namespace microsynth
{
    void SignalGenerators::setSampleRate(const int hz)
    {
        sampleRate = static_cast<size_t>(hz);
        secondsPerSample = 1.0 / hz;
    }

    void SignalGenerators::setSampleRate(const size_t hz)
    {
        sampleRate = hz;
        secondsPerSample = 1.0 / static_cast<int>(hz);
    }

    size_t SignalGenerators::getSampleRate() const
    {
        return sampleRate;
    }

    static constexpr double TAU = 6.283185307179586;

    std::unique_ptr<queueable> SignalGenerators::sine(const double freq, const double amplitude) const
    {
        const size_t period_steps = static_cast<size_t>(1.0 / freq * static_cast<double>(sampleRate));
        auto buf = std::make_unique<signal_buf>(period_steps);
        for (size_t i = 0; i < period_steps; i++)
        {
            const double frac = static_cast<double>(i) / static_cast<double>(period_steps);
            buf[i] = static_cast<float>(std::sin(frac * TAU) * amplitude);
        }
        return std::make_unique<queueable>(queueable{
            .buf = std::move(buf),
            .length = period_steps,
            .loop_at = period_steps,
            .loop_to = 0,
            .position = 0
        });
    }

    std::unique_ptr<queueable> SignalGenerators::square(const double freq, const double amplitude) const
    {
        const size_t period_steps = static_cast<size_t>(1.0 / freq * static_cast<double>(sampleRate));
        auto buf = std::make_unique<signal_buf>(period_steps);
        for (size_t i = 0; i < period_steps; i++)
        {
            const double frac = static_cast<double>(i) / static_cast<double>(period_steps);
            buf[i] = static_cast<float>(frac <= 0.5 ? amplitude : -amplitude);
        }
        return std::make_unique<queueable>(queueable{
            .buf = std::move(buf),
            .length = period_steps,
            .loop_at = period_steps,
            .loop_to = 0,
            .position = 0
        });
    }

    std::unique_ptr<queueable> SignalGenerators::sawtooth(const double freq, const double amplitude) const
    {
        const size_t period_steps = static_cast<size_t>(1.0 / freq * static_cast<double>(sampleRate));
        auto buf = std::make_unique<signal_buf>(period_steps);
        const double step = 2.0 / static_cast<double>(period_steps);
        double value = -1.0;
        for (size_t i = 0; i < period_steps; i++)
        {
            buf[i] = static_cast<float>(value * amplitude);
            value += step;
            // if (value >= 1.0) value = -1.0;
        }
        return std::make_unique<queueable>(queueable{
            .buf = std::move(buf),
            .length = period_steps,
            .loop_at = period_steps,
            .loop_to = 0,
            .position = 0
        });
    }

    std::unique_ptr<queueable> SignalGenerators::add_tail(const std::unique_ptr<const queueable>& from, const double tail) const
    {
        const auto fadeout_steps = static_cast<size_t>(tail * static_cast<double>(sampleRate));
        const double step_frac = 1.0 / static_cast<double>(fadeout_steps);
        const size_t origin_length = from->length;
        auto buf = std::make_unique<signal_buf>(origin_length + fadeout_steps);
        size_t read_cursor = from->loop_to;
        double amp = 1.0;

        // Perform copying into the new size of array
        for (size_t i = 0; i < origin_length; i++)
            buf[i] = from->buf[static_cast<std::ptrdiff_t>(i)];

        for (size_t i = 0; i < fadeout_steps; i++)
        {
            amp -= step_frac;
            buf[origin_length + i] = static_cast<float>(from->buf[static_cast<std::ptrdiff_t>(read_cursor)] * amp);
            if (++read_cursor == from->loop_at) read_cursor = from->loop_to;
        }

        return std::make_unique<queueable>(queueable {
            .buf = std::move(buf),
            .length = origin_length + fadeout_steps,
            .loop_at = from->loop_at,
            .loop_to = from->loop_to,
            .position = 0
        });
    }
}
