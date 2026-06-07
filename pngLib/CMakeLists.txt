file(GLOB PNG_SOURCES
    *.c
)

add_library(pnglib STATIC ${PNG_SOURCES})

target_include_directories(pnglib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/zlib
)

target_link_libraries(pnglib PUBLIC zlib)