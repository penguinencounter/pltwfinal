//
// Created by penguinencounter on 12/11/24.
//

#ifndef PI_NATIVE_H
#define PI_NATIVE_H
#include <exception>

namespace microsynth_hw {
    class Hardware {
    private:
        int i2cfd {-1};

    public:
        constexpr static int LCD_ADDR = 0x27;

        Hardware();
        ~Hardware();

        bool switch_i2c_dev(int new_device) const;
    };
}

#endif //PI_NATIVE_H
