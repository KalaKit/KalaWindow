# 1.4.1 (in development)

- crash log layout cleanup
- cleaned up hex-dec inconsistencies in crash handler logic
- cleaned up warnings
- added missing fixed underlying types to enums
- added missing unknown exception code/signal for windows and linux
- ...

# 1.4.0

- no longer need to manually initialize global vulkan
- moved crash init to internal window_global init
- can set app name and extentions with separate functions
- exposed getters for detecting if on Wine or virtual machine
- added vulkan version setter
- displaying virtual machine status in os info
- added windows build version and revision getters

# 1.3.0

- added cpu, gpu, ram and os system info to crash handler log
- fixed windows popup message order
- no longer need to manually initialize crash handler

# 1.2.4

- fixed GetFiles not cleaning up held key data after file explorer UI is closed
- added cpu, gpu, ram and os system info getters

# 1.2.3

- flipped incorrect SetForceCloseContent target and reason order
- using CreatePopup in linux crash handler like in windows crash handler

# 1.2.2

- Made unsafe to call destructors private in `graphics/window.hpp`, `graphics/vulkan.hpp` and `core/input.hpp`

# 1.2.1

- Fixed libcanberra and libnotify never being set true on x11

# 1.2.0

- Removed dpi selection at window creation, Windows is locked to 'DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2'
- Privated some potentially dangerous input header functions
- Fixed some raw input issues on windows and linux
- Flipped incorrect popup title and message order
- Fixed notifications not appearing on windows (requires startup app shortcut creation, can be disabled with KW_NO_SHORTCUT)
- Fixed linux not showing call stack at crash, fixed linux not creating dump at crash

# 1.1.0

- Simplified initialization and update logic

# 1.0.0

- None. This is the first release of KalaWindow in its stable state for Windows and Linux.
