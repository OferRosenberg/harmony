# OpenAI Harmony - ARM64 Windows Compatibility

This document describes the **minimal fix** for OpenAI Harmony's ARM64 Windows compatibility issue and provides convenient Windows build scripts.

## Overview

The standard OpenAI Harmony build includes `reqwest` for downloading vocabulary files at runtime. However, `reqwest` depends on the `ring` crate, which has compilation issues on ARM64 Windows due to missing clang/LLVM toolchain requirements.

This **minimal fix** solves the ARM64 Windows compatibility issue by:
1. Adding feature flags to make `reqwest` optional
2. Pre-downloading vocabulary files using PowerShell
3. Building without `reqwest` using `--no-default-features`
4. Setting `TIKTOKEN_ENCODINGS_BASE` to use local files

## Quick Start

### Option 1: Using Convenient Batch Files (Recommended)
```cmd
# 1. Download vocabulary files (one-time setup)
download_vocab_files_rust_windows.bat

# 2. Build Rust library (ARM64 Windows compatible)
build_rust_windows.bat

# 3. Build Python bindings (optional)
build_python_rust_windows.bat

# 4. Run tests
test_rust_windows.bat
```

### Option 2: Direct Cargo Commands
```cmd
# 1. Download vocabulary files (one-time setup)
download_vocab_files_rust_windows.bat

# 2. Build without reqwest (ARM64 Windows compatible)
cargo build --release --no-default-features

# 3. Run tests
cargo test --release --no-default-features

# 4. Build Python bindings (optional)
maturin develop --release --no-default-features --features python-binding
```

## The Minimal Fix

### Changes Made to Core Files

#### 1. Cargo.toml (3 changes only)
```toml
[features]
default = ["remote-vocab-download"]          # ← ADDED
remote-vocab-download = ["reqwest"]          # ← ADDED
python-binding = ["pyo3"]
wasm-binding = ["wasm-bindgen", "serde-wasm-bindgen", "wasm-bindgen-futures"]

[dependencies]
# ... all original dependencies preserved ...
reqwest = { version = "0.12.5", optional = true, default-features = false, features = [...] }
#                                ↑ ADDED: "optional = true"
```

#### 2. Conditional Compilation (already existed)
The `src/tiktoken_ext/public_encodings.rs` already had conditional compilation:
```rust
#[cfg(feature = "remote-vocab-download")]
fn load_remote_file() { /* Uses reqwest */ }

#[cfg(not(feature = "remote-vocab-download"))]
fn load_remote_file() { /* Uses local files */ }
```

### Build Behavior
- **Default build**: `cargo build --release` (includes reqwest, may fail on ARM64 Windows)
- **ARM64 Windows build**: `cargo build --release --no-default-features` (no reqwest, works)

## Files Created

### Vocabulary Files
- `vocab_files/o200k_base.tiktoken` - 3.6MB vocabulary file
- `vocab_files/cl100k_base.tiktoken` - 1.7MB vocabulary file

### Convenient Windows Scripts
- `download_vocab_files_rust_windows.bat` - Downloads vocabulary files using PowerShell
- `build_rust_windows.bat` - Builds Rust library without reqwest
- `test_rust_windows.bat` - Runs comprehensive Rust tests
- `build_python_rust_windows.bat` - Builds Python bindings without reqwest

## Requirements

### System Requirements
- **Windows 10/11** (x64 or ARM64)
- **Rust toolchain** (stable channel)
- **Python 3.8+** (for Python bindings)
- **PowerShell** (for downloading vocabulary files)

### Rust Installation
```cmd
# Install Rust from https://rustup.rs/
# Or use winget:
winget install Rustlang.Rustup
```

### Python Dependencies (for Python bindings)
```cmd
pip install maturin
```

## How It Works

### 1. Feature Flag System
The minimal fix uses Cargo's feature system:
- **`remote-vocab-download`** - Enables reqwest for downloading vocab files
- **Default features** - Include `remote-vocab-download` for backward compatibility
- **`--no-default-features`** - Disables reqwest, uses local files only

### 2. Vocabulary File Management
Instead of downloading files at runtime with `reqwest`:
- Downloads files at build time using PowerShell's `Invoke-WebRequest`
- Stores files in `vocab_files/` directory
- Sets `TIKTOKEN_ENCODINGS_BASE=vocab_files` environment variable
- Uses conditional compilation to handle missing reqwest

### 3. Build Outputs
- **Rust Library**: `target/release/libopenai_harmony.rlib`
- **C-Compatible Library**: `target/release/openai_harmony.dll`
- **Python Package**: Installed in development mode

## Compatibility

### ✅ Supported Platforms
- **Windows x64** (Intel/AMD) - Both with and without reqwest
- **Windows ARM64** (Qualcomm Snapdragon) - Without reqwest only
- **Apple Silicon Macs** (via Parallels/VMware) - Without reqwest only

### ✅ Supported Features
- Core message rendering and parsing
- Tokenization with local vocabulary files
- Python bindings via PyO3
- All unit and integration tests (25 tests + 1 doc test)
- WASM support (with pre-downloaded vocab files)

### ❌ Limitations (ARM64 Windows only)
- No automatic vocabulary file downloading at runtime
- Vocabulary files must be present before running
- Must use `--no-default-features` flag

## Troubleshooting

### Build Fails with "vocabulary files not found"
```cmd
# Run the download script first
download_vocab_files_rust_windows.bat
```

### Build Fails with "failed to find tool 'clang'"
```cmd
# This happens with default build on ARM64 Windows
# Use the no-reqwest build instead:
cargo build --release --no-default-features
```

### Python import fails
```cmd
# Make sure Python bindings were built without reqwest
build_python_rust_windows.bat

# Or manually:
maturin develop --release --no-default-features --features python-binding

# Test the import
python -c "import openai_harmony; print('[OK] Import successful')"
```

### Tests fail with encoding errors
```cmd
# Ensure TIKTOKEN_ENCODINGS_BASE is set
set TIKTOKEN_ENCODINGS_BASE=vocab_files
cargo test --release --no-default-features
```

## Comparison with Standard Build

| Feature | Standard Build | ARM64 Windows Build |
|---------|---------------|---------------------|
| **Dependencies** | Includes `reqwest` + `ring` | No `reqwest` or `ring` |
| **ARM64 Windows** | ❌ Fails (ring compilation) | ✅ Works |
| **Build Command** | `cargo build --release` | `cargo build --release --no-default-features` |
| **Vocab Files** | Downloads at runtime | Pre-downloaded locally |
| **Network Access** | Required at runtime | Only at build time |
| **Core Features** | ✅ Full functionality | ✅ Full functionality |
| **Python Bindings** | ✅ Supported | ✅ Supported |
| **WASM Support** | ✅ Supported | ✅ Supported |
| **Backward Compatibility** | ✅ Default behavior | ✅ Preserved |

## Integration

### Using in Rust Projects
```toml
# For ARM64 Windows compatibility
[dependencies]
openai-harmony = { path = "path/to/harmony", default-features = false }

# For standard platforms (includes reqwest)
[dependencies]
openai-harmony = { path = "path/to/harmony" }
```

### Using in Python Projects
```python
import openai_harmony

# Load encoding with local vocab files
encoding = openai_harmony.load_harmony_encoding('HarmonyGptOss')

# Create and render messages
message = openai_harmony.Message.from_role_and_content(
    openai_harmony.Role.User, 
    "Hello, world!"
)
tokens = encoding.render([message])
```

### Environment Setup
```cmd
# Set environment variable to use local vocab files
set TIKTOKEN_ENCODINGS_BASE=vocab_files

# Or in PowerShell
$env:TIKTOKEN_ENCODINGS_BASE = "vocab_files"
```

## Technical Details

### Minimal Changes Summary
The fix required only **3 changes** to the codebase:
1. **2 lines added** to `[features]` section in Cargo.toml
2. **1 word added** (`optional = true`) to reqwest dependency
3. **Conditional compilation** (already existed in the code)

### Feature Flag Logic
```toml
# Default behavior (backward compatible)
default = ["remote-vocab-download"]

# When remote-vocab-download is enabled
remote-vocab-download = ["reqwest"]  # Include reqwest

# When disabled (--no-default-features)
# reqwest is not included, uses local files only
```

### Vocabulary Files
- **o200k_base.tiktoken** - Used by O200kBase and O200kHarmony encodings
- **cl100k_base.tiktoken** - Used by Cl100kBase encoding
- **Total size**: ~5.3MB
- **Hash verification**: Files are verified against expected SHA256 hashes

## Contributing

When making changes to this system:
1. **Test on ARM64 Windows** if possible (Qualcomm Snapdragon PCs)
2. **Preserve backward compatibility** - default build should still work on standard platforms
3. **Update batch files** if changing build commands
4. **Verify vocabulary file handling** - ensure `TIKTOKEN_ENCODINGS_BASE` is set
5. **Test both feature configurations**:
   - With reqwest: `cargo build --release`
   - Without reqwest: `cargo build --release --no-default-features`

## License

Same as OpenAI Harmony main project - Apache 2.0 License.
