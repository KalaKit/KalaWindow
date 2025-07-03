# Minimum GPU, CPU, and Driver Requirements (by Feature and Vendor)

This table lists the **earliest supported CPU and GPU models** from AMD, NVIDIA, and Intel for:

- OpenGL 3.3
- Vulkan 1.2

All targets assume **64-bit systems with Windows 10+ or Debian 11+**.  
Older OS versions *may* work but are **untested and unsupported**.  
**OpenGL 3.3** supports legacy dual-core CPUs and decade-old GPUs.  
**Vulkan 1.2** is supported by most GPUs released since ~2015.

---

## AMD — Earliest Supported CPUs & GPUs by Feature

| Feature        | First Supported CPU (x64)     | First Supported GPU             | Minimum Driver Version              |
|----------------|-------------------------------|----------------------------------|-------------------------------------|
| OpenGL 3.3     | AMD Athlon II X2 240          | Radeon HD 5770 (Evergreen)       | Catalyst 10.3+ / Mesa 7.9+          |
| Vulkan 1.2     | AMD FX-6300 / Ryzen 1200      | Radeon RX 460 (GCN 4.0)          | AMDVLK 2020.Q1.1 / Mesa 20.0+       |

---

## NVIDIA — Earliest Supported CPUs & GPUs by Feature

| Feature        | First Supported CPU (x64)     | First Supported GPU             | Minimum Driver Version              |
|----------------|-------------------------------|----------------------------------|-------------------------------------|
| OpenGL 3.3     | Intel Core 2 Duo E8400        | GeForce GTX 460 (Fermi)          | NVIDIA 258.96+ / 260.xx+            |
| Vulkan 1.2     | Intel Core i5-2400            | GeForce GTX 950 (Maxwell 2.0)    | NVIDIA 441.28+                      |

---

## Intel — Earliest Supported CPUs & GPUs by Feature

| Feature        | First Supported CPU (x64)     | First Supported GPU             | Minimum Driver Version              |
|----------------|-------------------------------|----------------------------------|-------------------------------------|
| OpenGL 3.3     | Intel Core i3-3110M           | HD Graphics 4000 (Gen7)          | Intel 8.15.10.2696+ / Mesa 10.0     |
| Vulkan 1.2     | Intel Core i5-6200U           | HD Graphics 520 (Gen9 Skylake)   | ANV / Mesa 20.0+                    |

---

## Vulkan Feature Requirements — Any Vulkan-Capable GPU

| Feature                        | Min CPU | Min GPU        | Min Driver / Notes                                    |
|--------------------------------|---------|----------------|-------------------------------------------------------|
| VE_Surface                     | Any x64 | Any Vulkan GPU | Platform ICD                                          |
| VE_Win32Surface                | Any x64 | Any Vulkan GPU | Windows SDK / Driver                                  |
| VE_XcbSurface                  | Any x64 | Any Vulkan GPU | X11 / Mesa ICD                                        |
| VE_XlibSurface                 | Any x64 | Any Vulkan GPU | X11 / Mesa ICD                                        |
| VE_ExtHeadlessSurface          | Any x64 | Any Vulkan GPU | NV 455.38+ (W) / 440.82+ (L) / AMD 20.12.1+ / Mesa 21.0+ |
| VE_KhrSwapchain                | Any x64 | Any Vulkan GPU | NV 367.27+ / AMD 16.3+  / Intel 15.40.4473+ (W) / Mesa 12.0+ (L) |
| VE_KhrDisplay                  | Any x64 | Any Vulkan GPU | NV 418.96+ / AMD 19.3+ / Mesa 19.0+                    |
| VE_KhrDisplaySwapchain         | Any x64 | Any Vulkan GPU | NV 418.96+ / AMD 19.3+ / Mesa 19.0+                    |
| VE_ExtDebugReport              | Any x64 | Any Vulkan GPU | NV 367.27+ / AMD 16.3+ / Intel 15.40.4473+ (W)  / Mesa 12.0+ (L)|
| VE_DebugUtils                  | Any x64 | Any Vulkan GPU | SDK 1.1.106+                                           |
| VE_ExtValidationFeatures       | Any x64 | Any Vulkan GPU | SDK 1.1.106+                                           |
| VE_ExtDebugMarker              | Any x64 | Any Vulkan GPU | SDK 1.1.80+                                            |
| VE_ExtPipelineCreationFeedback | Any x64 | Any Vulkan GPU | SDK 1.2.162+                                           |
| VE_ExtToolingInfo              | Any x64 | Any Vulkan GPU | SDK 1.2.162+                                           |

---

## Vulkan Shader Feature Requirements — Modern Shader Models

| Feature                          | Min CPU | Min GPU                | Min Driver / Notes                               |
|----------------------------------|---------|------------------------|--------------------------------------------------|
| VE_KhrShaderAtomicInt64          | Any x64 | Pascal+ / Vega+ / Gen11+ | NV 443.41+ / AMD 20.12.1+ / Intel 26.20.100.7755+ |
| VE_KhrShaderSubgroupExtendedTypes| Any x64 | Pascal+ / Vega+ / Gen11+ | NV 443.41+ / AMD 20.12.1+ / Intel 26.20.100.7755+ |
| VE_KhrShaderTerminateInvocation  | Any x64 | Turing+ / RDNA+ / Xe+     | NV 443.41+ / AMD 20.12.1+ / Intel 26.20.100.7755+ |
| VE_KhrShaderClock                | Any x64 | Turing+ / RDNA2+ / Xe+    | NV 443.41+ / AMD 20.12.1+ / Intel 26.20.100.7755+ |

---

## Vulkan Ray Tracing Feature Requirements — RT-Capable GPUs

| Feature                         | Min CPU  | Min GPU                        | Min Driver / Notes                                           |
|---------------------------------|----------|--------------------------------|--------------------------------------------------------------|
| VE_KhrAccelerationStructure     | AVX2 CPU | RTX 2060+ / RX 6800+ / Arc A380 | NV 455.38+ / AMD 20.11.2+ / Mesa 21.3.9+   |
| VE_KhrRayTracingPipeline        | AVX2 CPU | RTX 2060+ / RX 6800+ / Arc A380 | NV 455.38+ / AMD 20.11.2+ / Mesa 21.3.9+   |
| VE_KhrRayQuery                  | AVX2 CPU | RTX 2060+ / RX 6800+ / Arc A380 | NV 455.38+ / AMD 20.11.2+ / Mesa 21.3.9+   |
| VE_KhrDeferredHostOperations    | AVX2 CPU | RTX 2060+ / RX 6800+ / Arc A380 | NV 455.38+ / AMD 20.11.2+ / Mesa 21.3.9+   |
| VE_KhrBufferDeviceAddress       | AVX2 CPU | RTX 2060+ / RX 6800+ / Arc A380 | NV 455.38+ / AMD 20.11.2+ / Mesa 21.3.9+   |