cmake_minimum_required(VERSION 3.23)

project(jpeglib C)

set(JPEG_SOURCES
    jaricom.c
    jcapimin.c
    jcapistd.c
    jcarith.c
    jccoefct.c
    jccolor.c
    jcdctmgr.c
    jchuff.c
    jcinit.c
    jcmainct.c
    jcmarker.c
    jcmaster.c
    jcomapi.c
    jcparam.c
    jcprepct.c
    jcsample.c
    jctrans.c
    jdatadst.c
    jfdctflt.c
    jfdctfst.c
    jfdctint.c
    jdapimin.c
    jdapistd.c
    jdarith.c
    jdatasrc.c
    jdcoefct.c
    jdcolor.c
    jddctmgr.c
    jdhuff.c
    jdinput.c
    jdmainct.c
    jdmarker.c
    jdmaster.c
    jdmerge.c
    jdpostct.c
    jdsample.c
    jdtrans.c
    jidctflt.c
    jidctfst.c
    jidctint.c
    jquant1.c
    jquant2.c
    jerror.c
    jmemmgr.c
    jmemnobs.c
    jutils.c
)

add_library(jpeglib STATIC ${JPEG_SOURCES})

target_include_directories(jpeglib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(jpeglib PRIVATE
    WIN32
    _LIB
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_WARNINGS
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

if(MSVC)
    set_source_files_properties(${JPEG_SOURCES} PROPERTIES LANGUAGE C)
endif()

set_target_properties(jpeglib PROPERTIES
    OUTPUT_NAME JPEGLIB
)