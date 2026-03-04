call emsdk\emsdk_env.bat

:: find cpp files
@echo off
SETLOCAL EnableDelayedExpansion
set CPP_FILES=

for %%D in (Physics OLCEngine Math Game) do (
    for %%F in (%%D\*.cpp) do (
        set "CPP_FILES=!CPP_FILES! %%F"
    )
)

:: Trim leading space
set CPP_FILES=%CPP_FILES:~1%

echo %CPP_FILES%

:: create the build directory
set BUILD_DIR=wasm_build
mkdir "wasm_build" 2>nul

call em++ -O2 -o %BUILD_DIR%\space-combat-arena.html %CPP_FILES% --preload-file .\SoundPack.dat -s USE_LIBPNG=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_HEAP=536870912 --minify 0 -std=c++20 -s EXPORTED_RUNTIME_METHODS=HEAPF32