#!/bin/bash
set -e

echo "========================================"
echo "OpenAI Harmony Rust Tests (Linux)"
echo "========================================"
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

echo "Running Rust tests for OpenAI Harmony..."
echo "Using manifest: Cargo.toml (Linux version without reqwest)"
echo

# Change to project root directory for cargo
cd ../..

echo "[1/3] Running unit tests..."
if TIKTOKEN_ENCODINGS_BASE=vocab_files cargo test --release --no-default-features; then
    echo "[OK] Unit tests passed"
else
    echo "[ERROR] Unit tests failed"
    exit 1
fi

echo
echo "[2/3] Running documentation tests..."
if cargo test --release --no-default-features --doc; then
    echo "[OK] Documentation tests passed"
else
    echo "[ERROR] Documentation tests failed"
    exit 1
fi

echo
echo "[3/3] Running all tests together..."
if cargo test --release --no-default-features --all-targets; then
    echo "[OK] All tests passed"
else
    echo "[ERROR] Some tests failed"
    exit 1
fi

echo
echo "========================================"
echo "All Rust tests passed successfully! [OK]"
echo "========================================"
echo
echo "Test coverage includes:"
echo "- Core renderer functionality"
echo "- Message parsing and formatting"
echo "- Tokenization (with local vocab files)"
echo "- Error handling"
echo "- Documentation examples"
echo
echo "Environment:"
echo "- TIKTOKEN_ENCODINGS_BASE=vocab_files"
echo "- No reqwest dependency (uses local vocab files)"
