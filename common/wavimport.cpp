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
        std::uint32_t size = sizebuf_actual[0] | sizebuf_actual[1] << 8u | sizebuf_actual[2] << 16u | sizebuf_actual[3] << 24u;
        std::cout << size;
        return {nullptr};
    }
}
