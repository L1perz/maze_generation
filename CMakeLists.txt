cmake_minimum_required(VERSION 3.14)
project(MazeExplorer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/FS)
endif()

set(SFML_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML-2.6.1)
set(PROJECT_SOURCES
    src/Maze.cpp
    src/Generator.cpp
    src/BFSStrategy.cpp
    src/DFSStrategy.cpp
    src/AStarStrategy.cpp
    src/Solver.cpp
)

add_executable(MazeExplorer src/main.cpp ${PROJECT_SOURCES})
target_include_directories(MazeExplorer PRIVATE include)

if(WIN32 AND MSVC)
    target_include_directories(MazeExplorer PRIVATE ${SFML_ROOT}/include)
    target_link_libraries(MazeExplorer PRIVATE
        ${SFML_ROOT}/lib/sfml-graphics.lib
        ${SFML_ROOT}/lib/sfml-window.lib
        ${SFML_ROOT}/lib/sfml-system.lib
    )
    add_custom_command(TARGET MazeExplorer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SFML_ROOT}/bin/sfml-graphics-2.dll
            ${SFML_ROOT}/bin/sfml-window-2.dll
            ${SFML_ROOT}/bin/sfml-system-2.dll
            $<TARGET_FILE_DIR:MazeExplorer>
    )
    add_custom_command(TARGET MazeExplorer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SFML_ROOT}/examples/tennis/resources/tuffy.ttf
            $<TARGET_FILE_DIR:MazeExplorer>
    )
elseif(WIN32)
    message(FATAL_ERROR "The bundled SFML libraries are built for MSVC. Configure with Visual Studio, for example: cmake -S . -B build -G \"Visual Studio 17 2022\" -A x64")
else()
    find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
    target_link_libraries(MazeExplorer PRIVATE sfml-graphics sfml-window sfml-system)
endif()

include(CTest)

if(BUILD_TESTING)
    find_package(Catch2 3 QUIET)

    if(NOT Catch2_FOUND)
        include(FetchContent)
        FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.5.2)
        FetchContent_MakeAvailable(Catch2)
    endif()

    file(GLOB TEST_FILES "tests/*.cpp")
    foreach(TEST_FILE ${TEST_FILES})
        get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_FILE} ${PROJECT_SOURCES})
        target_link_libraries(${TEST_NAME} PRIVATE Catch2::Catch2WithMain)
        target_include_directories(${TEST_NAME} PRIVATE include)
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endforeach()
endif()
