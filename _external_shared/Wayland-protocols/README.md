# Wayland Protocol Files

This folder contains the pre-generated source files for optional Wayland protocols. These files are needed to interact with advanced Wayland compositor features such as top-level windows (`xdg-shell`) and explicit GPU synchronization (`linux-drm-syncobj`).

These files are necessary because these protocols are **not included as pre-generated headers/sources** in system packages, and must be generated using `wayland-scanner` from XML protocol definitions.

---

## 🧱 Protocols Included

### 🔹 `xdg-shell`
Used to create and manage top-level windows in Wayland compositors (e.g., Weston, Sway, GNOME, KDE).

- `xdg-shell-client-protocol.h`
  → C header defining the client-side interface for `xdg-shell`.

- `xdg-shell-protocol.c`
  → C source file implementing message dispatching logic for `xdg-shell`.

### 🔹 `linux-drm-syncobj`
Used to enable **explicit GPU synchronization** with `wl_surface` via Linux DRM syncobj file descriptors (required for compatibility with some NVIDIA EGL paths).

- `linux-drm-syncobj-v1-client-protocol.h`
  → C header defining the client-side interface for `wp_linux_drm_syncobj`.

- `linux-drm-syncobj-v1-protocol.c`
  → C source file implementing the request encoding for `wp_linux_drm_syncobj`.

---

## ⚙️ How These Files Were Generated

These files were generated using `wayland-scanner`, the official protocol compiler from the Wayland project. The XML definitions come from the [wayland-protocols](https://gitlab.freedesktop.org/wayland/wayland-protocols) repository.

### `xdg-shell` Generation:

```bash
wayland-scanner client-header \
  /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
  xdg-shell-client-protocol.h

wayland-scanner private-code \
  /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
  xdg-shell-protocol.c
```

### `linux-drm-syncobj` Generation:

```bash
wayland-scanner client-header \
  ./linux-drm-syncobj-unstable-v1.xml \
  linux-drm-syncobj-v1-client-protocol.h

wayland-scanner private-code \
  ./linux-drm-syncobj-unstable-v1.xml \
  linux-drm-syncobj-v1-protocol.c
```
