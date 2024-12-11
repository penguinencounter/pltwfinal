cmake_minimum_required(VERSION 3.30)

project(microsynthTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror")

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    set(TargetArch "aarch64")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    set(TargetArch "x64")
else()
    message(FATAL_ERROR "Could not detect the current architecture (maybe ${CMAKE_SYSTEM_PROCESSOR} isn't supported?)")
endif()

add_subdirectory("../common" "common_test")

add_executable(microsynthTest main_test.cpp)
target_link_libraries(microsynthTest common portaudio)
target_include_directories(microsynthTest PRIVATE "../common")
target_include_directories(microsynthTest PRIVATE "../portaudio_x64/include")
