#include "ms_ads1115.h"

#include <sstream>
#include <thread>

namespace microsynth_hw {
    using conf = ms_ads1115::conf;

    template<typename T>
    T wrong_endian(const T &in) {
        constexpr std::size_t size = sizeof(T);
        T result = 0;
        for (std::size_t dst = 0; dst < size; ++dst) {
            result <<= 8;
            result |= (in >> (dst * 8)) & 0xff;
        }
        return result;
    }

    std::uint16_t conf::pack() const {
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

    conf conf::unpack(const std::uint16_t value) {
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

    ms_ads1115::ms_ads1115(Hardware &hw, const int pin_id): hw(hw), i2cfd(wiringPiI2CSetup(pin_id)), pin_id(pin_id),
                                                            current_configuration(conf{}) {
    }

    std::uint16_t ms_ads1115::fetch_conf_raw() const {
        return last_synced = wrong_endian(static_cast<std::uint16_t>(wiringPiI2CReadReg16(i2cfd, REG_CONFIG)));
    }

    conf ms_ads1115::fetch_conf() const {
        return conf::unpack(fetch_conf_raw());
    }

    int ms_ads1115::write_conf_raw(const std::uint16_t it) const {
        last_synced = it;
        return wiringPiI2CWriteReg16(i2cfd, REG_CONFIG, wrong_endian(it));
    }

    int ms_ads1115::write_conf(const conf &it) const {
        return write_conf_raw(it.pack());
    }

    void ms_ads1115::pull_conf() {
        current_configuration = fetch_conf();
    }

    int ms_ads1115::push_conf() const {
        const std::uint16_t value = current_configuration.pack() & 0b0111111111111111;
        if (value == last_synced) return 0;
        last_synced = value;
        return write_conf_raw(value);
    }

    std::int16_t ms_ads1115::get_conversion() const {
        return wrong_endian(static_cast<std::int16_t>(wiringPiI2CReadReg16(i2cfd, REG_CONVERSION)));
    }

    int ms_ads1115::pulse() const {
        return write_conf_raw(last_synced | (1 << 15));
    }

    void ms_ads1115::spinfetch() const {
        while ((fetch_conf_raw() & (1 << 15)) == 0) // spin while the device does its thing
        {
            // If this actually yields (impl-defined & depending on current
            // conditions) then it will almost certainly be done in one loop
            // Otherwise, spinning is probably worth it anyway
            std::this_thread::yield();
        }
    }


    std::int16_t ms_ads1115::convert_now() const {
        if (pulse() < 0) throw communication_failure();
        spinfetch();
        return get_conversion();
    }

    std::int16_t ms_ads1115::analog_read(const std::uint8_t channel) const {
        if ((last_synced >> 12 & 0b111) != (channel | 0b100))
            // the next fetch is the wrong data, so we need to wait to clear it out
            spinfetch();
        if (write_conf_raw(
                (last_synced & (0b0000111111111111)) // clear channel, OS
                | (channel & 0b11) << 12 // set the channel
                | (0b1100000000000000) // use GND reference, start conversion
            ) < 0)
            throw communication_failure();
        spinfetch();
        return get_conversion();
    }

    std::ostream &operator<<(std::ostream &os, const conf &val) {
        std::stringstream ss{};
        ss << "{";
        if (val.status_set) ss << "idle";
        else ss << "inuse";
        ss << " ";
        switch (val.mux) {
            case 0:
                ss << "A0-A1";
                break;
            case 1:
            case 2:
            case 3:
                ss << "A" << (val.mux - 1) << "-A3";
                break;
            case 4:
            case 5:
            case 6:
            case 7:
                ss << "A" << (val.mux - 4) << "-GND";
                break;
            default:
                ss << "mux??";
        }
        ss << " ";
        switch (val.pga) {
            case conf::PGA_6_144V:
                ss << "6.144V";
                break;
            case conf::PGA_4_096V:
                ss << "4.096V";
                break;
            case conf::PGA_2_048V:
                ss << "2.048V";
                break;
            case conf::PGA_1_024V:
                ss << "1.024V";
                break;
            case conf::PGA_0_512V:
                ss << "0.512V";
                break;
            case conf::PGA_0_256V:
                ss << "0.256V";
                break;
            default:
                ss << "pga??";
                break;
        }
        ss << " ";
        if (val.mode) ss << "oneshot";
        else ss << "continuous";
        ss << " ";
        if (val.data_rate <= 4) {
            ss << 8 * (1 << val.data_rate) << "SPS";
        } else {
            switch (val.data_rate) {
                case 5:
                    ss << 250;
                    break;
                case 6:
                    ss << 475;
                    break;
                case 7:
                    ss << 860;
                    break;
                default:
                    ss << "???";
                    break;
            }
            ss << "SPS";
        }
        ss << " comp(";
        if (val.is_window_comp) ss << "window ";
        ss << (val.comp_active_level ? "high" : "low") << " ";
        if (val.comp_latch) ss << "latch ";
        switch (val.comp_qd) {
            case 0:
                ss << "queue=1)";
                break;
            case 1:
                ss << "queue=2)";
                break;
            case 2:
                ss << "queue=4)";
                break;
            case 3:
                ss << "disabled)";
                break;
            default:
                ss << "??";
                break;
        }
        ss << "}";
        return os << ss.str();
    }
}
