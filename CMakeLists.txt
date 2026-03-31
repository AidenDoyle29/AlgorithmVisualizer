cmake_minimum_required(VERSION 3.14)
project(AlgorithmVisualizer)

set(CMAKE_CXX_STANDARD 11)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)

set(IMGUI_DIR imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(AlgorithmVisualizer main.cpp ${IMGUI_SOURCES})

target_include_directories(AlgorithmVisualizer PRIVATE ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(AlgorithmVisualizer glfw OpenGL::GL)