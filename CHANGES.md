# 1.2.4

- fixed GetFiles not cleaning up held key data after file explorer UI is closed
- added cpu, gpu, ram and os system info getters

# 1.2.3

Changes:
- flipped incorrect SetForceCloseContent target and reason order
- using CreatePopup in linux crash handler like in windows crash handler

# 1.2.2

Changes:
- Made unsafe to call destructors private in `graphics/window.hpp`, `graphics/vulkan.hpp` and `core/input.hpp`

# 1.2.1

Changes:
- Fixed libcanberra and libnotify never being set true on x11

# 1.2.0

Changes:
- Removed dpi selection at window creation, Windows is locked to 'DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2'
- Privated some potentially dangerous input header functions
- Fixed some raw input issues on windows and linux
- Flipped incorrect popup title and message order
- Fixed notifications not appearing on windows (requires startup app shortcut creation, can be disabled with KW_NO_SHORTCUT)
- Fixed linux not showing call stack at crash, fixed linux not creating dump at crash

# 1.1.0

Changes:
- Simplified initialization and update logic

# 1.0.0

Changes:
- None. This is the first release of KalaWindow in its stable state for Windows and Linux.
