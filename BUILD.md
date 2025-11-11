# Building WAMount

This document provides detailed instructions for building WAMount from source.

## Prerequisites

### Required Software

1. **Windows 10 or 11** (64-bit recommended)

2. **WinFsp** (Windows File System Proxy)
   - Download from: https://winfsp.dev/
   - Install the latest stable version (1.12 or later)
   - Default installation path: `C:\Program Files (x86)\WinFsp`

3. **Build Tools** (choose one):

   **Option A: Visual Studio 2019 or later**
   - Download from: https://visualstudio.microsoft.com/
   - Install "Desktop development with C++" workload
   - Includes CMake and MSBuild

   **Option B: MinGW-w64**
   - Download from: https://www.mingw-w64.org/
   - Or use MSYS2: https://www.msys2.org/
   - Install CMake separately: https://cmake.org/

4. **CMake 3.15 or later**
   - Download from: https://cmake.org/download/
   - Add to PATH during installation

## Building with Visual Studio

### Method 1: Using CMake GUI

1. Open CMake GUI
2. Set "Where is the source code" to the WAMount directory
3. Set "Where to build the binaries" to `wamount/build`
4. Click "Configure"
5. Select "Visual Studio 16 2019" (or your version) as generator
6. Select "x64" as platform
7. Click "Finish"
8. If WinFsp is not found, set `WINFSP_PATH` to your installation directory
9. Click "Generate"
10. Click "Open Project" to open in Visual Studio
11. Build solution (F7) or right-click WAMount → Build

### Method 2: Using Command Line

```cmd
# Open "Developer Command Prompt for VS 2019"
cd path\to\wamount
mkdir build
cd build

# Configure
cmake -G "Visual Studio 16 2019" -A x64 ..

# Or specify WinFsp path explicitly
cmake -G "Visual Studio 16 2019" -A x64 -DWINFSP_PATH="C:\Program Files (x86)\WinFsp" ..

# Build
cmake --build . --config Release

# The executable will be in build\Release\wamount.exe
```

## Building with MinGW

```cmd
# Open terminal with MinGW in PATH
cd path\to\wamount
mkdir build
cd build

# Configure
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..

# Build
cmake --build .

# Or use make directly
mingw32-make

# The executable will be in build\wamount.exe
```

## Building with MSYS2

```bash
# Install dependencies
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake make

# Navigate to source
cd /path/to/wamount

# Build
mkdir build
cd build
cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make

# The executable will be in build/wamount.exe
```

## Troubleshooting

### WinFsp Not Found

If CMake cannot find WinFsp, you can specify the path:

```cmd
cmake -DWINFSP_PATH="C:\Path\To\WinFsp" ..
```

Or set an environment variable:

```cmd
set WINFSP_PATH=C:\Path\To\WinFsp
cmake ..
```

### Missing DLL at Runtime

If you get "winfsp-x64.dll not found" when running wamount.exe:

1. Copy `winfsp-x64.dll` from `C:\Program Files (x86)\WinFsp\bin\` to the same directory as `wamount.exe`
2. Or add `C:\Program Files (x86)\WinFsp\bin\` to your system PATH

### Compiler Errors

**C++ Standard Issues:**
Ensure your compiler supports C++17:
- Visual Studio 2017 15.7 or later
- GCC 7 or later
- Clang 5 or later

**Missing Headers:**
Make sure WinFsp SDK is properly installed with headers in `WinFsp/inc/`

## Testing the Build

After building, test with:

```cmd
# Show help
wamount.exe --help

# Try mounting (requires admin privileges)
# Replace X: with available drive letter
# Replace PhysicalDrive1 with your APFS disk
wamount.exe -m X: \\.\PhysicalDrive1
```

## Installation

To install to a specific directory:

```cmd
cmake --install . --prefix "C:\Program Files\WAMount"
```

Or using Visual Studio:
1. Right-click "INSTALL" project in Solution Explorer
2. Select "Build"

## Debug Build

For development and debugging:

```cmd
# Visual Studio
cmake -G "Visual Studio 16 2019" -A x64 ..
cmake --build . --config Debug

# MinGW
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

Debug builds include:
- Symbol information for debugging
- Assertions enabled
- Debug logging enabled by default

## Clean Build

To start fresh:

```cmd
# Delete build directory
cd ..
rmdir /s /q build
mkdir build
cd build
cmake ..
```

## Advanced Configuration

### Custom Compiler Flags

```cmd
cmake -DCMAKE_CXX_FLAGS="/W4 /O2" ..
```

### Static Linking (MinGW)

```cmd
cmake -DCMAKE_CXX_FLAGS="-static" ..
```

### Building with Clang

```cmd
cmake -G "Ninja" -DCMAKE_CXX_COMPILER=clang++ ..
ninja
```

## Supported Configurations

Tested and working:
- ✅ Visual Studio 2019 (x64)
- ✅ Visual Studio 2022 (x64)
- ✅ MinGW-w64 GCC 11+ (x64)
- ✅ MSYS2 GCC (x64)

Not tested:
- ❓ 32-bit builds (may work with adjustments)
- ❓ Clang on Windows

## Next Steps

After building successfully:
1. Read [README.md](README.md) for usage instructions
2. Make sure WinFsp service is running
3. Run with administrator privileges
4. Try mounting an APFS partition

## Getting Help

If you encounter build issues:
1. Check that all prerequisites are installed
2. Verify WinFsp is installed correctly
3. Try a clean build
4. Check the error messages for missing dependencies
5. Open an issue on GitHub with:
   - Your OS version
   - Compiler version
   - CMake version
   - Full error message
