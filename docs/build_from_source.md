# Prerequisites for building from source

This document is only applicable if you are building this repository from source, if you see this document in a shared release package then you can ignore it.

# How to build from source

1) [download KalaMake](https://github.com/KalaKit/KalaMake/releases) and get the latest Windows or Linux build.
2) open your console in the folder where project.kmake is at.
3) type `kalamake --compile project.kmake presetname`
- presetname is the preset you chose to compile this project with
- presetname can be release-windows, debug-windows, release-linux or debug-linux.

The built project files will be placed to `build/` inside the folder with the name of the preset you chose.
