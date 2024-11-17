cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(project_name VERSION 0.1.0 LANGUAGES CXX)

find_package( OpenGL REQUIRED )
include_directories( ${OPENGL_INCLUDE_DIRS} )

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_LOG_LEVEL ERROR)


#==============================================================================

# Set ImGui direcories

add_subdirectory(3rdparty/glfw)
add_subdirectory(3rdparty/glad)
add_subdirectory(3rdparty/glm)
add_subdirectory(application)
