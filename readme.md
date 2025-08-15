# KalaWindow

[![License](https://img.shields.io/badge/license-Zlib-blue)](LICENSE.md)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-brightgreen)
![Development Stage](https://img.shields.io/badge/development-Alpha-yellow)

![Logo](logo.png)

> Due to ongoing rewriting of the project Linux support is currently not available. It will be added back in 0.4 for both X11 and Wayland. Vulkan code exists in the codebase but is not fully functional until 0.4 releases too, you should stick to OpenGL only in 0.3.

**KalaWindow** is a C++20 multimedia framework library for **Windows** and **Linux** (X11/Wayland), built for native desktop applications ranging from lightweight tools to full-featured game engines. It provides **multi-window support**, **input handling** and a unified interface to the **native message loop** on each platform. The API is fully **OS-agnostic** and **vendor-neutral** — no platform-specific or graphics API headers are included in its public interface, keeping user code clean and portable.

KalaWindow includes a built-in rendering backend supporting **OpenGL 3.3** and **Vulkan 1.2**, bundled in a single binary. Each backend features a simple **shader system** (compilation, binding, cleanup), along with utilities for **initialization** and **GPU resource management**. This gives you everything needed to plug rendering directly into your application's logic without boilerplate, whether you're building a game engine or a desktop tool.

---

# Prerequisites (when compiling from source code)

## On Windows

> Read Windows_prerequisites.txt and use Windows_prerequisites.zip

## On Linux

> Read Linux_prerequisites.txt

Follow the [example project](https://github.com/lost-empire-entertainment/circuit-chan) to see how to set up and use this library in a real-world game example for Windows.

---

# External libraries

These libraries are used by KalaWindow for various purposes, their licenses are included in their folder inside '_external_shared/*' and KalaWindow license does not apply to them.

| Library                                                              | Use case                                             |
|----------------------------------------------------------------------|------------------------------------------------------|
| [GLM](https://github.com/icaven/glm)                                 | Math library for vectors, matrices, and 3D graphics. |
| [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) | Loads image files like PNG and JPEG into memory.     |
| [ImGui](https://github.com/ocornut/imgui)                            | Creates debug and editor user interfaces.            |
| [MiniAudio](https://github.com/mackron/miniaudio)                    | Plays and records audio on multiple platforms.       |

> **Note:** The ImGui docking branch was used instead of the main branch to enable dockable windows.

---

# Minimum GPU, CPU, and Driver Requirements (by Feature and Vendor)

The following tables list the oldest known hardware that should, in theory, run this framework at the API level. These configurations are not tested and may not offer usable performance. They represent the technical minimum for compatibility, not a performance recommendation.

All targets assume **64-bit systems with Windows 10/11 or Linux distro from the same era**. Older OS versions may work but are **untested**.

## OpenGL 3.3

| Intel CPU           | AMD CPU            | GPU Type        | GPU Name/Series               | Min Driver Version                |
|---------------------|--------------------|-----------------|--------------------------------|------------------------------------|
| Pentium 4 520       | A8-3850            | Integrated APU  | Radeon HD 6550D (Sumo)         | Catalyst 11.6+ / Mesa 8.0+         |
| Pentium 4 520       | Athlon 64 3000+    | Discrete GPU    | Radeon HD 5000 series *1        | Catalyst 10.6+ / Mesa 10.1+        |
| Pentium 4 520       | Athlon 64 3000+    | Discrete GPU    | GeForce 8 series *1             | GeForce/ION 260.89+                |
| Core i3-3217U       | FX-4100            | Integrated GPU  | HD Graphics 4000 (Ivy Bridge)  | 8.15.10.2622+ / Mesa 10.0+         |
| Core i3-10100       | Ryzen 3 3200G      | Discrete GPU    | Iris Xe MAX Graphics (DG1)     | 31.0.101.3616+ / Mesa 20.0+        |

---

## Vulkan 1.2

| Intel CPU           | AMD CPU            | GPU Type        | GPU Name/Series               | Min Driver Version                |
|---------------------|--------------------|-----------------|--------------------------------|------------------------------------|
| Core i3-2100        | Ryzen 3 2200U      | Integrated APU  | Radeon Vega 8                  | AMDVLK 2020.Q1.1 / Mesa 20.0+      |
| Core i3-2100        | FX-4300            | Discrete GPU    | Radeon HD 7000 series (GCN 1.0+)| Adrenalin 21.6.1+ / RADV 20.0+     |
| Core 2 Duo E4300    | Athlon 64 X2 3800+ | Discrete GPU    | GeForce 600 series             | GeForce 441.99 beta (R440 series)+ |
| Pentium Gold G5400  | Ryzen 3 2200G      | Integrated GPU  | UHD Graphics 610 (Pentium)     | 26.20.100.7755+ / Mesa 20.0+       |
| Core i3-10100       | Ryzen 3 3200G      | Discrete GPU    | Iris Xe MAX Graphics (DG1)     | 27.20.100.8845+ / Mesa 20.0+       |

---

**Notes:**  
*1 – Also available as AGP variants; earliest CPUs: Intel Pentium 4 2.4 GHz / AMD Athlon 64 2800+  
     NVIDIA AGP min driver: 340.52  
     AMD AGP min driver: Catalyst 13.9 (legacy)