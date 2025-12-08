# KalaWindow

![Logo](logo.png)

[![License](https://img.shields.io/badge/license-Zlib-blue)](LICENSE.md)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-brightgreen)
![Development Stage](https://img.shields.io/badge/development-Alpha-yellow)

**KalaWindow** is a C++20 window library for **Windows**, built for native desktop applications for Windows with support coming for Linux in the future.

**KalaWindow** creates and owns the OpenGL 3.3 context but shader binding, swapping GL buffers, making context to current, checking if context is current, other GL related functions and GL core functions (+ the wgl and glx ones) must come from your GL library like [KalaGL](https://github.com/kalakit/kalagl) or another GL source. This way **KalaWindow** stays purely a GL context initializer and a window library with input and a message loop and nothing more. **KalaGL** also provides a full UI framework with a widget system.

[KalaAudio](https://github.com/kalakit/kalaaudio) is recommended to be used for audio playback and audio streaming.

Features built into **KalaWindow**:
- windowing with support for multiple windows
- input (keyboard and mouse)
- native message loop
- built in crash handler (previously known as KalaCrashHandler, now only maintained in this repository codebase)
- OpenGl 3.3 context

---

## Supported platforms

| Platform  | Status         | Notes                                                |
|-----------|----------------|------------------------------------------------------|
| Windows   | Supported      | Requires Windows 10 (build 1809 / 17763) or higher   |
| Linux     | Planned        | Support coming in a future version                   |
| WebGL     | Planned        | Support coming in a future version                   |
| Mac       | Not Supported  | No plans for macOS compatibility                     |
| ARM       | Not Supported  | ARM architecture will not be targeted                |
| Console   | Not Supported  | No plans for console platforms                       |
| Mobile    | Not Supported  | No plans for mobile platforms                        |

---

## Supported render pipelines

| Render Pipeline | Status         | Notes                                             |
|-----------------|----------------|---------------------------------------------------|
| OpenGL          | Supported      | Internally locked to OpenGL 3.3                   |
| Vulkan          | Planned        | Support for Vulkan 1.3 coming in a future version |
| DirectX         | Not Supported  | No plans for DirectX compatibility                |
| MoltenVK        | Not Supported  | No plans for MoltenVK compatibility               |

---

## Links

[Donate on PayPal](https://www.paypal.com/donate/?hosted_button_id=QWG8SAYX5TTP6)

[Official Discord server](https://discord.gg/jkvasmTND5)

[Official Youtube channel](https://youtube.com/greenlaser)

---

## Docs

[How to build from source](docs/build_from_source.md)

[External libraries](docs/external_libraries.md)

[Minimum requirements](docs/minimum_requirements.md)

[Lost Empire Entertainment and KalaKit ecosystem](docs/ecosystem.md)