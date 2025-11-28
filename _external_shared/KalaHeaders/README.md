# KalaHeaders

Header-only scripts made in C++ 20, great for general use. They don't depend on each other and can be used for any c++ projects.

## core_utils.hpp

Macros and cross platform import/export

- macros for __declspec/__attribute__/__stdcall for windows/linux api
- inline macros for forceinline and noinline
- deprecated macro
- debug assert macro (assertions are compiled out completely in release)
- shorthands for casters
- overrideable cross-platform templates for converting between uintptr_t and pointers/integrals/enums

### Templates

| Function                | Converts **from → to**           | Constraint             | Typical use cases                       |
|-------------------------|----------------------------------|------------------------|------------------------------------------|
| `ToVar<T>(uintptr_t h)` | `uintptr_t` → pointer `T`        | `is_pointer_v<T>`      | Reinterpret raw handle as a pointer (structs, classes, arrays, funcs) |
| `ToVar<T>(uintptr_t h)` | `uintptr_t` → integral `T`       | `is_integral_v<T>`     | Reinterpret raw handle as int/flag/opaque handle |
| `ToVar<T>(uintptr_t h)` | `uintptr_t` → enum `T`           | `is_enum_v<T>`         | Reinterpret raw handle as enum/enum flags/API handle |
| `FromVar<T>(T* h)`      | pointer `T*` → `uintptr_t`       | —                      | Turn a pointer into a numeric handle (safe storage, hashing, serialization) |
| `FromVar<T>(T h)`       | integral `T` → `uintptr_t`       | `is_integral_v<T>`     | Turn an int/flag into a raw handle value |
| `FromVar<T>(T h)`       | enum `T` → `uintptr_t`           | `is_enum_v<T>`         | Turn an enum/enum flag into a raw handle value |

---

## math_utils.hpp

Provides:
- shorthands for math variables
- vec2, vec3, vec4, mat2, mat3, mat4, quat (vec4)
- operators and helpers for vec, mat and quat types
- swizzle operators for vec2-vec4
- mat containers as column-major and scalar form
- color conversion, color operators

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

| Function             | Description                                                         |
|----------------------|---------------------------------------------------------------------|
| CompareStrings       | Compare origin to target with optional case sensitivity toggle      |
| TokenizeString       | Split origin into a vector of chunks between each splitter, keep strings between two tokens as a single string with preserved tokens |
| SplitString          | Split origin into a vector of chunks between each splitter          |
| JoinString           | Join all elements of a vector into a single string with a delimiter |
| TrimString           | Remove leading and trailing whitespace characters from origin       |
| RemoveAllFromString  | Remove all occurrences of target from origin                        |
| ReplaceAllFromString | Replace all occurrences of target from origin with replacement      |
| ToUpperString        | Set all letters of this string to uppercase letters                 |
| ToLowerString        | Set all letters of this string to lowercase letters                 |
| StartsWith           | Check if origin starts with target                                  |
| EndsWith             | Check if origin ends with target                                    |

---

## file_utils.hpp

Provides file management, file metadata, text I/O and binary I/O helper functions

### File management

| Function              | Description |
|-----------------------|-------------|
| CreateFile            | Create regular or binary file at target path. If you also want data written to the new file after its been created then pass a fileData struct with one of the fields filled in, only the first found field data is used |
| CreateDirectory       | Create a directory at target path, this also creates all parent folders up to it that don't exist yet |
| ListDirectoryContents | List all the contents of a folder, with optional recursive flag |
| RenamePath            | Rename file or folder in its current directory |
| DeletePath            | Delete file or folder in target path (recursive for directories) |
| CopyPath              | Copy file or folder from origin to target, with optional overwrite flag |
| MovePath              | Move file or folder from origin to target, target is always overwritten if it already exists |

### File metadata

| Function             | Description |
|----------------------|-------------|
| GetFileSize          | Get the size of the target file in bytes |
| GetDirectorySize     | Get the size of the target directory in bytes |
| GetTextFileLineCount | Get the count of lines in a text file |
| GetPathStem          | Get the stem (filename without extension) of the target |
| SetPathExtension     | Set the extension of the target |

### Text I/O

| Function          | Description |
|-------------------|-------------|
| WriteTextToFile   | Write all text from a string to a text file, with optional append flag. A new file is created at target path if it doesn't already exist |
| ReadTextFromFile  | Read all text from a file into a string |
| WriteLinesToFile  | Write all lines from a vector to a text file, with optional append flag. A new file is created at target path if it doesn't already exist |
| ReadLinesFromFile | Read all lines from a file into a vector of strings with optional lineStart and lineEnd values to avoid placing all lines to memory. If lineEnd is 0 and lineStart isnt, then this function defaults end to EOF |

### Binary I/O

| Function                 | Description |
|--------------------------|-------------|
| ReadBinaryBufferFromFile | Read raw binary data from a file into a buffer (up to bufferSize bytes). OutBytesRead returns the number of bytes read |
| WriteBinaryLinesToFile   | Write all binary data from a vector<uint8_t> to a file, with optional append flag. A new file is created at target path if it doesn't already exist |
| ReadBinaryLinesFromFile  | Read all binary data from a file into a vector<uint8_t> with optional rangeStart and rangeEnd values to avoid placing whole binary file to memory. If rangeEnd is 0 and rangeStart isnt, then this function defaults end to EOF |
| GetRangeByValue          | Return all start and end of defined string in a binary |
| GetRangeByValue          | Return all start and end of defined bytes in a binary |

---

## log_utils.hpp

Comprehensive logger header for any logging needs - sends stdout and stderr messages to your console.

- can get current system time and date, formatted to chosen TimeFormat or DateFormat enum choice
- can set default time and date format so that TIME_DEFAULT and DATE_DEFAULT always use them with full Print function
- has full and basic Print functions

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
| DATE_DMY          | Day/Month/Year                    | 31/12/2025          |
| DATE_MDY          | Month/Day/Year                    | 12/31/2025          |
| DATE_ISO_8601     | ISO 8601                          | 2025-12-31          |
| DATE_TEXT_DMY     | Day Month, Year                   | 31 December, 2025   |
| DATE_TEXT_MDY     | Month Day, Year                   | December 31, 2025   |
| DATE_FILENAME_DMY | Filename-safe (day-month-year)    | 31-12-2025          |
| DATE_FILENAME_MDY | Filename-safe (month-day-year)    | 12-31-2025          |

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

| Function      | Description                                                         |
|---------------|---------------------------------------------------------------------|
| PreReadCheck  | Check file path existence, extension and read permissions for its directory |
| TryOpenCheck  | Check if file isnt locked, if file isnt empty, too small or too big |
| GetHeaderData | Returns the top header data as a struct                             |
| GetTableData  | Returns the glyph tables as a vector of structs for glyph streaming |
| StreamModels  | Returns the glyph blocks for the given glyph tables as a vector of structs |
| ImportKFD     | Returns the top header data, all tables and all blocks as structs   |

---

## import_kmd.hpp

Import kmd (kalamodeldata) binaries into your program for runtime models. Use the [KalaModel cli](https://github.com/kalakit/kalamodel) for exporting fbx, obj or gltf models as kmd.

Each kmd contains this structure:
	- one top header
	- table and block pair per model
	
The tables are used for looking up models, each table contains the model name, its block size and offset.

| Function      | Description                                                         |
|---------------|---------------------------------------------------------------------|
| PreReadCheck  | Check file path existence, extension and read permissions for its directory |
| TryOpenCheck  | Check if file isnt locked, if file isnt empty, too small or too big |
| GetHeaderData | Returns the top header data as a struct                             |
| GetTableData  | Returns the model tables as a vector of structs for model streaming |
| StreamModels  | Returns the model blocks for the given model tables as a vector of structs |
| ImportKMD     | Returns the top header data, all tables and all blocks as structs   |

---

## key_standards.hpp

Provides:
  - standard layout with enums for mouse buttons, keyboard keys and gamepad actions
  - standard layout for typography, math and currency symbols
  - standard layout for latin and cyrillic alphabet letters
  - standard layout for emojis and kaomojis
