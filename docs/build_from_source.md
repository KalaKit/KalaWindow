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

[Download KalaMake](https://github.com/KalaKit/KalaMake/releases) and get the latest Windows or Linux build.

# How to build from source

1) open your console in the folder where project.kmake is at.
2) type kalamake --compile presetname

Presetname can be release-windows, debug-windows, release-linux or debug-linux.

The compiled executable/binary/cli and its files will be placed to `build/` inside the folder with the name of the preset you chose.
