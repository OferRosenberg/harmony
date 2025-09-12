@echo off
REM Test script for OpenAI Harmony (Rust) - Windows without reqwest dependency
REM This script runs Rust tests using local vocabulary files

echo ========================================
echo OpenAI Harmony Rust Tests (Windows)
echo ========================================
echo.

REM Check if Rust is installed
where cargo >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Cargo not found. Please install Rust first.
    echo Visit: https://rustup.rs/
    exit /b 1
)

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

echo Running Rust tests for OpenAI Harmony...
echo Using manifest: Cargo.toml (Windows version without reqwest)
echo.

REM Run unit tests
echo [1/3] Running unit tests...
cargo test --release --no-default-features --lib
if %ERRORLEVEL% neq 0 (
    echo.
    echo [ERROR] Unit tests failed
    exit /b 1
)

echo.
echo [2/3] Running documentation tests...
cargo test --release --no-default-features --doc
if %ERRORLEVEL% neq 0 (
    echo.
    echo [ERROR] Documentation tests failed
    exit /b 1
)

echo.
echo [3/3] Running all tests together...
cargo test --release --no-default-features
if %ERRORLEVEL% neq 0 (
    echo.
    echo [ERROR] Full test suite failed
    exit /b 1
)

echo.
echo ========================================
echo All Rust tests passed successfully! [OK]
echo ========================================
echo.
echo Test coverage includes:
echo - Core renderer functionality
echo - Message parsing and formatting  
echo - Tokenization (with local vocab files)
echo - Error handling
echo - Documentation examples
echo.
echo Environment:
echo - TIKTOKEN_ENCODINGS_BASE=%TIKTOKEN_ENCODINGS_BASE%
echo - No reqwest dependency (uses local vocab files)
echo.
