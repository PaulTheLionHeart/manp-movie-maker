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

add_subdirectory(jpeglib)
add_subdirectory(zlib)
add_subdirectory(pnglib)

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
    ${CMAKE_SOURCE_DIR}/pnglib
    ${CMAKE_SOURCE_DIR}/zlib
)

# ---------------------------------------------------------
# Force the executable to depend on the library targets
# so they build first
# ---------------------------------------------------------

add_dependencies(ManpMovieMaker jpeglib zlib pnglib)

# ---------------------------------------------------------
# Link system libraries only in the normal way
# ---------------------------------------------------------

target_link_libraries(ManpMovieMaker PRIVATE
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
# Link the three MMM libraries explicitly by file path,
# matching the old VC++ project style as closely as possible.
# ---------------------------------------------------------

target_link_options(ManpMovieMaker PRIVATE
    $<$<CONFIG:Debug>:
        "${CMAKE_BINARY_DIR}/pnglib/Debug/PNGLib.lib"
        "${CMAKE_BINARY_DIR}/zlib/Debug/ZLib.lib"
        "${CMAKE_BINARY_DIR}/jpeglib/Debug/JPEGLIB.lib"
        /NODEFAULTLIB:LIBCMT.lib
        /NODEFAULTLIB:MSVCRTD.lib
    >
    $<$<CONFIG:Release>:
        "${CMAKE_BINARY_DIR}/pnglib/Release/PNGLib.lib"
        "${CMAKE_BINARY_DIR}/zlib/Release/ZLib.lib"
        "${CMAKE_BINARY_DIR}/jpeglib/Release/JPEGLIB.lib"
    >
)

set_target_properties(ManpMovieMaker PROPERTIES
    VS_GLOBAL_LinkLibraryDependencies true
)