cmake_minimum_required(VERSION 3.14)
project(AlgorithmVisualizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 1. Declare SFML
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)

# 2. Declare ImGui Core
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90
)

# 3. Declare ImGui-SFML Wrapper
FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG v2.6
)

# --- THE FIX STARTS HERE ---
# We need to "Populate" imgui first to get its source directory
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()

# Now tell imgui-sfml where those files are
set(IMGUI_DIR "${imgui_SOURCE_DIR}" CACHE PATH "Path to ImGui" FORCE)
set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "" FORCE)

# 4. Declare GTest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Now make everything available
FetchContent_MakeAvailable(SFML imgui-sfml googletest)
# --- THE FIX ENDS HERE ---

# Source files for core logic
set(CORE_SOURCES
    src/Model/Graph.cpp
    src/Algorithm/BFSStrategy.cpp
)

add_executable(AlgorithmVisualizer main.cpp ${CORE_SOURCES})
target_include_directories(AlgorithmVisualizer PRIVATE 
    include 
    include/Model 
    include/Algorithm 
    include/Playback
)
target_link_libraries(AlgorithmVisualizer PRIVATE ImGui-SFML::ImGui-SFML)

# --- Unit Testing ---
enable_testing()
add_executable(RunTests tests/test_bfs.cpp ${CORE_SOURCES})
target_include_directories(RunTests PRIVATE 
    include 
    include/Model 
    include/Algorithm 
    include/Playback
)
target_link_libraries(RunTests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(RunTests)