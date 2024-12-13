//
// Created by penguinencounter on 12/13/24.
//

#ifndef HELPERS_H
#define HELPERS_H
#include <stdexcept>
#include <memory>
#include <string>

[[noreturn]]
inline void TODO() {
    throw std::runtime_error("Not yet implemented");
}

// template<typename... Args>
// std::string strformat(const std::string &format, Args... args) {
//     const int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
//     if (size_s <= 0) throw std::runtime_error("Formatting error");
//     const auto size = static_cast<size_t>(size_s);
//     const std::unique_ptr<char[]> buf(new char[size]);
//     std::snprintf(buf.get(), size, format.c_str(), args...);
//     return { buf.get(), buf.get() + size - 1 };
// }

#endif //HELPERS_H
