cmake_minimum_required(VERSION 3.10)
project(MazeExplorer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Путь к SFML 2.6.1 (измените под свой)
set(CMAKE_PREFIX_PATH "E:/SFML-2.6.1")

find_package(SFML COMPONENTS graphics window system REQUIRED)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    sfml-graphics
    sfml-window
    sfml-system
)
