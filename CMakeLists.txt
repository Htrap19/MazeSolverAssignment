cmake_minimum_required(VERSION 3.5)

project(maze_solver_visualization LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

add_subdirectory(vendor)

# Include directories
include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    include
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui"
)

add_executable(maze_solver_visualization
    src/main.cpp
    src/glad/glad.c
    src/renderer.h src/renderer.cpp
    src/shader.h src/shader.cpp

    src/maze.h src/maze.cpp
    src/pathfinder.h src/pathfinder.cpp
    src/application.h src/application.cpp
    src/mazevisualizerapp.h src/mazevisualizerapp.cpp
    src/recursivebacktrackingmaze.h src/recursivebacktrackingmaze.cpp
    src/kruskalmaze.h src/kruskalmaze.cpp
)

target_link_libraries(maze_solver_visualization
    imgui
    ${OPENGL_LIBRARIES}
    glfw
    GLEW::GLEW)

include(GNUInstallDirs)
install(TARGETS maze_solver_visualization
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
