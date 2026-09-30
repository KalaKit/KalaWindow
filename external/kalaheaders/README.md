# KalaHeaders

Header-only scripts made in C++ 20 for various purposes. Completely self-dependant, unrelated to each other and can be used independently without dragging any of the other ones in.

Please be aware that this library/software has limited or no documentation at the current stage due to the KalaKit and the Elypso Engine ecosystem being in early development, if you have questions then message me on discord at @kirjukala
 or via [email](mailto:sanderveski@gmail.com?subject=Questions%20about%20KalaKit%20and%20the%20Elypso%20Engine%20ecosystem). The website linked at the right side also does not currently function because both the domain and its [server backend](https://github.com/kalakit/kalaserver) are still in early development.

## core_utils.hpp

Provides:
  - useful low level macros
  - common container concepts
  - helpers for getting enum or string from any enum to string or string to enum in any map or unordered map
  - helpers for checking if raw array, array, vector, map or unordered map contains key or value
  - helpers for removing duplicates from vector, map and unordered_map
  - safe conversions between uintptr_t and pointers, integrals, enums

---

## math_utils.hpp

Provides:
  - shorthands for math variables
  - GLM-like containers as vec2, vec3, vec4, mat2, mat3, mat4, quat
  - operators and helpers for vec, mat and quat types
  - mat containers as column-major and scalar form

## string_utils.hpp

Various string conversions and functions to improve workflow with string operations

---

## file_utils.hpp

Provides: 
  - wildcards - Get files and folders via wildcards non-recursively and recusively
  - file management - create file, create directory, list directory contents, rename, delete, copy, move
  - file metadata - file size, directory size, line count, set extension
  - text I/O - read/write data for text files with vector of string lines or string blob
  - binary I/O - read/write data for binary files with vector of bytes or buffer + size

---

## log_utils.hpp

Comprehensive logger header for any logging needs - sends stdout and stderr messages to your console.

Provides:
  - detailed logger - time, date, log type, origin tag
  - simple logger - just a fwrite to the console with a single string parameter
  - log types - info (no log type stamp), debug (skipped in release), success, warning, error
  - time stamp, date stamp accurate to system clock
  - logHook - user-defined function that allows emitting logs to another target like the crash log storage in kalawindow

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

| Enum Value       | Description                        | Example          |
|------------------|------------------------------------|------------------|
| TIME_NONE        | No time printed                    | *(empty)*        |
| TIME_DEFAULT     | Globally defined default format    | TIME_HMS_MS_US   |
| TIME_HMS         | Hours:Minutes:Seconds              | 23:59:59         |
| TIME_HMS_MS      | Hours:Minutes:Seconds:Milliseconds | 23:59:59:123     |
| TIME_HMS_MS_US   | Hours:Minutes:Seconds:Milliseconds:Microseconds | 23:59:59:123:456 |
| TIME_12H         | 12-hour clock with AM/PM           | 11:59:59 PM      |
| TIME_ISO_8601    | ISO 8601 UTC-style                 | 23:59:59Z        |
| TIME_FILENAME    | Filename-safe (no colons)          | 23-59-59         |
| TIME_FILENAME_MS | Filename-safe with microseconds    | 23-59-59-123-456 |

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

## key_standards.hpp

Provides:
  - standard layout with enums for mouse buttons, keyboard keys and gamepad actions
  - standard layout for typography, math and currency symbols
  - standard layout for latin and cyrillic alphabet letters
  - standard layout for emojis
  
## export_glb.hpp

Provides:
  - export .glb files (full parity with KalaGraphics material system, but as fully standalone header)
  - print json file of .glb to debug the output, can make it look pretty by setting makePretty true
  
## export_png.hpp

Provides:
  - exports .png files with full parity with the KalaGraphics material system while remaining as fully standalone header
  - uses dynamic Huffman deflate compression (if compression is enabled)
  - uses adaptive png scanline filtering
  - converts raw pixel data directly into .png binary data
  - exports already-converted .png binary data directly to disk
  
## password_hasher.hpp

Provides:
  - Argon2id v1.3 (RFC 9106) and BLAKE2b (RFC 7693) compatible password hash implementation
  - HashPassword function to convert any string into a hashed password and salt
  - VerifyPassword function to confirm if a raw password is correct compared to a hashed password and its salt
  - StringToBytes function to safely convert hexadecimal string to binary bytes
  - BytesToString function to safely convert binary bytes to hexadecimal string 
