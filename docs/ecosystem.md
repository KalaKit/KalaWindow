# Core structure

the header collection:
- kalaheaders

the clis:
- kalamove
- kaladata
- kalafont
- kalaglyph
- kalamodel

Note: All clis follow the same original cli template which means all clis follow the same cli command system and can also accept system commands through the run command.

the libraries (rely on clis or headers in some way):
- kalaserver
- kalawindow

the executables (rely on libs or clis or headers in some way):
- solin
- vulin
- circuit chan
- elypso engine
- elypso hub

---

## KalaHeaders

A collection of headers for various purposes. Completely self-dependant, unrelated to each other and can be used independently without dragging any of the other ones in.

### core_utils.hpp

Provides:
  - Cross-platform export/import macro (LIB_API)
  - Win32 machine level function calling convenction (LIB_APIENTRY)
  - Function inlining control (FORCE_INLINE, NO_INLINE)
  - Deprecation marker (DEPRECATED)
  - Debug-only assertion (DEBUG_ASSERT)
  - Safe conversions between uintptr_t and pointers, integrals, enums
  
### file_utils.hpp

Provides:
  - file management - create file, create directory, list directory contents, rename, delete, copy, move
  - file metadata - file size, directory size, line count, set extension
  - text I/O - read/write data for text files with vector of string lines or string blob
  - binary I/O - read/write data for binary files with vector of bytes or buffer + size
  
### hierarchy_utils.hpp

Provides:
  - parent-child hierarchy management
  - fast lookup through recursive traversal across parents, children and siblings
  
### import_ktf.hpp

Provides:
  - Helpers for streaming individual font glyphs or loading the full kalafont type binary into memory
  
### log_utils.hpp

Provides:
  - Detailed logger - time, date, log type, origin tag
  - Simple logger - just a fwrite to the console with a single string parameter
  - Log types - info (no log type stamp), debug (skipped in release), success, warning, error
  - Time stamp, date stamp accurate to system clock
  
### math_utils.hpp

Provides:
  - shorthands for math variables
  - GLM-like containers as vec2, vec3, vec4, mat2, mat3, mat4, quat (vec4)
  - operators and helpers for vec, mat and quat types
  - mat containers as column-major and scalar form
  
### string_utils.hpp

Provides:
  - Various string conversions and functions to improve workflow with string operations
 
---

## KalaMove

Uses a manifest system with the .kmf extension and a custom syntax to control how do move, copy, delete and rename files and folders and how to create new dirs where needed.

---

## KalaData

Stores various formats in bulk inside a custom binary, either uncompressed or with 7z compression algorithms chosen by the end user with simple compression choice of fast, normal or archive.

---

## KalaFont

Compiles .ttf and .otf fonts into .ktf (kalatype font) - a runtime-ready bitmap or per-glyph binary of vertices, indices, uvs and other relevant data related to the font that was passed. Locks to one base height and strips out all metadata of the original font file.

Uses FreeType to help with font parsing.

---

## KalaGlyph

Compiles .png, .jpg and .jpeg images into .kgm (kalaglyph map) - a runtime-ready glyph map binary of vertices, indices, uvs and other relevant data related to the original images that were passed. Locks to one base height and strips out all metadata of the original image files.

Uses stb_image to help with image parsing.

Note: This is a concept, it has not yet started development.

---

## KalaModel

Compiles .fbx, .obj and .gltf models into .kmm (kalamodel map) - a runtime-ready model map binary of vertices, indices and other relevant data related to the original models that were passed.

Uses Assimp to help with font parsing.

Note: This is a concept, it has not yet started development.

---

## KalaServer

A single .lib and .dll that can be attached to any executable or cli. Provides the networking stack required for sending and receiving packets for any server-related needs. Can autoban spammers, obvious bots trying to reach sensitive files etc.

Also provides a framework for hosting websites. Can route root path to any path inside its own root so the original path is unreachable by the end user if they try to look what files are accessible.

Uses completely original code with help from WinSock and CloudFlare Tunnel, does not rely on any external server stacks.

---

## KalaWindow

A single .lib and .dll that can be attached to any executable or cli. Provides the multimedia framework library stack required for running executables with graphics on Windows and Linux. Locked to OpenGL 3.3 and Vulkan 1.3. Linux and Vulkan support have not yet been added but will be added in a future release.


Provides:
  - windowing with message loop
  - crash handler with error popup and crash log on disk + optional dump log
  - per-window input handling
  - window-independent audio playback and streaming (miniaudio)
  - class-agnostic registry system for classes that have more than once instance. uses unique ptr in unordered map for ownership and fast id-based lookup and vector for non-owning pointers for fast loop at runtime
  - reusable transform system in 2D and 3D, uses self-made math header library with no glm help across the entire KalaWindow framework
  
  - opengl shader and texture framework for initializing both, hot reloading both (new image raw pixel data for textures, full hot reload for shaders)
  - per-window camera framework
  - custom per-window UI framework built from the ground up. uses the widget system where the text and image widgets are root, and everything else is built upon them. uses ktf files for fonts and kgm files for images
  
Does not rely on glfw, glm, glew, sdl, sfml, raylib, QT, ImGui or other common helpers similar to these.

---

## Solin

Self-made IDE, uses several headers from KalaHeaders and runs off of KalaWindow.

Accepts any runtime or build commands (as system calls internally), passes results and errors related to them to the compiler window if building, or debug window if running in debug. Does not contain its own compiler, instead expects end user to pass clang, cl or gcc for debugging and compilation through the runtime and build commands in project settings.

---

## Vulin

Self-made database system, uses several headers from KalaHeaders and runs off of KalaWindow and accepts VCALLs via KalaServer as packets from external IPs.

The database itself uses tables where each table has an ID and fields. Tables can hold tables but a child parent cannot hold its parent as its child or sibling. Fields can be any vcall payload type.

### VCALL

A vcall is a vulin packet that is sent out by Vulin to an IP or accepted by Vulin from an external IP. Vulin can send and receive vcall payloads as the following types: u8-u32, i8-i32, f32, string (max 128 u32 characters).

---

## Circuit Chan

A first person in-development 3D game that uses KalaWindow for its foundation.

---

## Elypso Engine

A game and software development engine that relies on KalaWindow.

---

## Elypso Hub

The project manager and updater for Elypso Engine.