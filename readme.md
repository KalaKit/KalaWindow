# KalaWindow

[![License](https://img.shields.io/badge/license-Zlib-blue)](LICENSE.md)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-brightgreen)
![Development Stage](https://img.shields.io/badge/development-Alpha-yellow)

![Logo](logo.png)

> Due to ongoing rewriting of the project Linux support is currently not available. It will be added back before full 0.3 rewrite. 

KalaWindow is a lightweight C++ 20 library for Windows and Linux (x11/wayland) that is used for rendering the window your program will be ran inside of which creates its own OpenGL 3.3 or Vulkan 1.2 context depending on which binary you choose. The OpenGL binary also uses a custom OpenGL function loader system. KalaWindow also has built in input support and includes [KalaCrashHandler](https://github.com/KalaKit/KalaCrashHandler) for handy crash reports. All the API code is completely OS-agnostic so the exact same code works on both Windows and Linux.

> KalaWindow will not support anything older than OpenGL 3.3 or Vulkan 1.2, and anything newer than OpenGL 3.3, but Vulkan support might update in the future if there is enough demand and features.

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

All targets assume **64-bit systems with Windows 10/11 or Linux distro from the same era**.  
Older OS versions *may* work but are **untested and unsupported**.
Some Vulkan extensions, like those related to *Raytracing* may require newer hardware, consult the [official Vulkan Specifications](https://registry.khronos.org/vulkan/specs/latest/html/vkspec.html) for additional details.

## AMD

| Feature | Type           | First Supported GPU              | Minimum Driver Version         |
|---------|----------------|----------------------------------|--------------------------------|
| OpenGL  | Integrated APU | Radeon HD 6550D (Sumo)           | Catalyst 11.6+    / Mesa 8.0+  |
| OpenGL  | Discrete GPU   | Radeon HD 5000 series            | Catalyst 10.6+    / Mesa 10.1+ |
| Vulkan  | Integrated APU | Radeon Vega 8                    | AMDVLK 2020.Q1.1  / Mesa 20.0+ |
| Vulkan  | Discrete GPU   | Radeon HD 7000 series (GCN 1.0+) | Adrenalin 21.6.1+ / RADV 20.0+ |

## NVIDIA

| Feature | Type         | First Supported GPU | Minimum Driver Version             |
|---------|--------------|---------------------|------------------------------------|
| OpenGL  | Discrete GPU | GeForce 8 series    | GeForce/ION 260.89+                |
| Vulkan  | Discrete GPU | GeForce 600 series  | GeForce 441.99 beta (R440 series)+ |

## Intel

| Feature | Type           | First Supported GPU           | Minimum Driver Version       |
|---------|----------------|-------------------------------|------------------------------|
| OpenGL  | Integrated GPU | HD Graphics 4000 (Ivy Bridge) | 8.15.10.2622+   / Mesa 10.0+ |
| OpenGL  | Discrete GPU   | Iris Xe MAX Graphics (DG1)    | 31.0.101.3616+  / Mesa 20.0+ |
| Vulkan  | Integrated GPU | UHD Graphics 610 (Pentium)    | 26.20.100.7755+ / Mesa 20.0+ |
| Vulkan  | Discrete GPU   | Iris Xe MAX Graphics (DG1)    | 27.20.100.8845+ / Mesa 20.0+ |
