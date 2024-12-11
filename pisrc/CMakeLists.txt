cmake_minimum_required(VERSION 3.30)

project(microsynthPi)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ")

set(TargetArch "aarch")

add_subdirectory("../less-WiringPi/wiringPi" "wiringPi_pisrc")
add_subdirectory("../common" "common_pi")

add_executable(microsynthPi main_pi.cpp)
target_link_libraries(microsynthPi wiringPi common portaudio)

target_include_directories(microsynthPi PRIVATE "../common")
target_include_directories(microsynthPi PRIVATE "../less-WiringPi/wiringPi")
target_include_directories(microsynthPi PRIVATE "../portaudio_aarch/include")
