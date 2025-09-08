@echo off
REM Build script for OpenAI Harmony Python bindings - Windows without reqwest dependency
REM This script builds Python wheels using local vocabulary files

echo ========================================
echo OpenAI Harmony Python Builder (Windows)
echo ========================================
echo.

REM Check if Rust is installed
where cargo >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Cargo not found. Please install Rust first.
    echo Visit: https://rustup.rs/
    exit /b 1
)

REM Check if Python is installed
where python >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Python not found. Please install Python first.
    exit /b 1
)

REM Check if Maturin is installed
where maturin >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Maturin not found. Installing...
    pip install maturin
    REM Check again after installation
    where maturin >nul 2>nul
    if %ERRORLEVEL% neq 0 (
        echo [ERROR] Failed to install maturin or maturin not in PATH
        echo Try restarting your terminal or adding Python Scripts to PATH
        exit /b 1
    )
)

REM Display versions
echo Build tools info:
rustc --version
python --version
maturin --version
echo.

REM Check for vocabulary files
if not exist "vocab_files\o200k_base.tiktoken" (
    echo [ERROR] Vocabulary files not found.
    echo Please run: download_vocab_files_rust_windows.bat
    echo.
    echo This will download the required .tiktoken files to avoid reqwest dependency.
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

echo Building Python wheel for OpenAI Harmony...
echo Using manifest: Cargo.toml (Windows version without reqwest)
echo.

REM Build Python wheel with local vocabulary files (no reqwest)
maturin develop --release --no-default-features --features python-binding
if %ERRORLEVEL% neq 0 (
    echo.
    echo [ERROR] Failed to build Python wheel
    exit /b 1
)

echo.
echo ========================================
echo Python wheel build completed successfully! [OK]
echo ========================================
echo.
echo The openai_harmony package has been installed in development mode.
echo.
echo Environment:
echo - TIKTOKEN_ENCODINGS_BASE=%TIKTOKEN_ENCODINGS_BASE%
echo - No reqwest dependency (uses local vocab files)
echo.
echo Test the installation:
echo   python -c "import openai_harmony; print('[OK] Import successful')"
echo.
echo Next steps:
echo - Run tests: test_python_rust_windows.bat
echo.
