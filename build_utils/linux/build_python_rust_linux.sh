#!/bin/bash
set -e

echo "========================================"
echo "OpenAI Harmony Python Builder (Linux)"
echo "========================================"
echo

# Check if virtual environment exists
if [ ! -d "../../.venv" ]; then
    echo "[ERROR] Virtual environment not found!"
    echo "Please run: ./install_python_dependencies_linux.sh"
    exit 1
fi

echo "Activating virtual environment..."
source ../../.venv/bin/activate
echo "[OK] Virtual environment activated"

# Check build tools
echo "Build tools info:"
rustc --version
python3 --version
maturin --version
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

echo "Building Python wheel for OpenAI Harmony..."
echo "Using manifest: Cargo.toml (Linux version without reqwest)"
echo

# Change to project root directory for maturin
cd ../..

# Build Python wheel with maturin
if TIKTOKEN_ENCODINGS_BASE=vocab_files maturin develop --release --features python-binding --no-default-features; then
    echo
    echo "========================================"
    echo "Python wheel build completed successfully! [OK]"
    echo "========================================"
    echo
    echo "The openai_harmony package has been installed in development mode."
    echo
    echo "Environment:"
    echo "- TIKTOKEN_ENCODINGS_BASE=vocab_files"
    echo "- No reqwest dependency (uses local vocab files)"
    echo
    echo "Test the installation:"
    echo "  python3 -c \"import openai_harmony; print('[OK] Import successful')\""
    echo
    echo "Next steps:"
    echo "- Run tests: ./test_python_rust_linux.sh"
else
    echo
    echo "[ERROR] Python wheel build failed!"
    echo "Common issues:"
    echo "- Missing build tools: sudo apt install build-essential python3-dev"
    echo "- Missing Rust: curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh"
    echo "- Missing maturin: pip install maturin"
    exit 1
fi
