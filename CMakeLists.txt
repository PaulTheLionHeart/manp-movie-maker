cmake_minimum_required(VERSION 3.23)

project(ManpMovieMaker LANGUAGES C CXX RC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY
    "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_executable(ManpMovieMaker WIN32
    src/ManpMovieMaker.cpp
    src/crop.cpp
    src/Dib.cpp
    src/Nextfile.cpp
    src/PNG.cpp
    src/Resize.cpp
    src/Jpgview.c
    src/ManpMovieMaker.rc
)

target_include_directories(ManpMovieMaker PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

add_subdirectory(jpeglib)
add_subdirectory(pnglib)
add_subdirectory(zlib)

target_link_libraries(ManpMovieMaker PRIVATE
    pnglib
    zlib
    jpeglib
    comctl32
)

#
# Force Visual Studio to link project dependencies.
#
set_target_properties(ManpMovieMaker PROPERTIES
    VS_GLOBAL_LinkLibraryDependencies true
)