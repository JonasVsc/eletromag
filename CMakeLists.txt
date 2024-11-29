cmake_minimum_required(VERSION 3.28)
project(Electromagnetism)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(ElectroApp application/src/application.cpp)

set(DAWN_FETCH_DEPENDENCIES ON)
add_subdirectory(application/vendor/dawn EXCLUDE_FROM_ALL)

target_link_libraries(ElectroApp PRIVATE dawn::webgpu_dawn glfw webgpu_glfw)

configure_file(${CMAKE_SOURCE_DIR}/application/src/index.html ${CMAKE_BINARY_DIR}/index.html COPYONLY)