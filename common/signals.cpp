//
// Created by penguinencounter on 1/6/25.
//

#include "signals.h"
#include <cmath>
#include <random>
#include <iostream>

namespace microsynth {
    namespace {
        inline std::mt19937 &rng() {
            static thread_local std::mt19937 gen(std::random_device{}());
            return gen;
        }

        // A function to generate integers in the range [min, max]
        template<typename T, std::enable_if_t<std::is_integral_v<T> >* = nullptr>
        T rand(T min, T max) {
            std::uniform_int_distribution<T> dist(min, max);
            return dist(rng());
        }

        // A function to generate floats in the range [min, max)
        template<typename T, std::enable_if_t<std::is_floating_point_v<T> >* = nullptr>
        T rand(T min, T max) {
            std::uniform_real_distribution<T> dist(min, max);
            return dist(rng());
        }
    }

    void SignalGenerators::setSampleRate(const int hz) {
        sampleRate = static_cast<std::size_t>(hz);
        secondsPerSample = 1.0 / hz;
    }

    void SignalGenerators::setSampleRate(const std::size_t hz) {
        sampleRate = hz;
        secondsPerSample = 1.0 / static_cast<int>(hz);
    }

    size_t SignalGenerators::getSampleRate() const {
        return sampleRate;
    }

    static constexpr double TAU = 6.283185307179586;

    std::unique_ptr<generic_clip> SignalGenerators::sine(const double freq, const double amplitude) const {
        return std::make_unique<generic_clip>(generic_clip{
            .clip_type = generic_clip::clip_type_t::EXACT,
            .data = {
                exact_clip{
                    .getPCM = [=]([[maybe_unused]] const std::shared_ptr<generic_clip> &that, const PaTime base_time,
                                  const std::size_t offset) {
                        const double real_t = (static_cast<double>(offset) / static_cast<double>(sampleRate)) + base_time;
                        return static_cast<float>(std::sin(real_t * TAU * freq) * amplitude);
                    }
                }
            },
            .sample_post = [=](const std::shared_ptr<generic_clip> &that, [[maybe_unused]] PaTime _1,
                               [[maybe_unused]] std::size_t _2) {
                that->volume = 1.0;
            },
            .stop_sample_post = [=](const std::shared_ptr<generic_clip> &that, [[maybe_unused]] PaTime base_time,
                                    PaTime stop_time, std::size_t offset) {
                if (stop_time > 0.05) that->alive = false;
                const double real_t = static_cast<double>(offset) / static_cast<double>(sampleRate) + stop_time;
                that->volume *= std::max(0.0, 20.0 * (0.05 - real_t));
            }
        });
    }

    std::unique_ptr<generic_clip> SignalGenerators::square(const double freq, const double amplitude) const {
        const double period = 1.0 / freq;
        return std::make_unique<generic_clip>(generic_clip{
            .clip_type = generic_clip::clip_type_t::EXACT,
            .data = {
                exact_clip{
                    .getPCM = [=]([[maybe_unused]] const std::shared_ptr<generic_clip> &that, const PaTime base_time,
                                  const std::size_t offset) {
                        const double real_t = static_cast<double>(offset) / static_cast<double>(sampleRate) + base_time;
                        return static_cast<float>(
                            (std::fmod(real_t, period) < (0.5 * period)) ? amplitude : -amplitude);
                    }
                }
            },
            .sample_post = [=](const std::shared_ptr<generic_clip> &that, [[maybe_unused]] PaTime _1,
                               [[maybe_unused]] std::size_t _2) {
                that->volume = 1.0;
            },
            .stop_sample_post = [=](const std::shared_ptr<generic_clip> &that, [[maybe_unused]] PaTime base_time,
                                    PaTime stop_time, std::size_t offset) {
                if (stop_time > 0.05) that->alive = false;
                const double real_t = static_cast<double>(offset) / static_cast<double>(sampleRate) + stop_time;
                that->volume *= std::max(0.0, 20.0 * (0.05 - real_t));
            }
        });
    }
}
