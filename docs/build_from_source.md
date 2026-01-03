# Prerequisites for building from source

This document is only applicable if you are building this repository from source, if you see this document in a shared release package then you can ignore it.

Linux is currently unsupported so these instructions are only for Windows users.

Download Visual Studio or Visual Studio Build Tools:
https://visualstudio.microsoft.com/vs/

once its installed run the visual studio installer exe and enable this checkbox:
- desktop development with c++

and make sure these are toggled on at the right side panel:
- msvc build tools
- windows 10/11 sdk
- c++ cmake tools
- c++ atl
- c++ mfc

Download and install CMake and add to system path:
https://cmake.org/

Download and install Ninja and add to system path:
https://github.com/ninja-build/ninja

# How to build from source

The compiled executable/binary/cli and its files will be placed to `/release` and `/debug` in the root folder relative to the CMakeLists.txt file. Run `build_windows.bat` to build from source.