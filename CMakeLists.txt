cmake_minimum_required(VERSION 3.12)
project(AlgoViz
        VERSION 0.1
        DESCRIPTION "A program which visualizes algorithms (sorting, graph based stuff, etc.)")

set(_project_name AlgoViz)
set(CMAKE_CXX_STANDARD 17)

# third party libraries

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/GLFW)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/spdlog)

find_package(OpenGL REQUIRED)

include(${CMAKE_CURRENT_SOURCE_DIR}/src/CMakeLists.txt)

set(_AlgoViz_extra_src_files
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glad/src/glad.c
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_demo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_draw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_tables.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/imgui_widgets.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends/imgui_impl_glfw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui/backends/imgui_impl_opengl3.cpp)

add_executable(${_project_name} ${_AlgoViz_srcs} ${_AlgoViz_extra_src_files} ${_AlgoViz_hdrs})

set(_AlgoViz_compile_flags
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
  -Werror -Wall -Wextra -pedantic>
  $<$<CXX_COMPILER_ID:MSVC>:
  /WX /W4>)

target_compile_options(${_project_name} PRIVATE ${_AlgoViz_compile_flags})

target_link_libraries(${_project_name} ${OpenGL_LIBRARIES} glfw spdlog)

set(_AlgoViz_private_include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${OPENGL_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/GLFW/include
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/spdlog/include
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glad/include
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui)

target_include_directories(${_project_name} PRIVATE ${_AlgoViz_private_include_dirs})
