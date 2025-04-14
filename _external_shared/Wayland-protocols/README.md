# Wayland Protocol Files

This folder contains the pre-generated source files for the `xdg-shell` Wayland protocol. These are needed to create and manage top-level windows in Wayland compositors (e.g., Weston, Sway, KDE, GNOME).

These files are required because `xdg-shell` is an *optional* protocol and not included as pre-generated headers/sources in system packages.

## Files Included

- `xdg-shell-client-protocol.h`
  → C header defining the client-side interface for `xdg-shell`.

- `xdg-shell-protocol.c`
  → C source file implementing message dispatching logic for `xdg-shell`.

## How They Were Generated

These files were generated using `wayland-scanner`, a utility provided by the Wayland project. The protocol definition XML comes from the `wayland-protocols` package.

### Commands Used

```bash
wayland-scanner client-header \
  /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
  xdg-shell-client-protocol.h

wayland-scanner private-code \
  /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
  xdg-shell-protocol.c
```
