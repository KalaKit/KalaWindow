# KalaWindow

[![License](https://img.shields.io/badge/license-Zlib-blue)](LICENSE.md)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-brightgreen)
![Development Stage](https://img.shields.io/badge/development-Alpha-yellow)

![Logo](logo.png)

> Due to ongoing rewriting of the project Linux support is currently not available. It will be added back before full 0.3 rewrite. 

**KalaWindow** is a C++20 multimedia framework library for **Windows** and **Linux** (X11/Wayland), built for native desktop applications ranging from full-featured game engines to lightweight graphical tools. It provides **multi-window support**, **input handling**, and a unified interface to the **native message loop** on each platform. Crash reporting is integrated via [**KalaCrashHandler**](https://github.com/KalaKit/KalaCrashHandler), and the API is fully **OS-agnostic** and **vendor-neutral** — no platform-specific or graphics API headers are included in its public interface, keeping user code clean and portable.

KalaWindow includes a built-in rendering backend supporting **OpenGL 3.3** and **Vulkan 1.2**, bundled in a single binary. Each backend features a simple **shader system** (compilation, binding, cleanup), along with utilities for **initialization** and **GPU resource management**. This gives you everything needed to plug rendering directly into your application's logic without boilerplate, whether you're building a game engine or a desktop tool.

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
