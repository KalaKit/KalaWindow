# Build from source

This document is only applicable if you are building this repository from source, if you see this document in a release package then you can ignore it.

## Prerequisites

- [Download KalaMake (1.4.0 or newer)](https://github.com/KalaKit/KalaMake/releases).
- [Download mf](https://github.com/greeenlaser/personal-stash/tree/main/mf).
- [Clang (this or newer version, older versions should work fine too)](https://github.com/llvm/llvm-project/releases/tag/llvmorg-23.1.2)
- [Powershell 7 or newer (if building on Windows, required to use shell scripts)](https://learn.microsoft.com/en-us/powershell/scripting/install/install-powershell?view=powershell-7.6)
- [Build tools for Visual Studio 2026 (if building on Windows, 2022 should work too)](https://visualstudio.microsoft.com/downloads/)

## How to build from source

1) open your console in the folder where `project.kmake` is at.
2) type `build.sh --yourpresetname --export`

yourpresetname name can be windows, windows-gnu or linux.

The compiled executable/binary and its files will be placed to `build/` inside the version folder with the name of the preset you chose.
