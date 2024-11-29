cmake_minimum_required(VERSION 3.28)
project(Electromagnetism)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("C:/emsdk/upstream/emscripten/cache/sysroot/include")

add_executable(ElectroApp application/src/application.cpp)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")

    set(CMAKE_EXECUTABLE_SUFFIX ".html")  # Gera um arquivo .html

    target_link_options(ElectroApp PRIVATE -sUSE_WEBGPU=1 -sWASM=1)

endif()