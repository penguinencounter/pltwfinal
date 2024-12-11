cmake_minimum_required(VERSION 3.30)

project(microsynthTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror")

set(TargetArch "x64")

add_subdirectory("../common" "common_test")

add_executable(microsynthTest main_test.cpp)
target_link_libraries(microsynthTest common portaudio)
target_include_directories(microsynthTest PRIVATE "../common")
target_include_directories(microsynthTest PRIVATE "../portaudio_x64/include")
