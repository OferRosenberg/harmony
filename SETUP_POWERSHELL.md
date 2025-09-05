# PowerShell Setup Guide for OpenAI Harmony C++

This guide is specifically for setting up the OpenAI Harmony C++ library using PowerShell on Windows.

## Prerequisites Check

Your system already has:
- ✅ CMake 3.31.8
- ✅ Visual Studio 2022 Community
- ✅ Git 2.47.0
- ✅ vcpkg (installed at C:\vcpkg)

## PowerShell Environment Setup

### 1. Set Environment Variables (Current Session)
```powershell
# Add vcpkg to PATH for current session
$env:PATH += ";C:\vcpkg"

# Set VCPKG_ROOT for current session
$env:VCPKG_ROOT = "C:\vcpkg"

# Verify vcpkg is working
vcpkg version
```

### 2. Set Environment Variables (Permanent)
```powershell
# Add to system PATH permanently (requires admin)
[Environment]::SetEnvironmentVariable("Path", $env:PATH + ";C:\vcpkg", "Machine")

# Set VCPKG_ROOT permanently (requires admin)
[Environment]::SetEnvironmentVariable("VCPKG_ROOT", "C:\vcpkg", "Machine")
```

Or use the GUI:
1. Press `Win + R`, type `sysdm.cpl`, press Enter
2. Click "Environment Variables"
3. Under "System Variables", select "Path" and click "Edit"
4. Click "New" and add `C:\vcpkg`
5. Click "New" again and add a new variable `VCPKG_ROOT` with value `C:\vcpkg`

## Install Dependencies

```powershell
# Install required packages
vcpkg install nlohmann-json:x64-windows
vcpkg install openssl:x64-windows
vcpkg install curl:x64-windows
vcpkg install gtest:x64-windows

# Verify installations
vcpkg list
```

## Build the Project

### Option 1: Use Build Script
```powershell
# Run the Windows build script
.\build_windows.bat
```

### Option 2: Manual Build with PowerShell
```powershell
# Create build directory
New-Item -ItemType Directory -Force -Path "build"
Set-Location "build"

# Configure with CMake
cmake .. -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
         -DCMAKE_BUILD_TYPE=Release `
         -DBUILD_EXAMPLES=ON `
         -DBUILD_TESTS=ON `
         -G "Visual Studio 17 2022" `
         -A x64

# Build
cmake --build . --config Release

# Return to project root
Set-Location ..
```

## Run Examples

```powershell
# Navigate to build output
Set-Location "build\Release"

# Run examples
.\harmony_example.exe
.\interactive_example.exe

# Run tests (if built)
.\harmony_tests.exe
```

## Common PowerShell vs CMD Differences

| Task | Command Prompt (cmd) | PowerShell |
|------|---------------------|------------|
| Set environment variable | `set VAR=value` | `$env:VAR = "value"` |
| Add to PATH | `set PATH=%PATH%;C:\path` | `$env:PATH += ";C:\path"` |
| Check if file exists | `if exist file.txt` | `Test-Path "file.txt"` |
| Create directory | `mkdir dirname` | `New-Item -ItemType Directory "dirname"` |
| Change directory | `cd dirname` | `Set-Location "dirname"` |
| List files | `dir` | `Get-ChildItem` or `ls` |

## Troubleshooting

### vcpkg Command Not Found
```powershell
# Check if vcpkg exists
Test-Path "C:\vcpkg\vcpkg.exe"

# Add to PATH if needed
$env:PATH += ";C:\vcpkg"

# Test again
vcpkg version
```

### Build Errors
```powershell
# Clean build directory
Remove-Item -Recurse -Force "build" -ErrorAction SilentlyContinue
New-Item -ItemType Directory "build"

# Reconfigure
Set-Location "build"
cmake .. -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" -DBUILD_EXAMPLES=ON
```

### DLL Not Found at Runtime
The build system automatically copies DLLs to the executable directory, but if you get DLL errors:

```powershell
# Check if DLLs are in the right place
Get-ChildItem "build\Release\*.dll"

# Manually copy if needed
Copy-Item "build\Release\openai_harmony.dll" -Destination "build\Release\"
```

## Development Workflow

```powershell
# For development, use Debug configuration
Set-Location "build"
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build . --config Debug

# Run tests
Set-Location "Debug"
.\harmony_tests.exe
```

## Integration with Your Project

After building and installing, use in your CMake project:

```cmake
find_package(openai_harmony CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE openai_harmony::openai_harmony)
```

Or manually in PowerShell:
```powershell
# Compile your program
cl your_program.cpp /I"C:\vcpkg\installed\x64-windows\include" openai_harmony.lib
