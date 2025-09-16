# Minimum GPU, CPU, and Driver Requirements (by Feature and Vendor)

The following tables list the oldest known hardware that should, in theory, run this framework at the API level. These configurations are not tested and may not offer usable performance. They represent the technical minimum for compatibility, not a performance recommendation.

All targets assume **64-bit systems with Windows 10/11 or Linux distro from the same era**.

### OpenGL 3.3

| Intel CPU           | AMD CPU            | GPU Type        | GPU Name/Series               | Min Driver Version                |
|---------------------|--------------------|-----------------|--------------------------------|------------------------------------|
| Pentium 4 520       | A8-3850            | Integrated APU  | Radeon HD 6550D (Sumo)         | Catalyst 11.6+ / Mesa 8.0+         |
| Pentium 4 520       | Athlon 64 3000+    | Discrete GPU    | Radeon HD 5000 series *1        | Catalyst 10.6+ / Mesa 10.1+        |
| Pentium 4 520       | Athlon 64 3000+    | Discrete GPU    | GeForce 8 series *1             | GeForce/ION 260.89+                |
| Core i3-3217U       | FX-4100            | Integrated GPU  | HD Graphics 4000 (Ivy Bridge)  | 8.15.10.2622+ / Mesa 10.0+         |
| Core i3-10100       | Ryzen 3 3200G      | Discrete GPU    | Iris Xe MAX Graphics (DG1)     | 31.0.101.3616+ / Mesa 20.0+        |

**Notes:**  
*1 – Also available as AGP variants; earliest CPUs: Intel Pentium 4 2.4 GHz / AMD Athlon 64 2800+  
     NVIDIA AGP min driver: 340.52  
     AMD AGP min driver: Catalyst 13.9 (legacy)