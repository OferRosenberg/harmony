# OpenAI Harmony Build Utilities

Build scripts, installation utilities, and build documentation for the OpenAI Harmony project.

## Directory Structure

### Platform-Specific Builds
- **`windows/`** - Windows build scripts and documentation
- **`linux/`** - Linux build scripts and documentation

### Cross-Platform Documentation
- Platform-specific troubleshooting guides are in respective directories

## Quick Setup

### Windows
```cmd
cd windows
download_vocab_files_rust_windows.bat
install_python_dependencies_windows.bat
build_rust_windows.bat
build_python_rust_windows.bat
test_rust_windows.bat && test_python_rust_windows.bat
```

### Linux
```bash
cd linux
chmod +x *.sh
./download_vocab_files_linux.sh
./install_python_dependencies_linux.sh
./build_rust_linux.sh
./build_python_rust_linux.sh
./test_rust_linux.sh && ./test_python_rust_linux.sh
```

## Platform-Specific Guides

- **Windows users** → See `windows/README.md` and `windows/README_RUST_WINDOWS.md`
- **Linux users** → See `linux/README.md` for quick start, or `linux/LINUX_BUILD_SUMMARY.md` and `linux/COMPREHENSIVE_BUILD_GUIDE.md` for detailed guides

## Common Requirements

Both platforms need:
- Rust toolchain (rustc, cargo)
- Python 3.8+ with pip
- Platform-specific build tools (see platform directories)

## Troubleshooting

For installation issues, see:
- **Linux issues** → `linux/LINUX_TROUBLESHOOTING.md`
- **Windows issues** → `windows/README_RUST_WINDOWS.md`
- **Platform-specific issues** → See respective platform directory READMEs

**Verify installation:**
```bash
rustc --version && python -c "import openai_harmony; print('✅ Success!')"
