# KalaHeaders

Header-only scripts made in C++ 20, great for general use. They don't depend on each other and can be used for any c++ projects.

## WORKING HEADERS

### api.hpp

Macros and cross platform import/export

- macros for __declspec/__attribute__/__stdcall for windows/linux api
- inline macros for forceinline and noinline
- deprecated macro
- debug assert macro (assertions are compiled out completely in release)

---

### core_types.hpp

Shorthands for math variables and overrideable cross-platform templates for converting between uintptr_t and pointers/integrals/enums

#### Shorthands

| Alias | Underlying type | Size (bits) | Range / Notes                                    |
|-------|-----------------|-------------|--------------------------------------------------|
| **sst** | `SSIZE_T` (Win32) / `ssize_t` (POSIX) | 64 | Signed size type (≈ –9e18 to +9e18) |
| **st**  | `size_t`       | 64          | Unsigned size type (0 to ~1.8e19)                |
| **u8**  | `uint8_t`      | 8           | 0 to 255                                         |
| **u16** | `uint16_t`     | 16          | 0 to 65,535                                      |
| **u32** | `uint32_t`     | 32          | 0 to 4,294,967,295                               |
| **u64** | `uint64_t`     | 64          | 0 to ~1.8e19 (also useful for handles/pointers)  |
| **i8**  | `int8_t`       | 8           | –128 to 127                                      |
| **i16** | `int16_t`      | 16          | –32,768 to 32,767                                |
| **i32** | `int32_t`      | 32          | –2,147,483,648 to 2,147,483,647                  |
| **i64** | `int64_t`      | 64          | –9e18 to +9e18                                   |
| **f32** | `float`        | 32          | ~6 decimal digits precision                      |
| **f64** | `double`       | 64          | ~15 decimal digits precision                     |

#### Templates

| Function                | Converts **from → to**           | Constraint             | Typical use cases                       |
|-------------------------|----------------------------------|------------------------|------------------------------------------|
| `ToVar<T>(uintptr_t h)` | `uintptr_t` → pointer `T`        | `is_pointer_v<T>`      | Reinterpret raw handle as a pointer (structs, classes, arrays, funcs) |
| `ToVar<T>(uintptr_t h)` | `uintptr_t` → integral `T`       | `is_integral_v<T>`     | Reinterpret raw handle as int/flag/opaque handle |
| `ToVar<T>(uintptr_t h)` | `uintptr_t` → enum `T`           | `is_enum_v<T>`         | Reinterpret raw handle as enum/enum flags/API handle |
| `FromVar<T>(T* h)`      | pointer `T*` → `uintptr_t`       | —                      | Turn a pointer into a numeric handle (safe storage, hashing, serialization) |
| `FromVar<T>(T h)`       | integral `T` → `uintptr_t`       | `is_integral_v<T>`     | Turn an int/flag into a raw handle value |
| `FromVar<T>(T h)`       | enum `T` → `uintptr_t`           | `is_enum_v<T>`         | Turn an enum/enum flag into a raw handle value |

---

### logging.hpp

Comprehensive logger header for any logging needs - sends cout, cerr and clog messages to your console.

- can get current system time and date, formatted to chosen TimeFormat or DateFormat enum choice
- can set default time and date format so that TIME_DEFAULT and DATE_DEFAULT always use them with full Print function
- has full and basic Print functions

### Full and basic Print function differences

| Feature           | `Print(message, target, type, ...)` | `Print(message)`        |
|-------------------|-----------------------------------------------|-------------------------|
| Parameters        | Message + target + log type (+opts)           | Message only            |
| Log types         | Supports INFO, DEBUG, SUCCESS, WARNING, ERROR | Not supported (always cout) |
| Target handling   | Yes (with truncation checks)                  | No                      |
| Time/Date stamp   | Yes (configurable)                            | No                      |
| Output stream     | Varies by type (cout/clog/cerr)               | Always cout             |
| Truncation checks | Message + target length                       | Message length only     |
| Typical usage     | Detailed, tagged log for debugging/monitoring | Quick, simple console output |

#### Available time format types

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

#### Available date format types

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

### string_helpers.hpp

Provides various string helpers to improve workflow with string operations

| Function         | Description                                           |
|------------------|-------------------------------------------------------|
| SplitString      | Split origin into a vector of chunks between each splitter |
| TrimString       | Remove leading and trailing whitespace characters from origin |
| RemoveFromString | Remove all occurrences of target from origin          |
| ToUpperString    | Set all letters of this string to uppercase letters   |
| ToLowerString    | Set all letters of this string to lowercase letters   |
| StartsWith       | Check if origin starts with target                    |
| EndsWith         | Check if origin ends with target                      |

## EXPERIMENTAL HEADERS

These were made for fun and are not tested in detail and may not work as intended.

### kalautils.hpp

Provides shorthands and helpers for various types, each split into its own namespace

| Category               | Namespace     | Description                                           |
|------------------------|---------------|-------------------------------------------------------|
| STRING                 | KString       | Shorthands and helpers for string operations          |
| TYPE TRAITS            | KType         | Helpers for checking if var is type                   |
| FIXED TYPE CONTAINERS  | KContainer    | Shorthands for common fixed-type containers           |
| MEMORY                 | KMemory       | Shorthands and helpers for smart pointer operations   |
| TUPLE                  | KTuple        | Helpers for tuple operations                          |
| SPAN                   | KSpan         | Shorthands and helpers for span operations            |
| DYNAMIC TYPE VARIABLES | KDynamic      | Shorthands and helpers for optional and variant ops   |
| FILESYSTEM             | KFile         | Shorthands and helpers for filesystem operations      |
| FILESTREAM             | KFileStream   | Shorthands and helpers for fstream and sstream ops    |
| STRINGSTREAM           | KStringStream | Shorthands and helpers for fstream and sstream ops    |
| LOG                    | KLog          | Shorthands and helpers for logging operations         |
| TIME                   | KTime         | Shorthands and helpers for time operations            |
| SORT                   | KSort         | Helpers for sort operations                           |

---

### c_helpers.hpp

A collection of blazing fast char handling helpers and some general nice to have C helpers for C++ developers

Provides:
- Checks
  - kh_empty, kh_len, kh_starts, kh_ends
- Comparisons
  - kh_equals, kh_iequals, kh_nequals, kh_niequals
- Allocation and ownership
  - kh_set, kh_dup, kh_free
- Copy & append
  - kh_copy, kh_cat
- Search & cleanup
  - kh_fchar, kh_lchar, kh_trim, kh_remove, kh_aremove
- Modification
  - kh_tolower, kh_toupper, kh_replace, kh_areplace