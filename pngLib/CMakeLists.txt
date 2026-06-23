add_library(pnglib STATIC
    png.c
    pngerror.c
    pngget.c
    pngmem.c
    pngpread.c
    pngread.c
    pngrio.c
    pngrtran.c
    pngrutil.c
    pngset.c
    pngtrans.c
    pngwio.c
    pngwrite.c
    pngwtran.c
    pngwutil.c
)

target_include_directories(pnglib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/zlib
)

target_compile_definitions(pnglib PRIVATE
    WIN32
    DEBUG
    PNG_DEBUG=1
    _CRT_SECURE_NO_WARNINGS
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

target_link_libraries(pnglib PUBLIC zlib)

set_target_properties(pnglib PROPERTIES
    OUTPUT_NAME PNGLib
)