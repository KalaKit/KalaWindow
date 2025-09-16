# KalaWindow Library Naming Scheme

KalaWindow distributes prebuilt binaries with human-readable names so you can always tell which version and build configuration you are using. KWnd is the official shorthand for KalaWindow.

## Versioning

- XX = major + minor version
	- 03 = v0.3
	- 10 = v1.0

- d suffix = debug build
	- Includes extra checks, verbose logging, and no optimizations.
  
- _ prefix = prerelease build
	- This makes prerelease binaries impossible to confuse with stable ones.
	- Once a version is considered stable, the underscore is removed.
	- Prerelease builds are often unstable and should never be used in production.

## Windows examples

| Filename pattern | Meaning                |
|------------------|------------------------|
| KWndXX.dll       | Release DLL            |
| KWndXXd.dll      | Debug DLL              |
| KWndXX.lib       | Release import library |
| KWndXXd.lib      | Debug import library   |
| _KWndXX.dll      | In development DLL     |

## Linux examples

| Filename pattern | Meaning                      |
|------------------|------------------------------|
| libKWndXX.so     | Release shared object        |
| libKWndXXd.so    | Debug shared object          |
| libKWndXX.a      | Release static library       |
| libKWndXXd.a     | Debug static library         |
| _libKWndXX.so    | In development shared object |