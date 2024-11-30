cmake_minimum_required(VERSION 3.28)
project(Electromagnetism)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("C:/emsdk/upstream/emscripten/cache/sysroot/include")

add_executable(ElectroApp 
    "application/src/main.cpp" 
    "application/src/webgpu-utils.h"
    "application/src/webgpu-utils.cpp"
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")

    set_target_properties(ElectroApp PROPERTIES SUFFIX ".html")

    target_link_options(ElectroApp PRIVATE -sUSE_WEBGPU=1 -sWASM=1 -sUSE_GLFW=3 -sASYNCIFY)

    # configure_file(${CMAKE_SOURCE_DIR}/application/src/index.html ${CMAKE_BINARY_DIR}/index.html COPYONLY)

endif()