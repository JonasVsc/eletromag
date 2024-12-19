cmake_minimum_required(VERSION 3.28)
project(Electromagnetism)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)        
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("C:/emsdk/upstream/emscripten/cache/sysroot/include")
include_directories("${CMAKE_TOOLCHAIN_FILE}/src")

add_executable(ElectroApp 
    "application/src/main.cpp" 

    "application/src/application.h"
    "application/src/application.cpp"

    "application/src/window.h"
    "application/src/window.cpp"

    # UTILS
    # -----
    "application/src/utils/resource-manager.h"
    "application/src/utils/resource-manager.cpp"
    "application/src/utils/glfw3webgpu.h"
    "application/src/utils/glfw3webgpu.c"
    "application/src/utils/webgpu-utils.h"
    "application/src/utils/webgpu-utils.cpp"
    "application/src/utils/tiny_obj_loader.h"
    "application/src/utils/stb_image.h"

    "application/src/camera.h"
    "application/src/camera.cpp"

    "application/src/layer.h"
    "application/src/layer.cpp"

    "application/src/layer-stack.h"
    "application/src/layer-stack.cpp"

    "application/src/imgui-layer.h"
    "application/src/imgui-layer.cpp"

    "application/src/wgpu-renderer2.h"
    "application/src/wgpu-renderer2.cpp"

    "application/src/object.h"
    "application/src/object.cpp"

    "application/src/scene.h"
    "application/src/scene.cpp"

    "application/src/topico1-scene.h"
    "application/src/topico1-physics.cpp"
    "application/src/topico1-physics.cpp"

    "application/src/topico2-scene.h"
    "application/src/topico2-physics.cpp"
    "application/src/topico2-physics.cpp"


    "application/src/physics.h"
    "application/src/physics.cpp"

    "application/src/component.h"
    "application/src/transform.h"
    "application/src/mesh.h"

)

add_subdirectory("application/vendor/glm")
add_subdirectory("application/vendor/imgui")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")

    target_link_options(ElectroApp PRIVATE -sUSE_WEBGPU=1 -sWASM=1 -sUSE_GLFW=3 -sASYNCIFY -sNO_DISABLE_EXCEPTION_CATCHING -sALLOW_MEMORY_GROWTH)

    configure_file(${CMAKE_SOURCE_DIR}/application/src/pages/index.html ${CMAKE_BINARY_DIR}/index.html COPYONLY)

    target_compile_definitions(ElectroApp PRIVATE
        RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/application/resources"
    )

    target_link_options(ElectroApp PRIVATE
        --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/application/resources"
    )

    target_link_libraries(ElectroApp PRIVATE glm imgui)

endif()

