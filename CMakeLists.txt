cmake_minimum_required(VERSION 3.0)

set(OUTPUT_BIN ON)

add_subdirectory("FlareEditor/")
add_subdirectory("FlareEngine/")

project(FlareEditorCS VERSION 0.1 LANGUAGES CSharp)

include(CSharpUtilities)

file(GLOB_RECURSE SOURCES "FlareEditorCS/src/*.cs")

add_executable(FlareEditorCS ${SOURCES})

set_target_properties(FlareEditorCS PROPERTIES VS_DOTNET_REFERENCES "System.Xml")
set_target_properties(FlareEditorCS PROPERTIES VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6")
set_target_properties(FlareEditorCS PROPERTIES WIN32_EXECUTABLE FALSE)
set_target_properties(FlareEditorCS
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/"
)

add_custom_command(TARGET FlareEditorCS
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/bin/FlareEditorCS.exe"
        "${CMAKE_SOURCE_DIR}/bin/FlareEditorCS.dll")

target_link_libraries(FlareEditorCS FlareCS)
