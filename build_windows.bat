@echo off
REM Windows build script for OpenAI Harmony C++ library

echo Building OpenAI Harmony C++ Library for Windows
echo ===============================================

REM Check if vcpkg is available
where vcpkg >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: vcpkg not found in PATH
    echo.
    echo If you're using PowerShell, run these commands first:
    echo   $env:PATH += ";C:\vcpkg"
    echo   $env:VCPKG_ROOT = "C:\vcpkg"
    echo.
    echo Or see SETUP_POWERSHELL.md for detailed PowerShell setup
    echo.
    echo For Command Prompt, add C:\vcpkg to your system PATH
    echo See: https://github.com/Microsoft/vcpkg
    exit /b 1
)

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with vcpkg toolchain
echo Configuring with CMake...
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake ^
         -DCMAKE_BUILD_TYPE=Release ^
         -DBUILD_EXAMPLES=ON ^
         -DBUILD_TESTS=ON ^
         -G "Visual Studio 17 2022" ^
         -A x64

if %ERRORLEVEL% NEQ 0 (
    echo Error: CMake configuration failed
    exit /b 1
)

REM Build the project
echo Building...
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo Error: Build failed
    exit /b 1
)

echo.
echo Build completed successfully!
echo.
echo Executables are located in:
echo   build/Release/harmony_example.exe
echo   build/Release/interactive_example.exe
echo.
echo DLL is located in:
echo   build/Release/openai_harmony.dll
echo.
echo To run the examples:
echo   cd build/Release
echo   harmony_example.exe
echo   interactive_example.exe

cd ..
