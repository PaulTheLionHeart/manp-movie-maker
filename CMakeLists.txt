# =========================================================
# ManpMovieMaker Root CMake Script
# Diagnostic build: link MMM against explicit built .lib files
# to mimic the working VC++ linker behaviour
# =========================================================

cmake_minimum_required(VERSION 3.23)

project(ManpMovieMaker LANGUAGES C CXX RC)

# ---------------------------------------------------------
# Language / runtime settings
# ---------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Match VS project:
#   Debug   -> /MTd
#   Release -> /MT
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# ---------------------------------------------------------
# Source files
# ---------------------------------------------------------

set(MANPMOVIEMAKER_SOURCES
    src/ManpMovieMaker.cpp
    src/Resize.cpp
    src/PNG.cpp
    src/Nextfile.cpp
    src/Jpgview.c
    src/Dib.cpp
    src/crop.cpp
    src/ManpMovieMaker.rc
)

# ---------------------------------------------------------
# Third-party libraries
# ---------------------------------------------------------

# Keep local JPEG library for now
add_subdirectory(jpeglib)

# Use vcpkg for zlib + libpng
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)

# ---------------------------------------------------------
# Main executable
# ---------------------------------------------------------

add_executable(ManpMovieMaker WIN32 ${MANPMOVIEMAKER_SOURCES})

target_compile_definitions(ManpMovieMaker PRIVATE
    WIN32
    _WINDOWS
    _CRT_SECURE_NO_WARNINGS
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

target_include_directories(ManpMovieMaker PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/jpeglib
)

# ---------------------------------------------------------
# Force the executable to depend on the local JPEG library
# so it builds first
# ---------------------------------------------------------

add_dependencies(ManpMovieMaker jpeglib)

# ---------------------------------------------------------
# Link system libraries only in the normal way
# ---------------------------------------------------------


target_link_libraries(ManpMovieMaker PRIVATE
    PNG::PNG
    ZLIB::ZLIB

    comctl32
    comdlg32
    shell32
    ole32
    uuid
    gdi32
    oldnames
    odbc32
    odbccp32
    htmlhelp
)

# ---------------------------------------------------------
# Link the local JPEG library explicitly for now.
# PNG and ZLIB now come from vcpkg via target_link_libraries().
# ---------------------------------------------------------

target_link_options(ManpMovieMaker PRIVATE
    $<$<CONFIG:Debug>:
        "${CMAKE_BINARY_DIR}/jpeglib/Debug/JPEGLIB.lib"
    >
    $<$<CONFIG:Release>:
        "${CMAKE_BINARY_DIR}/jpeglib/Release/JPEGLIB.lib"
    >
)

set_target_properties(ManpMovieMaker PROPERTIES
    VS_GLOBAL_LinkLibraryDependencies true
)