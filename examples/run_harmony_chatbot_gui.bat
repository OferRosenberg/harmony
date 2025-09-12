@echo off
echo ========================================
echo OpenAI Harmony Chatbot GUI Launcher
echo ========================================
echo.

REM Check if virtual environment exists
if not exist ".venv" (
    echo [ERROR] Virtual environment not found!
    echo Please run: install_python_dependencies_windows.bat
    pause
    exit /b 1
)

REM Check if vocab files exist
if not exist "vocab_files" (
    echo [ERROR] Vocabulary files not found!
    echo Please run: download_vocab_files_rust_windows.bat
    pause
    exit /b 1
)

echo [OK] Virtual environment found: .venv
echo [OK] Vocabulary files found: vocab_files
echo.

echo Activating virtual environment...
call .venv\Scripts\activate.bat

echo Setting TIKTOKEN_ENCODINGS_BASE to vocab_files directory...
set TIKTOKEN_ENCODINGS_BASE=vocab_files

echo.
echo Starting OpenAI Harmony Chatbot GUI...
echo.
echo ========================================
echo GUI Features:
echo - Enter any prompt in the text area
echo - Click "Process with Harmony Format" 
echo - View first 20 tokens in results window
echo - See parsed message components
echo - View complete rendered harmony format
echo ========================================
echo.

python harmony_chatbot_gui.py

echo.
echo Chatbot GUI closed.
pause
