add_library(zlib STATIC
    adler32.c
    compress.c
    crc32.c
    deflate.c
    inffast.c
    inflate.c
    inftrees.c
    trees.c
    uncompr.c
    zutil.c
)

target_include_directories(zlib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(zlib PRIVATE
    WIN32
    _WINDOWS
    _CRT_SECURE_NO_DEPRECATE
    _CRT_NON_CONFORMING_SWPRINTFS
    _CRT_NONSTDC_NO_WARNINGS
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

set_target_properties(zlib PROPERTIES
    LINKER_LANGUAGE C
    OUTPUT_NAME ZLib
)