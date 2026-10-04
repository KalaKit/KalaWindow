# Build from source

This document is only applicable if you are building this repository from source, if you see this document in a release package then you can ignore it.

## Prerequisites

- [Download KalaMake (1.4.0 or newer)](https://github.com/KalaKit/KalaMake/releases).
- [Download Python (3.11 or newer)](https://www.python.org/downloads/).
- [Clang (this or other versions, older versions should work too)](https://github.com/llvm/llvm-project/releases/tag/llvmorg-23.1.2)
- [Visual Studio or Build tools 2022/2026 (untested on older versions)](https://visualstudio.microsoft.com/downloads/)

*Note*: Visual Studio or Build Tools also requires you to select `Desktop development with C++` and enable `C++ ATL` and `C++ MFC` during installation or else some projects will not compile correctly!

## How to build from source

1) open your console in the folder where `project.kmake` is at.
2) type `python build.py sync` to sync dependencies, this needs to be done only once unless build.toml or project.kmake changes
3) type `python build.py build` to build all applicable targets for your OS (windows on windows, or windows-gnu and linux on linux),
add `windows`, `windows-gnu` or `linux` to target a specific build type

The compiled executable/binary and its files will be placed to `build/` inside the version folder with the name of the target you chose.
