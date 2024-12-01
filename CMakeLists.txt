cmake_minimum_required(VERSION 3.28)
project(Electromagnetism)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("C:/emsdk/upstream/emscripten/cache/sysroot/include")
include_directories("application/vendor/glfw3webgpu")

add_executable(ElectroApp 
    "application/src/main.cpp" 
    "application/src/webgpu-utils.h"
    "application/src/webgpu-utils.cpp"
    "application/src/application.h" 
    "application/src/application.cpp"
)

add_subdirectory(application/vendor/glfw3webgpu)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")


    target_link_libraries(ElectroApp PRIVATE glfw3webgpu)

    target_link_options(ElectroApp PRIVATE -sUSE_WEBGPU=1 -sWASM=1 -sUSE_GLFW=3 -sASYNCIFY)

    configure_file(${CMAKE_SOURCE_DIR}/application/src/index.html ${CMAKE_BINARY_DIR}/index.html COPYONLY)

endif()