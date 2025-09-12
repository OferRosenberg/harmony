@echo off
REM Build script for OpenAI Harmony (Rust) - Windows without reqwest dependency
REM This script builds the core Rust library using local vocabulary files

echo ========================================
echo OpenAI Harmony Rust Builder (Windows)
echo ========================================
echo.

REM Check if Rust is installed
where cargo >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Cargo not found. Please install Rust first.
    echo Visit: https://rustup.rs/
    exit /b 1
)

REM Display Rust version
echo Rust toolchain info:
rustc --version
cargo --version
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

echo Building OpenAI Harmony (Release mode, no reqwest)...
echo Using manifest: Cargo.toml (Windows version without reqwest)
echo.

REM Build the core library without reqwest (disable remote vocab download feature)
cargo build --release --no-default-features
if %ERRORLEVEL% neq 0 (
    echo.
    echo [ERROR] Failed to build OpenAI Harmony
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully! [OK]
echo ========================================
echo.
echo Output files:
echo - target\release\openai_harmony.dll
echo - target\release\libopenai_harmony.rlib
echo.
echo Environment:
echo - TIKTOKEN_ENCODINGS_BASE=%TIKTOKEN_ENCODINGS_BASE%
echo - No reqwest dependency (uses local vocab files)
echo.
echo Next steps:
echo - For Python bindings: build_python_rust_windows.bat
echo - For tests: test_rust_windows.bat
echo.
