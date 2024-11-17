
set(3RDPARTY_DIR ../3rdparty)

set(GLAD_INCLUDE ${3RDPARTY_DIR}/glad/include)
set(ASIO_INCLUDE ${3RDPARTY_DIR}/asio/include)

include_directories(${3RDPARTY_DIR}/imgui)
include_directories(${3RDPARTY_DIR}/imgui/backends)

set(IMGUI_SRC 
	${3RDPARTY_DIR}/imgui/imgui.h
	${3RDPARTY_DIR}/imgui/imgui.cpp
	${3RDPARTY_DIR}/imgui/imgui_demo.cpp
	${3RDPARTY_DIR}/imgui/imgui_draw.cpp
	${3RDPARTY_DIR}/imgui/imgui_tables.cpp
	${3RDPARTY_DIR}/imgui/imgui_widgets.cpp
    ${3RDPARTY_DIR}/imgui/backends/imgui_impl_glfw.h   
	${3RDPARTY_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${3RDPARTY_DIR}/imgui/backends/imgui_impl_opengl3.h  
	${3RDPARTY_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(${PROJECT_NAME} src/main.cpp
	${IMGUI_SRC}
	${3RDPARTY_DIR}/stb_image.h
)



target_include_directories(${PROJECT_NAME} PUBLIC ${GLAD_INCLUDE} ${ASIO_INCLUDE})
target_link_libraries(${PROJECT_NAME} glfw glad glm)

# file(COPY ${CMAKE_SOURCE_DIR}/from_scratch/shaders DESTINATION ${CMAKE_BINARY_DIR})
