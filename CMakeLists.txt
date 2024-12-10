cmake_minimum_required(VERSION 3.30)

project(pltwfinal)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -Werror")

add_subdirectory("less-WiringPi/wiringPi")
add_executable(pltwfinal main.cpp)
target_link_libraries(pltwfinal wiringPi)
target_include_directories(pltwfinal PRIVATE "less-WiringPi/wiringPi")
