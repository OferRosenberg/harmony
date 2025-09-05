# Building OpenAI Harmony C++ on Windows

Quick guide for building the OpenAI Harmony C++ library on Windows.

## Prerequisites

1. **Visual Studio 2022** (Community, Professional, or Enterprise)
   - Install with "Desktop development with C++" workload
   - Ensure CMake tools are included

2. **vcpkg** (Package Manager)
   ```cmd
   git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
   cd C:\vcpkg
   .\bootstrap-vcpkg.bat
   ```

3. **Environment Variables**
   ```cmd
   set PATH=%PATH%;C:\vcpkg
   set VCPKG_ROOT=C:\vcpkg
   ```

## Install Dependencies

```cmd
vcpkg install nlohmann-json:x64-windows
vcpkg install openssl:x64-windows
vcpkg install curl:x64-windows
vcpkg install gtest:x64-windows
```

## Build

### Option 1: Use Build Script (Recommended)
```cmd
build_windows.bat
```

### Option 2: Manual Build
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

## Run Examples

```cmd
cd build\Release
harmony_example.exe
interactive_example.exe
```

## Output Files

- **DLL**: `build/Release/openai_harmony.dll`
- **Import Library**: `build/Release/openai_harmony.lib`
- **Examples**: `build/Release/*.exe`

## Troubleshooting

### Common Issues

1. **vcpkg not found**
   - Ensure vcpkg is in PATH: `where vcpkg`
   - Set VCPKG_ROOT environment variable

2. **Visual Studio not found**
   - Install Visual Studio 2022 with C++ development tools
   - Or use Visual Studio Build Tools

3. **Dependencies not found**
   - Verify vcpkg packages are installed: `vcpkg list`
   - Use correct triplet: `:x64-windows`

4. **DLL not found at runtime**
   - The build script automatically copies DLLs to executable directory
   - Or add DLL path to system PATH

### Build Configurations

```cmd
# Debug build
cmake --build . --config Debug

# Release build
cmake --build . --config Release

# Clean build
cmake --build . --config Release --target clean
```

## Integration with Your Project

### Using CMake
```cmake
find_package(openai_harmony REQUIRED)
target_link_libraries(your_target PRIVATE openai_harmony::openai_harmony)
```

### Manual Linking
```cpp
#include <openai_harmony/harmony.hpp>
```

Link with: `openai_harmony.lib`

## Development

For development, use Debug configuration:
```cmd
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build . --config Debug
```

Run tests:
```cmd
cd build\Debug
harmony_tests.exe
```

## See Also

- [BUILD_CROSS_PLATFORM.md](BUILD_CROSS_PLATFORM.md) - Complete cross-platform guide
- [README_CPP.md](README_CPP.md) - Full C++ library documentation
