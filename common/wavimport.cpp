#include "wavimport.h"

#include <iostream>
#include <cstdint>

namespace extras
{
    namespace
    {
        using namespace microsynth;
    }

    std::unique_ptr<queueable> import_wav(const std::string& path)
    {
        std::ifstream in{path, std::ios_base::in | std::ios_base::binary};
        // skip to 40, where the size is
        in.seekg(40);
        char sizebuf[4] {};
        std::uint8_t sizebuf_actual[4] {};
        in.read(sizebuf, 4);
        for (int i = 0; i < 4; i++)
            sizebuf_actual[i] = static_cast<unsigned char>(sizebuf[i]);
        const std::size_t size = sizebuf_actual[0] | sizebuf_actual[1] << 8u | sizebuf_actual[2] << 16u | sizebuf_actual[3] << 24u;
        const std::size_t out_buffer_size = size / 2;
        auto buf = std::make_unique<signal_buf>(out_buffer_size);
        for (std::size_t i = 0; i < out_buffer_size; i++)
        {
            char valuebuf[2] {};
            in.read(valuebuf, 2);
            const auto value = static_cast<std::int16_t>(static_cast<unsigned char>(valuebuf[0]) | static_cast<unsigned char>(valuebuf[1]) << 8);
            buf[i] = static_cast<float>(value) / 32767.0f;
        }
        return std::make_unique<queueable>(queueable{
            .buf = std::move(buf),
            .length = out_buffer_size,
            .capabilities = {
                .looping = false
            }
        });
    }
}
