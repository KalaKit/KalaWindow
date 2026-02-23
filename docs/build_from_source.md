# Prerequisites for building from source

This document is only applicable if you are building this repository from source, if you see this document in a shared release package then you can ignore it.

## Windows only

Download Visual Studio or Visual Studio Build Tools:
https://visualstudio.microsoft.com/vs/

once its installed run the visual studio installer exe and enable this checkbox:
- desktop development with c++

and make sure these are toggled on at the right side panel:
- msvc build tools
- windows 11 sdk
- c++ cmake tools
- c++ atl
- c++ mfc
- c++ clang

## Windows and Linux

[Download](https://github.com/Kitware/CMake/releases) and install CMake binary (cmake-4.x.x-windows-x86_64.zip
 or cmake-4.x.x-linux-x86_64.tar.gz).

[Download](https://github.com/ninja-build/ninja/releases) and install Ninja (ninja-win.zip or ninja-linux.xip).

# How to build from source

1) open your console in the folder where CMakeLists.txt is at.
2) type cmake --preset "presetname"
3) type cmake --build --preset "presetname"

Presetname can be windows-debug, windows-release, linux-debug or linux-release.

The compiled executable/binary/cli and its files will be placed to `build/` inside the folder with the name of the preset you chose.