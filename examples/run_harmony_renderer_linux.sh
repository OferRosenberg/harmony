#!/bin/bash
set -e

echo "========================================"
echo "OpenAI Harmony Command-Line Renderer (Linux)"
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
echo "Harmony Renderer Usage Examples:"
echo "========================================"
echo
echo "# Render a prompt and save as JSON:"
echo "python3 harmony_renderer.py \"What is 2+2?\" -o output.json"
echo
echo "# Render from file and save as text:"
echo "python3 harmony_renderer.py --input prompt.txt -o output.txt"
echo
echo "# Save raw tokens:"
echo "python3 harmony_renderer.py \"Hello world\" -o tokens.bin"
echo
echo "# Verbose output with first 10 tokens:"
echo "python3 harmony_renderer.py \"Explain AI\" -o result.json --verbose --show-tokens 10"
echo
echo "========================================"

# Check if arguments were provided
if [ $# -eq 0 ]; then
    echo "No arguments provided. Starting interactive mode..."
    echo
    echo "Enter your command or type 'help' for usage:"
    read -p "harmony_renderer> " user_input
    
    if [ "$user_input" = "help" ]; then
        python3 harmony_renderer.py --help
    elif [ "$user_input" = "exit" ]; then
        echo "Goodbye!"
        exit 0
    elif [ -n "$user_input" ]; then
        # Execute the user input as arguments to the script
        eval "python3 harmony_renderer.py $user_input"
    fi
else
    # Pass all arguments to the Python script
    python3 harmony_renderer.py "$@"
fi

echo
echo "Harmony renderer session complete."
