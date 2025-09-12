#!/bin/bash
set -e

echo "========================================"
echo "OpenAI Harmony Vocabulary File Downloader (Linux)"
echo "========================================"
echo

# Create vocab_files directory if it doesn't exist
if [ ! -d "../../vocab_files" ]; then
    echo "Creating vocab_files directory..."
    mkdir -p ../../vocab_files
fi

echo "Downloading vocabulary files..."
echo

# Download o200k_base.tiktoken
echo "[1/2] Downloading o200k_base.tiktoken..."
if curl -L --progress-bar -o ../../vocab_files/o200k_base.tiktoken \
    "https://openaipublic.blob.core.windows.net/encodings/o200k_base.tiktoken"; then
    echo "[OK] o200k_base.tiktoken downloaded successfully"
else
    echo "[ERROR] Failed to download o200k_base.tiktoken"
    exit 1
fi

# Download cl100k_base.tiktoken
echo "[2/2] Downloading cl100k_base.tiktoken..."
if curl -L --progress-bar -o ../../vocab_files/cl100k_base.tiktoken \
    "https://openaipublic.blob.core.windows.net/encodings/cl100k_base.tiktoken"; then
    echo "[OK] cl100k_base.tiktoken downloaded successfully"
else
    echo "[ERROR] Failed to download cl100k_base.tiktoken"
    exit 1
fi

echo
echo "========================================"
echo "Vocabulary files downloaded successfully!"
echo "========================================"
echo

echo "Files saved to vocab_files/ directory:"
ls -la ../../vocab_files/

echo
echo "You can now build OpenAI Harmony without reqwest dependency."
echo "Run: ./build_rust_linux.sh"
