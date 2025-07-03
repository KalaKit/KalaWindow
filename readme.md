# KalaWindow

[![License](https://img.shields.io/badge/license-Zlib-blue)](LICENSE.md)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-brightgreen)
![Development Stage](https://img.shields.io/badge/development-Alpha-yellow)

![Logo](logo.png)

> Due to ongoing rewriting of the project Linux support is currently not available. It will be added back before full 0.3 rewrite. 

KalaWindow is a lightweight C++ 20 library for Windows and Linux that is used for rendering the window your program will be ran inside of which creates its own OpenGL 3.3 or Vulkan 1.2 context depending on which binary you choose. The OpenGL binary also uses a custom OpenGL function loader system. KalaWindow also has built in input support and includes [KalaCrashHandler](https://github.com/KalaKit/KalaCrashHandler) for handy crash reports. All the API code is completely OS-agnostic so the exact same code works on both Windows and Linux.

> Wayland support has been deprecated due to nasty restrictions with the update loop. Only X11 will be supported.

---

# Prerequisites (when compiling from source code)

## On Windows

> Read Windows_prerequisites.txt and use Windows_prerequisites.zip

## On Linux

> Read Linux_prerequisites.txt

Follow the [example project](https://github.com/KalaKit/KalaTestProject) to see how to set up and use this library in a real-world example for both Windows and Linux.

---

# Minimum GPU, CPU, and Driver Requirements (by Feature and Vendor)

This table lists the **earliest supported CPU and GPU models** from AMD, NVIDIA, and Intel for:

- OpenGL 3.3
- Vulkan 1.2

All targets assume **64-bit systems with Windows 10+ or Debian 11+**.  
Older OS versions *may* work but are **untested and unsupported**.
Some Vulkan extensions, like those related to *Raytracing* may require newer hardware, consult the [official Vulkan Specifications](https://registry.khronos.org/vulkan/specs/latest/html/vkspec.html) for additional details.

## AMD

| Feature        | First Supported CPU (x64)     | First Supported GPU             | Minimum Driver Version              |
|----------------|-------------------------------|----------------------------------|-------------------------------------|
| OpenGL 3.3     | AMD Athlon II X2 240          | Radeon HD 5770 (Evergreen)       | Catalyst 10.3+ / Mesa 7.9+          |
| Vulkan 1.2     | AMD FX-6300 / Ryzen 1200      | Radeon RX 460 (GCN 4.0)          | AMDVLK 2020.Q1.1 / Mesa 20.0+       |


## NVIDIA

| Feature        | First Supported CPU (x64)     | First Supported GPU             | Minimum Driver Version              |
|----------------|-------------------------------|----------------------------------|-------------------------------------|
| OpenGL 3.3     | Intel Core 2 Duo E8400        | GeForce GTX 460 (Fermi)          | NVIDIA 258.96+ / 260.xx+            |
| Vulkan 1.2     | Intel Core i5-2400            | GeForce GTX 950 (Maxwell 2.0)    | NVIDIA 441.28+                      |

## Intel

| Feature        | First Supported CPU (x64)     | First Supported GPU             | Minimum Driver Version              |
|----------------|-------------------------------|----------------------------------|-------------------------------------|
| OpenGL 3.3     | Intel Core i3-3110M           | HD Graphics 4000 (Gen7)          | Intel 8.15.10.2696+ / Mesa 10.0     |
| Vulkan 1.2     | Intel Core i5-6200U           | HD Graphics 520 (Gen9 Skylake)   | ANV / Mesa 20.0+                    |