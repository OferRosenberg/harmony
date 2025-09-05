# Cross-Platform Build Guide for OpenAI Harmony C++

This guide explains how to build the OpenAI Harmony C++ library on both Windows and Linux platforms.

## Prerequisites

### Common Requirements
- CMake 3.20 or higher
- C++20 compatible compiler
- Git

### Windows Requirements
- Visual Studio 2022 (Community, Professional, or Enterprise)
- vcpkg package manager
- Windows 10/11

### Linux Requirements
- GCC 10+ or Clang 12+
- pkg-config
- Development packages for dependencies

## Dependencies

The library requires the following dependencies:

- **nlohmann/json**: JSON library for C++
- **OpenSSL**: Cryptographic library
- **libcurl**: HTTP client library
- **Google Test** (optional, for tests)

## Windows Build Instructions

### 1. Install vcpkg

If you don't have vcpkg installed:

```cmd
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

Add vcpkg to your PATH and set the VCPKG_ROOT environment variable:
```cmd
set PATH=%PATH%;C:\path\to\vcpkg
set VCPKG_ROOT=C:\path\to\vcpkg
```

### 2. Install Dependencies

```cmd
vcpkg install nlohmann-json:x64-windows
vcpkg install openssl:x64-windows
vcpkg install curl:x64-windows
vcpkg install gtest:x64-windows
```

### 3. Build the Library

Use the provided Windows build script:

```cmd
build_windows.bat
```

Or manually:

```cmd
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake ^
         -DCMAKE_BUILD_TYPE=Release ^
         -DBUILD_EXAMPLES=ON ^
         -DBUILD_TESTS=ON ^
         -G "Visual Studio 17 2022" ^
         -A x64
cmake --build . --config Release
```

### 4. Run Examples

```cmd
cd build\Release
harmony_example.exe
interactive_example.exe
```

## Linux Build Instructions

### 1. Install Dependencies

#### Ubuntu/Debian:
```bash
sudo apt update
sudo apt install build-essential cmake pkg-config
sudo apt install nlohmann-json3-dev libssl-dev libcurl4-openssl-dev
sudo apt install libgtest-dev  # Optional, for tests
```

#### RHEL/CentOS/Fedora:
```bash
sudo dnf install gcc-c++ cmake pkg-config
sudo dnf install nlohmann-json-devel openssl-devel libcurl-devel
sudo dnf install gtest-devel  # Optional, for tests
```

### 2. Build the Library

Use the provided Linux build script:

```bash
chmod +x build_linux.sh
./build_linux.sh
```

Or manually:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON
make -j$(nproc)
```

### 3. Run Examples

```bash
cd build
./harmony_example
./interactive_example
```

## Build Options

The following CMake options are available:

- `BUILD_EXAMPLES=ON/OFF`: Build example executables (default: OFF)
- `BUILD_TESTS=ON/OFF`: Build test suite (default: OFF)
- `CMAKE_BUILD_TYPE`: Build type (Debug/Release/RelWithDebInfo/MinSizeRel)

Example with custom options:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_EXAMPLES=ON -DBUILD_TESTS=OFF
```

## Output Files

### Windows
- **DLL**: `build/Release/openai_harmony.dll`
- **Import Library**: `build/Release/openai_harmony.lib`
- **Examples**: `build/Release/harmony_example.exe`, `build/Release/interactive_example.exe`
- **Tests**: `build/Release/harmony_tests.exe`

### Linux
- **Shared Library**: `build/libopenai_harmony.so`
- **Examples**: `build/harmony_example`, `build/interactive_example`
- **Tests**: `build/harmony_tests`

## Installation

### Windows
```cmd
cmake --build . --config Release --target install
```

### Linux
```bash
sudo make install
```

This will install:
- Headers to `/usr/local/include/openai_harmony/` (Linux) or `C:\Program Files\openai_harmony\include\` (Windows)
- Library to `/usr/local/lib/` (Linux) or `C:\Program Files\openai_harmony\lib\` (Windows)
- CMake config files for easy integration with other projects

## Using the Library in Your Project

### CMake Integration

After installation, you can use the library in your CMake project:

```cmake
find_package(openai_harmony REQUIRED)
target_link_libraries(your_target PRIVATE openai_harmony)
```

### Manual Integration

Include the headers and link against the library:

```cpp
#include <openai_harmony/harmony.hpp>
```

**Windows:**
```cmd
cl your_program.cpp /I"path\to\include" openai_harmony.lib
```

**Linux:**
```bash
g++ your_program.cpp -I/path/to/include -lopenai_harmony
```

## Troubleshooting

### Windows Issues

1. **vcpkg not found**: Ensure vcpkg is in your PATH and VCPKG_ROOT is set
2. **Visual Studio not found**: Install Visual Studio 2022 with C++ development tools
3. **DLL not found at runtime**: Ensure the DLL is in the same directory as your executable or in PATH

### Linux Issues

1. **Missing dependencies**: Install development packages for all dependencies
2. **Compiler too old**: Ensure you have GCC 10+ or Clang 12+ for C++20 support
3. **Library not found at runtime**: Add the library path to LD_LIBRARY_PATH or use ldconfig

### Common Issues

1. **CMake version too old**: Update to CMake 3.20 or higher
2. **C++20 not supported**: Update your compiler
3. **nlohmann/json not found**: Install the development package for your platform

## Development

For development builds, use Debug configuration:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
```

This enables debugging symbols and additional runtime checks.

## Contributing

When contributing to the project:

1. Ensure your changes work on both Windows and Linux
2. Test with both Debug and Release builds
3. Run the test suite: `./harmony_tests` (Linux) or `harmony_tests.exe` (Windows)
4. Update documentation if adding new features

## License

See the LICENSE file for license information.
