# converter

A source reconstruction of `converter.exe`, the asset converter from the Call of Duty 4 mod tools, recovered from the shipped 2007 binary.

## Requirements

- Visual Studio 2019 or later with the **x86 (32-bit) C++ toolset** installed
  (Desktop development with C++ → "MSVC ... C++ x64/x86 build tools")
- CMake 3.21 or later

## Build

```
cmake -B build -A Win32
cmake --build build --config Release
```

The executable is written to `bin\converter.exe`.

## Usage

```
converter
converter -single <asset type> <asset name>
```

Run it from the `bin` directory of the [Call of Duty 4 mod tools](https://github.com/promod/CoD4-Mod-Tools).
Default options are read from `converter.cfg`.

## Notes

The build uses `/arch:IA32 /fp:precise` on purpose: the original is an x87
build, and SSE2 code generation changes floating-point results and therefore
the output bytes.

Like the original, the executable imports `LIBMYSQL.dll` (shipped with the mod
tools). `asset_manager\libmysql.def` lists the functions used, and the build
generates the import library from it, so the MySQL SDK is not needed.
