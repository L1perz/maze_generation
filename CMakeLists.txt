cmake_minimum_required(VERSION 3.14)
project(MazeExplorer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML-2.6.1)
add_executable(MazeExplorer src/main.cpp src/Maze.cpp src/Generator.cpp src/BFSStrategy.cpp src/Solver.cpp)
target_include_directories(MazeExplorer PRIVATE include ${SFML_ROOT}/include)
target_link_directories(MazeExplorer PRIVATE ${SFML_ROOT}/lib)
target_link_libraries(MazeExplorer PRIVATE ${SFML_ROOT}/lib/sfml-graphics.lib ${SFML_ROOT}/lib/sfml-window.lib ${SFML_ROOT}/lib/sfml-system.lib)

include(FetchContent)
FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.5.2)
FetchContent_MakeAvailable(Catch2)

enable_testing()

file(GLOB TEST_FILES "tests/*.cpp")
foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} PRIVATE MazeCore Catch2::Catch2WithMain)
    target_include_directories(${TEST_NAME} PRIVATE include)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()