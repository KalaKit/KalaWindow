# KalaWindow Library Naming Scheme

KalaWindow distributes prebuilt binaries with human-readable names so you can always tell which version and build configuration you are using. KalaWindow is the official shorthand for KalaWindow.

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
| KalaWindowXX.dll       | Release DLL            |
| KalaWindowXXd.dll      | Debug DLL              |
| KalaWindowXX.lib       | Release import library |
| KalaWindowXXd.lib      | Debug import library   |
| _KalaWindowXX.dll      | In development DLL     |

## Linux examples

| Filename pattern | Meaning                      |
|------------------|------------------------------|
| libKalaWindowXX.so     | Release shared object        |
| libKalaWindowXXd.so    | Debug shared object          |
| libKalaWindowXX.a      | Release static library       |
| libKalaWindowXXd.a     | Debug static library         |
| _libKalaWindowXX.so    | In development shared object |