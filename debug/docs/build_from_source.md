# Prerequisites for building from source

## On Windows

Download Visual Studio 2022 Installer from here:
https://visualstudio.microsoft.com/vs/

once its installed make sure these are enabled when installing Visual Studio 2022:
- desktop development with c++ is enabled
- it has c++ cmake tools enabled
- it has windows 10/11 sdk enabled

And finally unpack prerequisites.7z and then run setup.bat to install cmake and ninja

## On Linux

> Not ready, coming soon

# How to build from source

The compiled executable and its files will be placed to `/release` and `/debug` in the root folder relative to the build stage

## On Windows

> Run `build_windows.bat`

## On Linux

> Not ready, coming soon

---

Follow the [example project](https://github.com/lost-empire-entertainment/circuit-chan) to see how to set up and use this library in a real-world game example for Windows.