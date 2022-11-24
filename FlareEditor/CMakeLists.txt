cmake_minimum_required (VERSION 3.0)

project (FlareEditor VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT WIN32)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
    set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")
endif()

set (FLAREEDITOR_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
set (FLAREEDITOR_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})

configure_file(
    "${PROJECT_SOURCE_DIR}/FlareEditorConfig.h.in"
    "${PROJECT_SOURCE_DIR}/include/FlareEditorConfig.h"
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules")

include("FileToHeader")

FileToHeader(
    SOURCE_FILE "templates/AssemblyControl.cs"
    HEADER_FILE "include/Templates/AssemblyControlCS.h" 
    VARIABLE_NAME "AssemblyControlTemplateCS"
)

include_directories("${PROJECT_SOURCE_DIR}/../FlareEngine/cpp/Headless/"
                    "${PROJECT_SOURCE_DIR}/../FlareEngine/deps/mono/build/include/mono-2.0/"
                    "${PROJECT_SOURCE_DIR}/include/"
                    "${PROJECT_SOURCE_DIR}/lib/glad/include/"
                    "${PROJECT_SOURCE_DIR}/lib/glfw/include/"
                    "${PROJECT_SOURCE_DIR}/lib/glm/"
                    "${PROJECT_SOURCE_DIR}/lib/imgui/"
                    "${PROJECT_SOURCE_DIR}/lib/imgui/backends/"
                    "${PROJECT_SOURCE_DIR}/lib/tinyxml2/")

file(GLOB SOURCES "src/*.cpp" "lib/glad/src/*.c" "lib/imgui/*.cpp" "lib/imgui/backends/imgui_impl_opengl3.cpp" "lib/imgui/backends/imgui_impl_glfw.cpp" "lib/tinyxml2/tinyxml2.cpp")

add_subdirectory("lib/glfw")

add_executable(FlareEditor ${SOURCES})
target_link_libraries(FlareEditor glfw ${GLFW_LIBRARIES})

link_directories(${CMAKE_SOURCE_DIR}/../FlareEngine/deps/mono/build/lib/)
target_link_libraries(FlareEditor mono-2.0)