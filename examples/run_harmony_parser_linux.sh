#!/bin/bash
set -e

echo "========================================"
echo "OpenAI Harmony Command-Line Parser (Linux)"
echo "========================================"
echo

# Check if virtual environment exists
if [ ! -d "../.venv" ]; then
    echo "[ERROR] Virtual environment not found!"
    echo "Please run: ../build_utils/linux/install_python_dependencies_linux.sh"
    exit 1
fi

# Check if vocab files exist
if [ ! -d "../vocab_files" ] || [ ! -f "../vocab_files/o200k_base.tiktoken" ] || [ ! -f "../vocab_files/cl100k_base.tiktoken" ]; then
    echo "[ERROR] Vocabulary files not found!"
    echo "Please run: ../build_utils/linux/download_vocab_files_linux.sh"
    exit 1
fi

echo "[OK] Virtual environment found: ../.venv"
echo "[OK] Vocabulary files found: ../vocab_files"
echo

echo "Activating virtual environment..."
source ../.venv/bin/activate

echo "Setting TIKTOKEN_ENCODINGS_BASE to vocab_files directory..."
export TIKTOKEN_ENCODINGS_BASE=../vocab_files

echo
echo "========================================"
echo "Harmony Parser Usage Examples:"
echo "========================================"
echo
echo "# Parse binary token file:"
echo "python3 harmony_parser.py tokens.bin"
echo
echo "# Parse with verbose output:"
echo "python3 harmony_parser.py tokens.bin --verbose"
echo
echo "# Show token preview:"
echo "python3 harmony_parser.py tokens.bin --show-tokens 10"
echo
echo "# Compact output (messages only):"
echo "python3 harmony_parser.py tokens.bin --compact"
echo
echo "# Summary only:"
echo "python3 harmony_parser.py tokens.bin --summary-only"
echo
echo "========================================"

# Check if arguments were provided
if [ $# -eq 0 ]; then
    echo "No arguments provided. Starting interactive mode..."
    echo
    echo "Available binary files in current directory:"
    ls -1 *.bin 2>/dev/null || echo "No .bin files found in current directory."
    echo
    echo "Enter your command or type 'help' for usage:"
    read -p "harmony_parser> " user_input
    
    if [ "$user_input" = "help" ]; then
        python3 harmony_parser.py --help
    elif [ "$user_input" = "exit" ]; then
        echo "Goodbye!"
        exit 0
    elif [ -n "$user_input" ]; then
        # Execute the user input as arguments to the script
        eval "python3 harmony_parser.py $user_input"
    fi
else
    # Pass all arguments to the Python script
    python3 harmony_parser.py "$@"
fi

echo
echo "Harmony parser session complete."
