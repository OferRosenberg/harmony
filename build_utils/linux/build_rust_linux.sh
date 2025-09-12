#!/bin/bash
set -e

echo "========================================"
echo "OpenAI Harmony Rust Builder (Linux)"
echo "========================================"
echo

# Check if Rust is installed
if ! command -v rustc &> /dev/null; then
    echo "[ERROR] Rust not found!"
    echo "Please install Rust: curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh"
    exit 1
fi

# Check if Cargo is installed
if ! command -v cargo &> /dev/null; then
    echo "[ERROR] Cargo not found!"
    echo "Please install Rust: curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh"
    exit 1
fi

echo "Rust toolchain info:"
rustc --version
cargo --version
echo

# Check if vocabulary files exist
if [ ! -d "../../vocab_files" ] || [ ! -f "../../vocab_files/o200k_base.tiktoken" ] || [ ! -f "../../vocab_files/cl100k_base.tiktoken" ]; then
    echo "[ERROR] Vocabulary files not found!"
    echo "Please run: ./download_vocab_files_linux.sh"
    exit 1
fi

echo "[OK] Vocabulary files found:"
ls -la ../../vocab_files/
echo

echo "Setting TIKTOKEN_ENCODINGS_BASE to vocab_files directory..."
export TIKTOKEN_ENCODINGS_BASE=vocab_files
echo "[OK] TIKTOKEN_ENCODINGS_BASE=$TIKTOKEN_ENCODINGS_BASE"
echo

echo "Building OpenAI Harmony (Release mode, no reqwest)..."
echo "Using manifest: Cargo.toml (Linux version without reqwest)"
echo

# Change to project root directory for cargo
cd ../..

# Build the Rust library without reqwest dependency
if TIKTOKEN_ENCODINGS_BASE=vocab_files cargo build --release --no-default-features; then
    echo
    echo "========================================"
    echo "Build completed successfully! [OK]"
    echo "========================================"
    echo
    echo "Output files:"
    echo "- target/release/libopenai_harmony.so"
    echo "- target/release/libopenai_harmony.rlib"
    echo
    echo "Environment:"
    echo "- TIKTOKEN_ENCODINGS_BASE=vocab_files"
    echo "- No reqwest dependency (uses local vocab files)"
    echo
    echo "Next steps:"
    echo "- For Python bindings: ./build_python_rust_linux.sh"
    echo "- For tests: ./test_rust_linux.sh"
else
    echo
    echo "[ERROR] Build failed!"
    echo "Check the error messages above for details."
    exit 1
fi
