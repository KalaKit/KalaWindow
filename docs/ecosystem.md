# Core structure

the header collection:
- kalaheaders

the clis:
- kalamove
- kalafont
- kalamodel

Note: All clis follow the same original cli template which means all clis follow the same cli command system and can also accept system commands through the run command.

the libraries (rely on clis or headers in some way):
- kalawindow
- kalaui
- kalaaudio
- kalaphysics
- kalaserver
- kalalua

the executables (rely on libs or clis or headers in some way):
- solin
- vulin
- elypso engine
- elypso hub

---

## KalaHeaders

Header-only scripts made in C++ 20 for various purposes. Completely self-dependant, unrelated to each other and can be used independently without dragging any of the other ones in.
 
---

## KalaMove

Uses a manifest system with the .kmf extension and a custom syntax to control how do move, copy, delete and rename files and folders and how to create new dirs where needed.

---

## KalaFont

**KalaFont** compiles .ttf and .otf fonts into .kfd (kalafontdata) - a runtime-ready bitmap or per-glyph binary of vertices, indices, uvs and other relevant data related to the font that was passed. Locks to one base height and strips out all metadata of the original font file.

**KalaFont** uses FreeType to help with font parsing but the runtime-ready binary or its importer header no longer requires FreeType. Use the [importer](https://github.com/KalaKit/KalaHeaders/blob/main/import_kfd.hpp) to import the binary to your target executable.

---

## KalaModel

**KalaModel** compiles .obj, .gltf and .fbx models into .kmd (kalamodeldata) - a runtime-ready model binary of vertices, indices, uvs and other relevant data related to the model that was passed. Strips out all metadata of the original model file.

**KalaModel** uses Assimp to help with model parsing but the runtime-ready binary or its importer header no longer requires Assimp. Use the [importer](https://github.com/KalaKit/KalaHeaders/blob/main/import_kmd.hpp) to import the binary to your target executable.

---

## KalaWindow

**KalaWindow** is a C++20 window library for **Windows**, built for native desktop applications for Windows with support coming for Linux in the future.

**KalaWindow** creates and owns the OpenGL 3.3 context, manages the message loop, handles crashes safely through the crash handler and handles all inputs through the input framework. Glad or any other opengl function loaders are not required because **KalaWindow** already provides most common ones for what you would need for OpenGL 3.3 use. **KalaWindow** also provides a comprehensive shader compiler system with helpful uniform functions and for binding the vert/frag/geometry shader as well.

---

## KalaUI

**KalaUI** is a retained mode production ready UI library for OpenGL 3.3 and it provides a fully self made UI framework, with a widget system where Image and Text are the root widgets that does not own or create a window or the GL context.

---

## KalaAudio

**KalaAudio** provides a simple wrapper with self-owned lifetime for MiniAudio. It supports up to 4 audio listeners, as many players as you want and covers most toggles that MiniAudio provides for audio listeners and players.

---

## KalaPhysics

**KalaPhysics** aims to cover most physics needs for ray casting, collision with most popular shapes in broadphase and narrowphase and will experiment with many various other things like **instant vs delayed rays**.

---

## KalaServer

A single .lib and .dll with no runtime of its own that can be attached to any executable or cli. Does not depend on KalaWindow, runs completely standalone from it. 

Provides the networking stack required for sending and receiving packets for any server-related needs. Can autoban spammers, obvious bots trying to reach sensitive files etc.

Also provides a framework for hosting websites. Can route root path to any path inside its own root so the original path is unreachable by the end user if they try to look what files are accessible.

Uses completely original code with help from WinSock and Cloudflare Tunnel, does not rely on any external server stacks.

---

## KalaLua

KalaLua is a C++ 20 wrapper library around Lua for Windows (Linux coming in the future) that allows you to route Lua calls to one middleman library without needing Lua itself in any way inside your own program.

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

## Elypso Engine

**Elypso engine** is currently not maintained and will be rewritten in 2026. It will use KalaWindow, KalaUI, KalaAudio and KalaPhysics and will heavily rely on them to build a powerful game engine for game development with OpenGL and Vulkan.

---

## Elypso Hub

**Elypso hub** is the project manager and updater for Elypso Engine that will be rewritten in 2026 alongside the engine. It will heavily rely on KalaWindow and KalaUI.