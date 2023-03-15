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

add_compile_definitions(GLM_FORCE_QUAT_DATA_XYZW)
add_compile_definitions(GLM_FORCE_RADIANS)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules")

include("FileToHeader")

FileToHeader(
    SOURCE_FILE "templates/AssemblyControl.cs"
    HEADER_FILE "include/Templates/AssemblyControlCS.h" 
    VARIABLE_NAME "AssemblyControlTemplateCS"
)

include_directories("${PROJECT_SOURCE_DIR}/../FlareEngine/cpp/Shared/"
                    "${PROJECT_SOURCE_DIR}/../FlareEngine/deps/flare-glfw/include/"
                    "${PROJECT_SOURCE_DIR}/../FlareEngine/deps/flare-glm/"
                    "${PROJECT_SOURCE_DIR}/../FlareEngine/deps/flare-mono/build/include/mono-2.0/"
                    "${PROJECT_SOURCE_DIR}/../FlareEngine/deps/flare-tinyxml2/"
                    "${PROJECT_SOURCE_DIR}/include/"
                    "${PROJECT_SOURCE_DIR}/lib/glad/include/"
                    "${PROJECT_SOURCE_DIR}/lib/imgui/"
                    "${PROJECT_SOURCE_DIR}/lib/imgui/backends/"
                    "${PROJECT_SOURCE_DIR}/lib/implot/"
                    "${PROJECT_SOURCE_DIR}/lib/stb/")

file(GLOB SOURCES 
    "src/*.cpp" 
    "../FlareEngine/deps/flare-tinyxml2/tinyxml2.cpp"
    "lib/glad/src/*.c" 
    "lib/imgui/*.cpp" 
    "lib/imgui/backends/imgui_impl_opengl3.cpp" 
    "lib/imgui/backends/imgui_impl_glfw.cpp" 
    "lib/implot/implot.cpp" 
    "lib/implot/implot_items.cpp")

add_subdirectory("${PROJECT_SOURCE_DIR}/../FlareEngine/deps/flare-glfw" "build/glfw")

link_directories("${PROJECT_SOURCE_DIR}/../FlareEngine/deps/mono/build/lib/")

add_executable(FlareEditor ${SOURCES})
target_link_libraries(FlareEditor glfw ${GLFW_LIBRARIES})

if (WIN32)
    target_link_libraries(FlareEditor wsock32 ws2_32)
endif()

if (MSVC)
    target_link_libraries(FlareEditor mono-2.0-sgen)
    set_property(TARGET FlareEditor PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
else()
    target_link_libraries(FlareEditor mono-2.0)
endif()
