#include "ms_ads1115.h"

#include <thread>

namespace microsynth_hw
{
    std::uint16_t ms_ads1115::conf::pack() const
    {
        // ReSharper disable CppRedundantParentheses
        return (this->status_set << 15)
            | (this->mux & 0b111) << 12
            | (this->pga & 0b111) << 9
            | this->mode << 8
            | (this->data_rate & 0b111) << 5
            | this->is_window_comp << 4
            | this->comp_active_level << 3
            | this->comp_latch << 2
            | (this->comp_qd & 0b11);
        // ReSharper restore CppRedundantParentheses
    }

    ms_ads1115::conf ms_ads1115::conf::unpack(const std::uint16_t value)
    {
        return {
            static_cast<bool>(value >> 15 & 1),
            static_cast<std::uint8_t>(value >> 12 & 0b111),
            static_cast<std::uint8_t>(value >> 9 & 0b111),
            static_cast<bool>(value >> 8 & 1),
            static_cast<std::uint8_t>(value >> 5 & 0b111),
            static_cast<bool>(value >> 4 & 1),
            static_cast<bool>(value >> 3 & 1),
            static_cast<bool>(value >> 2 & 1),
            static_cast<std::uint8_t>(value & 0b11),
        };
    }

    ms_ads1115::ms_ads1115(Hardware& hw, const int pin_id):
        hw(hw), i2cfd(wiringPiI2CSetup(pin_id)), pin_id(pin_id), current_configuration(fetch_conf())
    {
    }

    std::uint16_t ms_ads1115::fetch_conf_raw() const
    {
        return last_synced = static_cast<std::uint16_t>(wiringPiI2CReadReg16(i2cfd, REG_CONFIG));
    }

    ms_ads1115::conf ms_ads1115::fetch_conf() const
    {
        return conf::unpack(fetch_conf_raw());
    }

    int ms_ads1115::write_conf_raw(const std::uint16_t it) const
    {
        last_synced = it;
        return wiringPiI2CWriteReg16(i2cfd, REG_CONFIG, it);
    }

    int ms_ads1115::write_conf(const conf& it) const
    {
        return write_conf_raw(it.pack());
    }

    void ms_ads1115::pull_conf()
    {
        current_configuration = fetch_conf();
    }

    int ms_ads1115::push_conf() const
    {
        const std::uint16_t value = current_configuration.pack() & 0b0111111111111111;
        if (value == last_synced) return 0;
        last_synced = value;
        return write_conf_raw(value);
    }

    std::int16_t ms_ads1115::get_conversion() const
    {
        return static_cast<std::int16_t>(wiringPiI2CReadReg16(i2cfd, REG_CONVERSION));
    }

    int ms_ads1115::pulse() const
    {
        return write_conf_raw(last_synced | (1 << 15));
    }

    void ms_ads1115::spinfetch() const
    {
        while ((fetch_conf_raw() & (1 << 15)) == 0) // spin while the device does its thing
        {
            std::this_thread::yield();
        }
    }


    std::int16_t ms_ads1115::convert_now() const
    {
        if (pulse() < 0) throw communication_failure();
        spinfetch();
        return get_conversion();
    }

    std::int16_t ms_ads1115::analog_read(const std::uint8_t channel) const
    {
        spinfetch();
        if (write_conf_raw(
            (last_synced & (0b0000111111111111)) // clear channel, OS
            | (channel & 0b11) << 12 // set the channel
            | (0b1100000000000000)   // use GND reference, start conversion
            ) < 0) throw communication_failure();
        spinfetch();
        return get_conversion();
    }
}
