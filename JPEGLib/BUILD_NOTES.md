# JPEGLIB jpeg-10 Visual Studio Project

This package contains a fresh `JPEGLIB.vcxproj` for building the IJG jpeg-10 library as a static library.

## Files included

- `JPEGLIB.vcxproj`
- `JPEGLIB.vcxproj.filters`
- `jconfig.h` copied directly from jpeg-10 `jconfig.vc`

## How to use

1. Extract `jpegsr10.zip`.
2. Copy these three files into the `jpeg-10` source directory.
3. Open/add `JPEGLIB.vcxproj` in Visual Studio.
4. Build `x64 Release` first.
5. Confirm the output is `JPEGLIB.lib`.
6. Point ManpMovieMaker at the new `JPEGLIB.lib` and rebuild.

## Project settings

- Static Library
- Platform toolset: `v141`
- Runtime library: `/MT` for Release, `/MTd` for Debug
- Compile as C
- Full library source set: compression, decompression, transform support, common support
- Uses `jmemnobs.c` as the single system-dependent memory manager

## Note

This project intentionally does not include the command-line programs (`cjpeg`, `djpeg`, `jpegtran`, `rdjpgcom`, `wrjpgcom`) or their helper modules. It builds only the library.

If Visual Studio asks to retarget the Windows SDK, accept the installed SDK version. Keep the runtime library consistent with ManpMovieMaker.
