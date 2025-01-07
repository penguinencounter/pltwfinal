
message("common arch: ${TargetArch} in ${CMAKE_SOURCE_DIR}")

add_library(common STATIC libmicrosynth.h libmicrosynth.cpp audio.h audio.cpp signals.h signals.cpp helpers.h)
#target_link_libraries(portaudio)

add_library(portaudio SHARED IMPORTED GLOBAL)
set_target_properties(portaudio PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/../portaudio_${TargetArch}/lib/libportaudio.so")

target_link_libraries(common PRIVATE portaudio)
target_link_libraries(common PRIVATE "pthread")
target_include_directories(common PRIVATE "${CMAKE_SOURCE_DIR}/../portaudio_${TargetArch}/include")
