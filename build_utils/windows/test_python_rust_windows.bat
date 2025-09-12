@echo off
REM Test script for OpenAI Harmony Python bindings - Windows
REM This script runs Python tests using the virtual environment

echo ========================================
echo OpenAI Harmony Python Tests (Windows)
echo ========================================
echo.

REM Check if virtual environment exists
if not exist ".venv" (
    echo [ERROR] Virtual environment not found.
    echo Please run: python -m venv .venv
    echo Then run: install_python_dependencies_windows.bat
    exit /b 1
)

REM Activate virtual environment
echo Activating virtual environment...
call .venv\Scripts\activate.bat
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to activate virtual environment
    exit /b 1
)

echo [OK] Virtual environment activated
echo.

REM Check if pytest is installed
python -c "import pytest" >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Pytest not found in virtual environment.
    echo Please run: install_python_dependencies_windows.bat
    exit /b 1
)

REM Check if openai_harmony is installed
python -c "import openai_harmony" >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] openai_harmony package not found.
    echo Please run: build_python_rust_windows.bat
    exit /b 1
)

echo [OK] openai_harmony package found
echo.

REM Check for vocabulary files
if not exist "vocab_files\o200k_base.tiktoken" (
    echo [ERROR] Vocabulary files not found.
    echo Please run: download_vocab_files_rust_windows.bat
    exit /b 1
)

if not exist "vocab_files\cl100k_base.tiktoken" (
    echo [ERROR] cl100k_base.tiktoken not found.
    echo Please run: download_vocab_files_rust_windows.bat
    exit /b 1
)

echo [OK] Vocabulary files found:
dir vocab_files\*.tiktoken
echo.

REM Set environment variable to use local vocabulary files
echo Setting TIKTOKEN_ENCODINGS_BASE to vocab_files directory...
set TIKTOKEN_ENCODINGS_BASE=vocab_files
echo [OK] TIKTOKEN_ENCODINGS_BASE=%TIKTOKEN_ENCODINGS_BASE%
echo.

echo Running Python tests for OpenAI Harmony...
echo Test file: tests\test_harmony.py
echo.

REM Run pytest with verbose output
pytest tests\test_harmony.py -v
if %ERRORLEVEL% neq 0 (
    echo.
    echo [ERROR] Python tests failed
    exit /b 1
)

echo.
echo ========================================
echo All Python tests passed successfully! [OK]
echo ========================================
echo.
echo Test coverage includes:
echo - Python bindings functionality
echo - Message rendering and parsing
echo - Tokenization with local vocab files
echo - Conversation handling
echo - Streaming parser
echo - Error handling
echo.
echo Environment:
echo - TIKTOKEN_ENCODINGS_BASE=%TIKTOKEN_ENCODINGS_BASE%
echo - Virtual environment: .venv
echo - No reqwest dependency (uses local vocab files)
echo.
