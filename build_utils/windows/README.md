# Windows Build Scripts

Windows-specific build scripts and documentation for OpenAI Harmony.

## Build Scripts

- **`build_rust_windows.bat`** - Build Rust library
- **`build_python_rust_windows.bat`** - Build Python bindings
- **`install_python_dependencies_windows.bat`** - Install Python dependencies
- **`download_vocab_files_rust_windows.bat`** - Download vocabulary files
- **`test_rust_windows.bat`** - Test Rust library
- **`test_python_rust_windows.bat`** - Test Python bindings

## Documentation

- **`README_RUST_WINDOWS.md`** - Comprehensive Windows build guide

## Quick Setup

```cmd
download_vocab_files_rust_windows.bat
install_python_dependencies_windows.bat
build_rust_windows.bat
build_python_rust_windows.bat
test_rust_windows.bat && test_python_rust_windows.bat
```

## Requirements

- Rust toolchain (rustc, cargo)
- Python 3.8+ with pip
- Visual Studio Build Tools or Visual Studio
- Git

## Common Issues

- **Missing Visual Studio Build Tools** → Install from Microsoft
- **Python not found** → Add Python to PATH
- **Permission errors** → Run as Administrator if needed

For detailed troubleshooting, see `../RUST_INSTALLATION_TROUBLESHOOTING.md`.
