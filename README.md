# KalaWindow

**KalaWindow** is a C++20 window library for Windows and Linux, it is used for making software and games.

**KalaWindow** creates and owns the Vulkan 1.3 context, manages the message loop, handles crashes safely through the crash handler and handles all inputs through the input framework.

This project relies on several [external dependencies](https://github.com/greeenlaser/external-shared), they are not shipped inside this project, please make sure you have that repository cloned into a folder inside the same parent directory as this project folder before compiling this project from source.

---

## Platform support

| Platform  | Status        | Notes                                                |
|-----------|---------------|------------------------------------------------------|
| Windows   | Supported     | Requires Windows 10 (build 1809 / 17763) or higher   |
| Linux     | Supported     | Should work on any Linux distro released in the last ~10 years ago |
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
| Risc-v       | Not supported | No plans for Risc-v support                |

---

## Render pipeline support

| Render Pipeline | Status        | Notes                                |
|-----------------|---------------|--------------------------------------|
| Vulkan          | Supported     | Internally locked to Vulkan 1.3      |
| OpenGL          | Not supported | No plans for OpenGL support          |
| DirectX         | Not Supported | No plans for DirectX support         |
| MoltenVK        | Not Supported | No plans for MoltenVK support        |

---

## Links

[Donate on PayPal](https://www.paypal.com/donate/?hosted_button_id=QWG8SAYX5TTP6)

[Official Discord server](https://discord.gg/BZ2eJ5Cdr8)

[Official Youtube channel](https://youtube.com/greenlaser)

---

## Docs

[How to build from source](docs/build_from_source.md)

[External libraries](docs/external_libraries.md)

[Minimum requirements](docs/minimum_requirements.md)

[Lost Empire Entertainment and KalaKit ecosystem](docs/ecosystem.md)
