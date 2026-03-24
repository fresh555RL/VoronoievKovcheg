cmake_minimum_required(VERSION 3.15)

project(VoronoievKovcheg)

set(CMAKE_CXX_STANDARD 17)

# -----------------------------
# Настройки GLFW (ВАЖНО: до add_subdirectory)
# -----------------------------
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

# Если хочешь .dll/.so (динамика)
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

# -----------------------------
# Подключаем GLFW
# -----------------------------
add_subdirectory(glfw)

# -----------------------------
# GLAD как отдельная библиотека
# -----------------------------
add_library(glad src/glad.c) # если glad.c у тебя в src (см. ниже)
target_include_directories(glad PUBLIC include)

# -----------------------------
# Основной executable
# -----------------------------
file(GLOB SRC_FILES CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(main ${SRC_FILES})

# include директории
target_include_directories(main PRIVATE include)

# линковка
target_link_libraries(main
    glfw
    glad
)

# -----------------------------
# Linux: подлинковать OpenGL
# -----------------------------
if(UNIX AND NOT APPLE)
    target_link_libraries(main GL)
endif()

# -----------------------------
# Windows: копировать .dll рядом с exe
# -----------------------------
if(WIN32)
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:glfw>
        $<TARGET_FILE_DIR:main>
    )
endif()