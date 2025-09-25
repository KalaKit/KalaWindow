# KalaWindow

![Logo](logo.png)

[![License](https://img.shields.io/badge/license-Zlib-blue)](LICENSE.md)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-brightgreen)
![Development Stage](https://img.shields.io/badge/development-Alpha-yellow)

**KalaWindow** is a C++20 multimedia framework library for **Windows**, built for native desktop applications ranging from lightweight tools to full-featured game engines.

Features built into **KalaWindow**:
- windowing with support for multiple windows
- input (keyboard and mouse)
- native message loop
- built in crash handler (previously known as KalaCrashHandler, now only maintained in this repository codebase)
- OpenGl 3.3 context
- shader system with compilation and hot reload (recompile shaders at runtime)
- texture system with hot reload (can modify raw pixel data for each texture)
- basic UI support through ImGui wrapper
- audio framework through MiniAudio wrapper

---

## Additional notes

**KalaWindow** requires Windows 10 (1809 build 17763) or higher and is not supported on older versions.

**KalaWindow** plans to support **Linux** in a later version but no support for Mac, ARM, console, mobile or Web is planned.

Some **KalaWindow** headers pull in official GL extension headers:
- `opengl_functions_core.hpp` includes `glcorearb.h` and `glext.h`
- `opengl_functions_win.hpp` includes `wgleht.hpp`
- `opengl_functions_linux.hpp` includes `glxeht.hpp`

**KalaWindow** plans to support Vulkan 1.3 in a later version but no support for DirectX or MoltenVK or other graphics backends is planned.

---

## Projects made with KalaWindow

Send me an email at `sanderveski@gmail.com` or message me on discord at `greenlaser` if you want your game or program you made with **KalaWindow** to be listed here

- [Circuit Chan](https://github.com/lost-empire-entertainment/circuit-chan)

---

## Links

[Donate on PayPal](https://www.paypal.com/donate/?hosted_button_id=QWG8SAYX5TTP6)

[Official Discord server](https://discord.gg/jkvasmTND5)

[Official Youtube channel](https://youtube.com/greenlaser)

[Official website (currently down for maintenance)](https://thekalakit.com)

[Track development on the Trello board](https://trello.com/b/rXmMDRaf/kalawindow)

---

## Docs

[How to build from source](docs/build_from_source.md)

[External libraries](docs/external_libraries.md)

[Minimum requirements](docs/minimum_requirements.md)

[KalaWindow Library Naming Scheme](docs/naming_scheme.md)