file(GLOB JPEG_SOURCES
    *.c
)

add_library(jpeglib STATIC ${JPEG_SOURCES})

target_include_directories(jpeglib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)