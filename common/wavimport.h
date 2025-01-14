#ifndef WAVIMPORT_H
#define WAVIMPORT_H

#include <fstream>

#include "signalmeta.h"

namespace extras
{

    [[nodiscard]] std::unique_ptr<microsynth::generic_clip> import_wav(const std::string& path);

}

#endif //WAVIMPORT_H
