# Minimum requirements

The following tables list the oldest known hardware that should, in theory, run this software at the API level. These configurations are not tested and may not offer usable performance. They represent the technical minimum for compatibility, not a performance recommendation.

All targets assume **64-bit systems with Windows 10/11 or Linux distro from the same era**.

Minimum available disk space: 50MB

## Vulkan 1.3

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
