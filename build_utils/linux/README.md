# Linux Build Scripts

Linux-specific build scripts and documentation for OpenAI Harmony.

## Build Scripts

- **`build_rust_linux.sh`** - Build Rust library
- **`build_python_rust_linux.sh`** - Build Python bindings
- **`install_python_dependencies_linux.sh`** - Install Python dependencies
- **`download_vocab_files_linux.sh`** - Download vocabulary files
- **`test_rust_linux.sh`** - Test Rust library
- **`test_python_rust_linux.sh`** - Test Python bindings
- **`test_python.sh`** - Basic Python test
- **`run_checks.sh`** - Code quality checks (format, clippy, tests)

## Documentation

### Build Guides
- **`LINUX_BUILD_SUMMARY.md`** - **START HERE** - Quick reference for getting started
- **`COMPREHENSIVE_BUILD_GUIDE.md`** - Detailed guide with full explanations and troubleshooting
- **`LINUX_TROUBLESHOOTING.md`** - Comprehensive troubleshooting for all Linux build issues

### Which Guide to Use?
- **New users or quick setup** → Use `LINUX_BUILD_SUMMARY.md`
- **Need detailed explanations** → Use `COMPREHENSIVE_BUILD_GUIDE.md`
- **Having build problems** → Use `LINUX_TROUBLESHOOTING.md`

## Quick Setup

```bash
chmod +x *.sh
./download_vocab_files_linux.sh
./install_python_dependencies_linux.sh
./build_rust_linux.sh
./build_python_rust_linux.sh
./test_rust_linux.sh && ./test_python_rust_linux.sh
```

## Requirements

- Rust toolchain (rustc, cargo)
- Python 3.8+ with pip and venv
- build-essential (gcc, make, pkg-config)
- Development libraries (libssl-dev, python3-dev)

## Common Issues

- **Missing build tools** → `sudo apt install build-essential` (Ubuntu)
- **Missing Python headers** → `sudo apt install python3-dev`
- **Script permissions** → Run `chmod +x *.sh` first
- **Rust installation issues** → See `LINUX_TROUBLESHOOTING.md`

For comprehensive troubleshooting, see `LINUX_TROUBLESHOOTING.md`.
