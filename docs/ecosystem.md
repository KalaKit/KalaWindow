# Core structure

the header collection:
- kalaheaders

the clis:
- kalamove
- kalafont
- kalamodel

Note: All clis follow the same original cli template which means all clis follow the same cli command system and can also accept system commands through the run command.

the libraries (rely on clis or headers in some way):
- kalaserver
- kalawindow
- kalaui
- kalaaudio

the executables (rely on libs or clis or headers in some way):
- solin
- vulin
- elypso engine
- elypso hub

---

## KalaHeaders

A collection of headers for various purposes. Completely self-dependant, unrelated to each other and can be used independently without dragging any of the other ones in.
 
---

## KalaMove

Uses a manifest system with the .kmf extension and a custom syntax to control how do move, copy, delete and rename files and folders and how to create new dirs where needed.

---

## KalaFont

Compiles .ttf and .otf fonts into .ktf (kalatype font) - a runtime-ready bitmap or per-glyph binary of vertices, indices, uvs and other relevant data related to the font that was passed. Locks to one base height and strips out all metadata of the original font file.

Uses FreeType to help with font parsing.

---

## KalaModel

Compiles .obj, .gltf and .fbx fonts into .kmf (kalamodelfile) - a runtime-ready model binary of vertices, indices, uvs and other relevant data related to the model that was passed. Strips out all metadata of the original model file.

Uses Assimp to help with model parsing.

---

## KalaServer

A single .lib and .dll with no runtime of its own that can be attached to any executable or cli. Does not depend on KalaWindow, runs completely standalone from it. 

Provides the networking stack required for sending and receiving packets for any server-related needs. Can autoban spammers, obvious bots trying to reach sensitive files etc.

Also provides a framework for hosting websites. Can route root path to any path inside its own root so the original path is unreachable by the end user if they try to look what files are accessible.

Uses completely original code with help from WinSock and CloudFlare Tunnel, does not rely on any external server stacks.

---

## KalaWindow

**KalaWindow** is a C++20 window library for **Windows**, built for native desktop applications for Windows with support coming for Linux in the future.

**KalaWindow** creates and owns the OpenGL 3.3 context, manages the message loop, handles crashes safely through the crash handler and handles all inputs through the input framework. Glad or any other opengl function loaders are not required because **KalaWindow** already provides most common ones for what you would need for OpenGL 3.3 use. **KalaWindow** also provides a comprehensive shader compiler system with helpful uniform functions and for binding the vert/frag/geometry shader as well.

---

## KalaUI

**KalaUI** is a retained mode production ready UI library for OpenGL 3.3 and it provides a fully self made UI framework, with a widget system where Image and Text are the root widgets that does not own or create a window or the GL context.

It is recommended to use [KalaWindow](https://github.com/kalakit/kalawindow) to get the GL context but any other window context initializer and GL library like GLFW should work as well.

---

## KalaAudio

**KalaAudio** provides a simple wrapper with self-owned lifetime for MiniAudio. It supports up to 4 audio listeners, as many players as you want and covers most toggles that MiniAudio provides for audio listeners and players.

---

## KalaPhysics

**KalaPhysics** aims to cover most physics needs for ray casting, collision with most popular shapes in broadphase and narrowphase and will experiment with many various other things like **instant vs delayed rays**.

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

A game and software development engine that relies on KalaWindow and KalaGraphics, currently on hiatus until KalaWindow and KalaGraphics are ready for use.

---

## Elypso Hub

The project manager and updater for Elypso Engine that relies on KalaWindow and KalaGraphics, currently on hiatus until KalaWindow and KalaGraphics are ready for use.