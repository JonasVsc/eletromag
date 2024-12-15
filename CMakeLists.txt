cmake_minimum_required(VERSION 3.28)
project(Electromagnetism)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)        
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("C:/emsdk/upstream/emscripten/cache/sysroot/include")

add_executable(ElectroApp 
    "application/src/main.cpp" 

    "application/src/application.h"
    "application/src/application.cpp"

    "application/src/window.h"
    "application/src/window.cpp"

    "application/src/resource-manager.h"
    "application/src/resource-manager.cpp"

    "application/src/glfw3webgpu.h"
    "application/src/glfw3webgpu.c"

    "application/src/webgpu-utils.h"
    "application/src/webgpu-utils.cpp"

    "application/src/tiny_obj_loader.h"
    "application/src/stb_image.h"

    "application/src/camera.h"
    "application/src/camera.cpp"

    "application/src/layer.h"
    "application/src/layer.cpp"

    "application/src/layer-stack.h"
    "application/src/layer-stack.cpp"

    "application/src/imgui-layer.h"
    "application/src/imgui-layer.cpp"

    "application/src/scene-layer.h"
    "application/src/scene-layer.cpp"

    "application/src/wgpu-pipeline.h"
    "application/src/wgpu-pipeline.cpp"
    
    "application/src/wgpu-renderer2.h"
    "application/src/wgpu-renderer2.cpp"

    "application/src/object.h"
)

add_subdirectory("application/vendor/glm")
add_subdirectory("application/vendor/imgui")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")

    target_link_options(ElectroApp PRIVATE -sUSE_WEBGPU=1 -sWASM=1 -sUSE_GLFW=3 -sASYNCIFY -sNO_DISABLE_EXCEPTION_CATCHING -sALLOW_MEMORY_GROWTH)

    configure_file(${CMAKE_SOURCE_DIR}/application/src/index.html ${CMAKE_BINARY_DIR}/index.html COPYONLY)

    target_compile_definitions(ElectroApp PRIVATE
        RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/application/resources"
    )

    target_link_options(ElectroApp PRIVATE
        --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/application/resources"
    )

    target_link_libraries(ElectroApp PRIVATE glm imgui)

endif()

