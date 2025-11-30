cmake_minimum_required(VERSION 3.20)
project(SystemMonitor LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------- GLFW --------
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)

# -------- GLAD --------
add_library(glad STATIC
    external/glad/src/glad.c
)
target_include_directories(glad PUBLIC
    ${CMAKE_SOURCE_DIR}/external/glad/include
)

# -------- ImGui --------
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/imgui)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/imgui_impl_opengl3.cpp

    ${IMGUI_DIR}/implot.cpp
    ${IMGUI_DIR}/implot_items.cpp
    #${IMGUI_DIR}/implot_demo.cpp
)

# -------- Your Code --------
file(GLOB_RECURSE SYSTEMMONITOR_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE SYSTEMMONITOR_HEADERS
    "${CMAKE_SOURCE_DIR}/inc/*.h"
    "${CMAKE_SOURCE_DIR}/inc/*.hpp"
)

add_executable(SystemMonitor
    ${SYSTEMMONITOR_SOURCES}
    ${SYSTEMMONITOR_HEADERS}
    ${IMGUI_SOURCES}
)

target_include_directories(SystemMonitor PRIVATE
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/external/glad/include
    ${CMAKE_SOURCE_DIR}/external/glfw/include
    ${IMGUI_DIR}
)

target_link_libraries(SystemMonitor PRIVATE
    glad
    glfw
    opengl32
)
