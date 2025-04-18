# Wayland Protocol Files

This folder contains the pre-generated source files for optional Wayland protocols. These files are needed to interact with advanced Wayland compositor features such as top-level windows (`xdg-shell`).

These files are necessary because these protocols are **not included as pre-generated headers/sources** in system packages, and must be generated using `wayland-scanner` from XML protocol definitions.

---

## 🧱 Protocols Included

### 🔹 `xdg-shell`
Used to create and manage top-level windows in Wayland compositors (e.g., Weston, Sway, GNOME, KDE).

- `xdg-shell-client-protocol.h`
  → C header defining the client-side interface for `xdg-shell`.

- `xdg-shell-protocol.c`
  → C source file implementing message dispatching logic for `xdg-shell`.

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
