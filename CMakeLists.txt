cmake_minimum_required(VERSION 3.28)
project(Electromagnetism)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)        
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("C:/emsdk/upstream/emscripten/cache/sysroot/include")

add_executable(ElectroApp 
    "application/src/main.cpp" 

    "application/src/app.h"
    "application/src/app.cpp"

    "application/src/window.h"
    "application/src/window.cpp"

    "application/src/wgpu-renderer.h"
    "application/src/wgpu-renderer.cpp"

    "application/src/resource-manager.h"
    "application/src/resource-manager.cpp"

    "application/src/glfw3webgpu.h"
    "application/src/glfw3webgpu.c"

    "application/src/webgpu-utils.h"
    "application/src/webgpu-utils.cpp"
)

add_subdirectory("application/vendor/glm")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")

    target_link_options(ElectroApp PRIVATE -sUSE_WEBGPU=1 -sWASM=1 -sUSE_GLFW=3 -sASYNCIFY -sNO_DISABLE_EXCEPTION_CATCHING)

    configure_file(${CMAKE_SOURCE_DIR}/application/src/index.html ${CMAKE_BINARY_DIR}/index.html COPYONLY)

    target_compile_definitions(ElectroApp PRIVATE
        RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/application/resources"
    )

    target_link_options(ElectroApp PRIVATE
        --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/application/resources"
    )

    target_link_libraries(ElectroApp PRIVATE glm)

endif()

