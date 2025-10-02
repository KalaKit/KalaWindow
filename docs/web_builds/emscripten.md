# Emscripten

This compiler is required to compile C++ into WebAssembly code for running your game or software on the browser.

## Python is required

Emscripten requires Python to run

Get Python here: https://www.python.org/downloads/

- Download the installer and run it

Add these to system environment path:

- 'C:\Users\<yourusername>\AppData\Local\Programs\Python\Python313'
- 'C:\Users\<yourusername>\AppData\Local\Programs\Python\Python313\Scripts'

## Setup

Get the SDK here: https://github.com/emscripten-core/emsdk/tags

- Download it to 'C:\BuildTools\emsdk'
- Extract to same folder, copy contents out of extracted folder into 'C:\BuildTools\emsdk'
- Open CMD, cd to 'C:\BuildTools\emsdk'
- Run command 'emsdk.bat install latest' to get up to date
- Run command 'emsdk.bat activate latest --permanent' to activate emsdk
- Run command 'emcc --version' to test if it installed correctly

Add these to system environment path:

- 'C:\BuildTools\emsdk'
- 'C:\BuildTools\emsdk\node\22.16.0_64bit\bin' (or your node version)
- 'C:\BuildTools\emsdk\upstream\emscripten'
- 'C:\BuildTools\emsdk\upstream\llvm\bin'