# Win32 Features for KalaWindow

- **Clipboard API**  
  - `Clipboard::SetText(const string&)`  
  - `Clipboard::GetText() -> string`  
  - Wraps `OpenClipboard`, `EmptyClipboard`, `SetClipboardData`, `GetClipboardData`

- **Drag & Drop Support**  
  - File drops: `WM_DROPFILES`, `DragQueryFile`  
  - Future: OLE drag-and-drop (text, images, etc.)

- **Taskbar Integration**  
  - Progress bar in taskbar button (`ITaskbarList3`)  

  - Flash window (`FlashWindowEx`)  

- **System Tray & Notifications**  
  - `Shell_NotifyIcon` wrappers  
  - Balloon tips / native notifications  

- **Power & Session Events**  
  - `WM_POWERBROADCAST` → battery, sleep, resume  
  - Session lock/unlock notifications  

- **Hotkeys**  
  - Global hotkey registration (`RegisterHotKey`)  

- **Hooks (Advanced)**  
  - Low-level input hooks (keyboard, mouse)  
  - Debugging / monitoring tools (careful with perf & security)

---

# Already added / won't be added

- **High DPI / Scaling Control**  
  - `SetProcessDpiAwarenessContext` wrappers  
  - Query per-monitor DPI → `GetDpiForWindow`  

- **Window Visual Effects**  
  - Per-pixel alpha transparency (`UpdateLayeredWindow`)  
  - Acrylic / Mica backgrounds (modern DWM effects)  
  - Rounded corners (`DwmSetWindowAttribute`)  
  
- **Input (Raw / Precision)**  
  - Raw input via `WM_INPUT`  
  - Distinguish left/right Shift, Ctrl, Alt  
  - High precision mouse (no acceleration)  
  
- **Window Chrome / Non-client Area**  
  - Toggle borders & caption (`WS_CAPTION`, `WS_THICKFRAME`)  
  - Custom draggable hit-test regions (`WM_NCHITTEST`)  
  - System drop shadow (`CS_DROPSHADOW`) - won't be added
  
 - **Taskbar Integration**  
    - Overlay icons (status indicators)  