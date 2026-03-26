cmake_minimum_required(VERSION 3.15)

project(VoronoievKovcheg)

set(CMAKE_CXX_STANDARD 17)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

add_executable(main main.cpp glad.c)
target_include_directories(main PRIVATE include external/glfw/include)
target_link_libraries(main PRIVATE glfw)

if(UNIX)
    target_link_libraries(main PRIVATE dl pthread)
endif()