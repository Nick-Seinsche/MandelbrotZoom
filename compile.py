src = """

build90 {
    g++ "mandelbrot.c" "res/resource.o"
    -o "mandelbrot90"
    -I "SDL2-2.0.9/i686-w64-mingw32/include"
    -L "SDL2-2.0.9/i686-w64-mingw32/lib"
    -lSDL2main
    -lSDL2
    -Wall
    -pg
    -O
    -Wl,--subsystem,windows
    -w
}

build45 {
    g++ "mandelbrot.c" "res/resource.o"
    -o "mandelbrot45"
    -I "SDL2-2.0.9/i686-w64-mingw32/include"
    -L "SDL2-2.0.9/i686-w64-mingw32/lib"
    -lSDL2main
    -lSDL2
    -Wall
    -pg
    -O
    -Wl,--subsystem,windows
    -w
}
"""
"""

whatever {
    -Wl,--subsystem,windows
    -w
    -mwindows
    -fopenmp
    -lmingw32
    -lSDL2main
    -lSDL2
    -lglew32
    -I "C:\MinGW\include"
    -L "C:\MinGW\lib"}
    -I "C:/OCL_SDK_Light/include"
    -L "C:/OCL_SDK_Light/lib"
    "C:/OCL_SDK_Light/lib/x86/opencl.lib"

"""

import sys
import os
import re

if __name__ == "__main__":
    # Cmake3.py debug
    dir = os.getcwd()
    args = sys.argv
    if len(args) == 2:
        mode = args[1]
    else:
        print("""Not enough arguments. Try:\n
              python MakeFiles/Cmake.py debug""")
        src = src.replace("\n", "")

    mode = r"(?<=(" + re.escape(mode) + r"\s{))([^}])*"
    execute_code = re.search(mode, src).group(0)
    execute_code = re.sub("\s\s+", " ", execute_code)

    os.chdir(dir)
    os.system(execute_code)
