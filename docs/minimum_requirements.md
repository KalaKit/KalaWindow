# Minimum requirements

The following tables list the oldest known specifications that should, in theory, run this library or software at the API level. These configurations are not tested and may not offer usable performance. They represent the technical minimum for compatibility, not a performance recommendation.

## Platform support

| Platform  | Status        | Notes                                                |
|-----------|---------------|------------------------------------------------------|
| Windows   | Supported     | Requires Windows 10 (build 1809 / 17763) or newer   |
| Linux     | Supported     | Requires glibc 2.38 (Ubuntu 24.04/Fedora 39/Debian 13 etc) or newer |
| BSD       | Planned       | BSD support is planned for a future version          |
| Mac       | Not Supported | No plans for Mac support                             |
| WebGL     | Not Supported | No plans for web support                             |
| Console   | Not Supported | No plans for console support                         |
| Mobile    | Not Supported | No plans for mobile support                          |

---

## Architecture support

| Architecture | Status        | Notes                                      |
|--------------|---------------|--------------------------------------------|
| x64          | Supported     | x64 is fully supported                     |
| x86          | Not supported | No plans for x86 support                   |
| Arm          | Planned       | Arm support is planned in a future release |

---

## Render pipeline support

| Render Pipeline | Status        | Notes                                |
|-----------------|---------------|--------------------------------------|
| Vulkan          | Supported     | Internally locked to Vulkan 1.4      |
| OpenGL          | Not supported | No plans for OpenGL support          |
| DirectX         | Not Supported | No plans for DirectX support         |
| MoltenVK        | Not Supported | No plans for MoltenVK support        |

---

## Hardware support

| Type    | AMD CPU                 | Intel CPU               |
|---------|-------------------------|-------------------------|
| Desktop | AMD Athlon 64 X2 3800+¹ | Intel Pentium D 820²    |
| Laptop  | AMD Turion 64 X2 TL-50¹ | Intel Core 2 Duo T5500² |

| Vendor | GPU Type       | GPU Name/Series                        |
|--------|----------------|----------------------------------------|
| NVIDIA | Discrete GPU   | GTX 750 / 750 Ti (Maxwell)             |
| AMD    | Discrete GPU   | RX 460 / 470 / 480 (Polaris / GCN 4)³  |
| AMD    | Integrated GPU | Ryzen 2000G / Radeon Vega 8/11         |
| Intel  | Integrated GPU | HD Graphics 520/530 (Skylake / 6th Gen)⁴   |
| Intel  | Discrete GPU   | Arc A310 / A380 (Alchemist)            |

- 1: Required CPU for AMD integrated GPU
    - Desktop: AMD Ryzen 3 2200G
    - Laptop: AMD Ryzen 3 2200U
- 2: Required CPU for Intel integrated GPU
    - Desktop: Intel Core i3-6100
    - Laptop: Intel Core i3-6006U
- 3: Linux lowest supported AMD discrete GPU: Radeon HD 7700 series (GCN 1.0) via Mesa RADV
- 4: Linux lowest supported integrated GPU: HD Graphics 4600 (Haswell / 4th Gen) via Mesa ANV
