# KalaHeaders

Header-only scripts made in C++ 20 for various purposes. Completely self-dependant, unrelated to each other and can be used independently without dragging any of the other ones in.

## core_utils.hpp

Provides:
  - Cross-platform export/import macro (LIB_API)
  - Win32 machine level function calling convenction (LIB_APIENTRY)
  - Function inlining control (FORCE_INLINE, NO_INLINE)
  - Deprecation marker (DEPRECATED)
  - Debug-only assertion (DEBUG_ASSERT)
  - Shorthands for casters
  - Common container concepts
  - Helpers for checking if raw array, array, vector, map or unordered map contains key or value
  - Helpers for removing duplicates from vector, map and unordered_map
  - Safe conversions between uintptr_t and pointers, integrals, enums

---

## math_utils.hpp

Provides:
- shorthands for math variables
- GLM-like containers as vec2, vec3, vec4, mat2, mat3, mat4, quat
- operators and helpers for vec, mat and quat types
- mat containers as column-major and scalar form

## color_utils.hpp

Provides:
- custom color container as a variable for linear RGBA operations
- color conversion, color operators

Provides:
- shorthands for math variables
- GLM-like containers as vec2, vec3, vec4, mat2, mat3, mat4, quat
- operators and helpers for vec, mat and quat types
- mat containers as column-major and scalar form

---

## thread_utils.hpp

Provides:
- lock, lockwait and unlock overrides for handling safe access to variables
- lock_m, lockwait_m (where applicable) and unlock_m for mutexes
- jthread (joinable thread) which returns the created thread so it can be joined
- dthread (self-exiting thread)

---

## string_utils.hpp

Various string conversions and functions to improve workflow with string operations

---

## file_utils.hpp

Provides file management, file metadata, text I/O and binary I/O helper functions

---

## log_utils.hpp

Comprehensive logger header for any logging needs - sends stdout and stderr messages to your console.

- Simple logger - just a fwrite to the console with a single string parameter
- Log types - info (no log type stamp), debug (skipped in release), success, warning, error
- Time stamp, date stamp accurate to system clock
- LogHook - user-defined function that allows emitting logs to another target like the crash log storage in kalawindow

## Full and basic Print function differences

| Feature           | `Print(message, target, type, ...)` | `Print(message)`        |
|-------------------|-----------------------------------------------|-------------------------|
| Parameters        | Message + target + log type (+opts)           | Message only            |
| Log types         | Supports INFO, DEBUG, SUCCESS, WARNING, ERROR | Not supported (always stdout) |
| Target handling   | Yes (with truncation checks)                  | Yes (with truncation checks) |
| Time/Date stamp   | Yes (configurable)                            | No                      |
| Output stream     | Varies by type (stdout/stderr)                | Always stdout           |
| Truncation checks | Message + target length                       | Message length only     |
| Typical usage     | Detailed, tagged log for debugging/monitoring | Simple console output   |

### Available time format types

| Enum Value       | Description                        | Example       |
|------------------|------------------------------------|---------------|
| TIME_NONE        | No time printed                    | *(empty)*     |
| TIME_DEFAULT     | Globally defined default format    | depends       |
| TIME_HMS         | Hours:Minutes:Seconds              | 23:59:59      |
| TIME_HMS_MS      | Hours:Minutes:Seconds:Milliseconds | 23:59:59:123  |
| TIME_12H         | 12-hour clock with AM/PM           | 11:59:59 PM   |
| TIME_ISO_8601    | ISO 8601 UTC-style                 | 23:59:59Z     |
| TIME_FILENAME    | Filename-safe (no colons)          | 23-59-59      |
| TIME_FILENAME_MS | Filename-safe with milliseconds    | 23-59-59-123  |

### Available date format types

| Enum Value        | Description                       | Example             |
|-------------------|-----------------------------------|---------------------|
| DATE_NONE         | No date printed                   | *(empty)*           |
| DATE_DEFAULT      | Globally defined default format   | depends             |
| DATE_DMY          | Day/Month/Year                    | 31/12/2026          |
| DATE_MDY          | Month/Day/Year                    | 12/31/2026          |
| DATE_ISO_8601     | ISO 8601                          | 2026-12-31          |
| DATE_TEXT_DMY     | Day Month, Year                   | 31 December, 2026   |
| DATE_TEXT_MDY     | Month Day, Year                   | December 31, 2026   |
| DATE_FILENAME_DMY | Filename-safe (day-month-year)    | 31-12-2026          |
| DATE_FILENAME_MDY | Filename-safe (month-day-year)    | 12-31-2026          |

---

## import_kfd.hpp

Import kfd (kalafontdata) binaries into your program for runtime fonts. Use the [KalaFont cli](https://github.com/kalakit/kalafont) for exporting ttf or otf fonts as kfd.

Each kfd contains this structure:
	for per-glyph kfd:
	- one top header
	- table and block pair per glyph
	
	for bitmap kfd:
	- one top header
	- tables for each glyph, one block for the bitmap texture
	
The tables are used for looking up glyphs, each table contains the glyph char code, its block size and offset.

---

## import_kmd.hpp

Import kmd (kalamodeldata) binaries into your program for runtime models. Use the [KalaModel cli](https://github.com/kalakit/kalamodel) for exporting fbx, obj or gltf models as kmd.

Each kmd contains this structure:
	- one top header
	- table and block pair per model
	
The tables are used for looking up models, each table contains the model name, its block size and offset.

---

## key_standards.hpp

Provides:
  - standard layout with enums for mouse buttons, keyboard keys and gamepad actions
  - standard layout for typography, math and currency symbols
  - standard layout for latin and cyrillic alphabet letters
  - standard layout for emojis
