#!/bin/bash
set -e

echo "========================================"
echo "OpenAI Harmony GUI Chatbot (Linux)"
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

# Check if tkinter is available
python3 -c "import tkinter" 2>/dev/null || {
    echo "[ERROR] tkinter not found!"
    echo "Please install tkinter:"
    echo "  Ubuntu/Debian: sudo apt install python3-tk"
    echo "  CentOS/RHEL/Fedora: sudo dnf install tkinter"
    echo "  Arch Linux: sudo pacman -S tk"
    exit 1
}

echo "[OK] Virtual environment found: ../.venv"
echo "[OK] Vocabulary files found: ../vocab_files"
echo "[OK] tkinter available"
echo

echo "Activating virtual environment..."
source ../.venv/bin/activate

echo "Setting TIKTOKEN_ENCODINGS_BASE to vocab_files directory..."
export TIKTOKEN_ENCODINGS_BASE=../vocab_files

echo
echo "========================================"
echo "Starting Harmony GUI Chatbot..."
echo "========================================"
echo
echo "The GUI application will open in a new window."
echo "If you don't see it, check your taskbar or window manager."
echo
echo "Features:"
echo "- Interactive harmony format exploration"
echo "- Real-time token rendering and parsing"
echo "- Message component analysis"
echo "- Export functionality"
echo
echo "Press Ctrl+C to stop the application."
echo "========================================"

# Launch the GUI application
python3 harmony_chatbot_gui.py "$@"

echo
echo "Harmony GUI chatbot session complete."
