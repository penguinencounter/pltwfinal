#ifndef ADS1115_H
#define ADS1115_H

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <cstdint>

#include "pi_native.h"

namespace microsynth_hw
{
// helpers
#define csi constexpr static int
#define csu8 constexpr static std::uint8_t

    class ms_ads1115
    {
    private:
        Hardware& hw;
        int i2cfd;
        mutable std::uint16_t last_synced = 0;

    public:
        class conf
        {
        public:
            // high bit (15)
            // Operational status (is the chip doing something??)
            // Write a 1 to start a conversion when we aren't doing anything
            bool status_set;

            // 14:12
            // first bit is 0:
                // 0: measure AIN0 vs AIN1
                // 1, 2, 3: measure AIN0/1/2 vs AIN3
            // first bit is 1:
                // 4, 5, 6, 7: measure AIN0/1/2/3 vs GND
            std::uint8_t mux;

            // 11:9
            // PGA.
            std::uint8_t pga;
            csu8 PGA_6_144V = 0b000; // watch out for overvoltage (bad!)
            csu8 PGA_4_096V = 0b001;
            csu8 PGA_2_048V = 0b010; // default
            csu8 PGA_1_024V = 0b011;
            csu8 PGA_0_512V = 0b100;
            csu8 PGA_0_256V = 0b101; // also, 0b110 and 0b111

            // 8
            // Operation mode (true: single-shot/poweroff [default]; false: continuous)
            bool mode;

            // 7:5
            // Data rate
            // units: SPS (samples per second)
            std::uint8_t data_rate;
            csu8 DATA_RATE_8 = 0;
            csu8 DATA_RATE_16 = 1;
            csu8 DATA_RATE_32 = 2;
            csu8 DATA_RATE_64 = 3;
            csu8 DATA_RATE_128 = 4; // default
            csu8 DATA_RATE_250 = 5;
            csu8 DATA_RATE_475 = 6;
            csu8 DATA_RATE_860 = 7;

            // 4
            // Comparator mode (true: window comp; false: traditional [default])
            bool is_window_comp;

            // 3
            // Comparator polarity (true: active high; false: active low [default])
            bool comp_active_level;

            // 2
            // Comparator latchiness
            // (true: latch - alert is latched until read or ACK'd, see spec)
            // (false: no latch [default])
            bool comp_latch;

            // 1:0
            // Comparator queue/disable, see enum and spec
            std::uint8_t comp_qd;
            csu8 COMP_QD_QUEUE1 = 0;
            csu8 COMP_QD_QUEUE2 = 1;
            csu8 COMP_QD_QUEUE4 = 2;
            csu8 COMP_QD_NOCOMP = 3;

            [[nodiscard]] std::uint16_t pack() const;
            static conf unpack(std::uint16_t value);
        };

        csi ADDR_GND = 0b1001000;
        csi ADDR_VDD = 0b1001001;
        csi ADDR_SDA = 0b1001010;
        csi ADDR_SCL = 0b1001011;

        csu8 REG_CONVERSION = 0b00;
        csu8 REG_CONFIG = 0b01;
        csu8 REG_LO_THRESH = 0b10;
        csu8 REG_HI_THRESH = 0b11;

        const int pin_id;
        conf current_configuration;

        explicit ms_ads1115(Hardware& hw, int pin_id);

        [[nodiscard]] std::uint16_t fetch_conf_raw() const;
        [[nodiscard]] conf fetch_conf() const;
        int write_conf_raw(std::uint16_t it) const; // NOLINT(*-use-nodiscard)
        int write_conf(const conf& it) const; // NOLINT(*-use-nodiscard)

        void pull_conf();
        // Not for pulsing the oneshot trigger. Synchronizes all other fields.
        int push_conf() const;

        [[nodiscard]] std::int16_t get_conversion() const;

        int pulse() const;
    private:
        void spinfetch() const;

    public:
        // These can block.
        [[nodiscard]] std::int16_t convert_now() const;
        // Read the channel indicated (0 to 3). Blocks until conversion is complete.
        [[nodiscard]] std::int16_t analog_read(std::uint8_t channel) const;
    };

#undef csi
#undef csu8
}

#endif //ADS1115_H
