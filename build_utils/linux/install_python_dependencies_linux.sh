#!/bin/bash
set -e

echo "========================================"
echo "OpenAI Harmony Python Dependencies Installer (Linux)"
echo "========================================"
echo

# Check if Python 3 is installed
if ! command -v python3 &> /dev/null; then
    echo "[ERROR] Python 3 not found!"
    echo "Please install Python 3:"
    echo "  Ubuntu/Debian: sudo apt install python3 python3-pip python3-venv python3-dev"
    echo "  CentOS/RHEL/Fedora: sudo dnf install python3 python3-pip python3-devel"
    echo "  Arch Linux: sudo pacman -S python python-pip"
    exit 1
fi

echo "Python version:"
python3 --version
echo

# Check if virtual environment exists
if [ ! -d "../../.venv" ]; then
    echo "Creating Python virtual environment..."
    if python3 -m venv ../../.venv; then
        echo "[OK] Virtual environment created: ../../.venv"
    else
        echo "[ERROR] Failed to create virtual environment"
        echo "Make sure python3-venv is installed:"
        echo "  Ubuntu/Debian: sudo apt install python3-venv"
        exit 1
    fi
else
    echo "[OK] Virtual environment found: ../../.venv"
fi

echo
echo "Activating virtual environment..."
source ../../.venv/bin/activate
echo "[OK] Virtual environment activated"
echo

echo "Upgrading pip..."
if pip install --upgrade pip; then
    echo "[OK] Pip upgraded"
else
    echo "[ERROR] Failed to upgrade pip"
    exit 1
fi

echo
echo "Installing maturin..."
if pip install "maturin>=1.8,<2.0"; then
    echo "[OK] Maturin installed"
else
    echo "[ERROR] Failed to install maturin"
    echo "Make sure you have build tools installed:"
    echo "  Ubuntu/Debian: sudo apt install build-essential python3-dev"
    echo "  CentOS/RHEL/Fedora: sudo dnf groupinstall 'Development Tools' && sudo dnf install python3-devel"
    exit 1
fi

echo
echo "Installing pydantic..."
if pip install "pydantic>=2.11.7"; then
    echo "[OK] Pydantic installed"
else
    echo "[ERROR] Failed to install pydantic"
    exit 1
fi

echo
echo "Installing pytest..."
if pip install pytest; then
    echo "[OK] Pytest installed"
else
    echo "[ERROR] Failed to install pytest"
    exit 1
fi

echo
echo "Installing optional demo dependencies..."
if pip install uvicorn fastapi; then
    echo "This is optional and won't affect core functionality"
    echo "[OK] Demo dependencies installed"
else
    echo "[WARNING] Failed to install demo dependencies (uvicorn, fastapi)"
    echo "This is optional and won't affect core functionality"
fi

echo
echo "Installed packages:"
pip list

echo
echo "========================================"
echo "Python dependencies installed successfully! [OK]"
echo "========================================"
echo
echo "Virtual environment: .venv"
echo
echo "Next steps:"
echo "- Build Python bindings: ./build_python_rust_linux.sh"
echo "- Run Python tests: ./test_python_rust_linux.sh"
echo
echo "To activate the virtual environment manually:"
echo "  source .venv/bin/activate"
