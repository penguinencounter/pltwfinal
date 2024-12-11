# https://cmake.org/cmake/help/book/mastering-cmake/chapter/Cross%20Compiling%20With%20CMake.html


# Make it cross-compile for aarch64
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

message("Setting up toolchain (target aarch64, host ${CMAKE_HOST_SYSTEM_PROCESSOR})")

# do we need to use this anyway?
if (NOT (${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "aarch64"))
    # locate an appropriate cxx compiler
    find_program(CrossCCompiler "aarch64-linux-gnu-gcc")
    find_program(CrossCXXCompiler "aarch64-linux-gnu-g++-12" "aarch64-linux-gnu-g++")

    message("Using ${CrossCCompiler} to target aarch64 [C]")
    message("Using ${CrossCXXCompiler} to target aarch64 [C++]")

    set(CMAKE_C_COMPILER ${CrossCCompiler})
    set(CMAKE_CXX_COMPILER ${CrossCXXCompiler})

    # adjust the default behavior of the FIND_XXX() commands:
    # search programs in the host environment
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

    # search headers and libraries in the target environment
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
else()
    message("Already on the target architecture, using host compilers and libraries")
endif ()