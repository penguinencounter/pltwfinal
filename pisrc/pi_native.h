//
// Created by penguinencounter on 12/11/24.
//

#ifndef PI_NATIVE_H
#define PI_NATIVE_H
#include <stdexcept>

namespace microsynth_hw {
    class communication_failure final : public std::runtime_error
    {
    public:
        communication_failure();
        explicit communication_failure(const std::string& extra);
    };

    class Hardware {
    private:
        mutable int i2cfd {-2};

    public:
        constexpr static int LCD_ADDR = 0x27;

        Hardware();
        ~Hardware();
    };
}

#endif //PI_NATIVE_H
