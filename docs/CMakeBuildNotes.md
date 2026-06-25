# CMake Build Notes

## 2026-06-25 — Manp Movie Maker 2.0d CMake/vcpkg update

This update modernises the MMM CMake build and reduces dependence on legacy local library projects.

### Build system changes

* Added `vcpkg.json` manifest for external dependencies.
* Switched the CMake build to use **vcpkg static** versions of **zlib** and **libpng** (`x64-windows-static`).
* Retained the local **jpeglib** project for now.
* CMake Release build now produces a single self-contained executable without separate `libpng`/`zlib` DLLs.

### PNG compatibility fixes

* Replaced the old custom `TextData[]` comment-loading mechanism with standard libpng text handling via `png_get_text()`.
* Replaced direct reads of internal libpng structure fields (`read_ptr->width`, `read_ptr->height`, `read_ptr->pixel_depth`) with proper libpng API calls.
* This fixed an image-loading failure seen when using stock/vcpkg libpng, where some PNGs were being interpreted as 1-pixel-wide images in the CMake build.

### Result

* MMM 2.0d now builds cleanly with CMake + vcpkg static libraries.
* PNG comment/zoom metadata loading continues to work correctly with the standard libpng API.

## CMake build support and Win32 callback modernisation (23/06/2026)

This note records the work needed to get **ManpMovieMaker** building and running correctly using the new **CMake** build system.

### Summary

ManpMovieMaker now builds and runs correctly using CMake in both **Debug** and **Release** configurations. The major runtime failure was **not** caused by the JPEG, PNG or ZLib CMake libraries, but by legacy Win32 callback signatures in the application code.

### Initial symptom

The CMake-built executable would compile and link successfully, but when run it behaved incorrectly:

* the process appeared in Task Manager
* no visible main window appeared
* the application did not function normally

This initially suggested a possible build-system or library mismatch, but the real cause was elsewhere.

### Root cause

The main problem was the use of obsolete Win32 callback prototypes that were tolerated by the older Visual Studio project build but failed under the modern x64 CMake build.

The key change was updating the main window procedure from the old form:

```cpp
long FAR PASCAL WndProc(HWND hwnd, UINT message, UINT wParam, LONG lParam)
```

to the correct modern Win32 signature:

```cpp
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
```

The dialog procedures were also modernised from the old `BOOL FAR PASCAL` form to:

```cpp
INT_PTR CALLBACK AboutBoxDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SpecifyImageFileDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK CreateFFMPEGCommandDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
```

### Result

After updating the callback signatures, the CMake-built executable created its main window correctly and ran normally.

### Other notes

* The smaller CMake Release executable size turned out **not** to be a problem once the program was confirmed to run correctly.
* The old Visual Studio 2017 “Release” executable is significantly larger, probably due to historical project settings and retained debug/linker baggage rather than missing content in the CMake build.
* During this work, the obsolete `register` keyword in `Nextfile.cpp` was also removed.

### Outcome

As of **2.0d**, ManpMovieMaker has a working CMake build and can be built successfully from the command line using Visual Studio 2022.
