cmake_minimum_required(VERSION 3.20)
project(DSVisualizerGroup6)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(DSVisualizerGroup6
    src/main.cpp
    src/UI/UI.cpp
    src/structures/linked_list/LinkedList.cpp
    src/structures/linked_list/LinkedListRenderer.cpp
)

find_package(raylib CONFIG REQUIRED)
target_link_libraries(DSVisualizerGroup6 PRIVATE raylib)