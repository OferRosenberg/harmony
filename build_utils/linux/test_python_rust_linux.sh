#!/bin/bash
set -e

echo "========================================"
echo "OpenAI Harmony Python Tests (Linux)"
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
echo

# Check if openai_harmony package is installed
if python3 -c "import openai_harmony" 2>/dev/null; then
    echo "[OK] openai_harmony package found"
else
    echo "[ERROR] openai_harmony package not found!"
    echo "Please run: ./build_python_rust_linux.sh"
    exit 1
fi

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

echo "Running Python tests for OpenAI Harmony..."
echo "Test file: tests/test_harmony.py"
echo

# Change to project root directory for pytest
cd ../..

# Run pytest with verbose output
if TIKTOKEN_ENCODINGS_BASE=vocab_files pytest tests/ -v; then
    echo
    echo "========================================"
    echo "All Python tests passed successfully! [OK]"
    echo "========================================"
    echo
    echo "Test coverage includes:"
    echo "- Python bindings functionality"
    echo "- Message rendering and parsing"
    echo "- Tokenization with local vocab files"
    echo "- Conversation handling"
    echo "- Streaming parser"
    echo "- Error handling"
    echo
    echo "Environment:"
    echo "- TIKTOKEN_ENCODINGS_BASE=vocab_files"
    echo "- Virtual environment: .venv"
    echo "- No reqwest dependency (uses local vocab files)"
else
    echo
    echo "[ERROR] Python tests failed!"
    echo "Common issues:"
    echo "- Missing openai_harmony package: ./build_python_rust_linux.sh"
    echo "- Missing vocabulary files: ./download_vocab_files_linux.sh"
    echo "- Missing test dependencies: pip install pytest"
    exit 1
fi
