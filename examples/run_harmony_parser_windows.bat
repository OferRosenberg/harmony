@echo off
echo ========================================
echo OpenAI Harmony Command-Line Parser (Windows)
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
echo ========================================
echo Harmony Parser Usage Examples:
echo ========================================
echo.
echo # Parse binary token file:
echo python harmony_parser.py tokens.bin
echo.
echo # Parse with verbose output:
echo python harmony_parser.py tokens.bin --verbose
echo.
echo # Show token preview:
echo python harmony_parser.py tokens.bin --show-tokens 10
echo.
echo # Compact output (messages only):
echo python harmony_parser.py tokens.bin --compact
echo.
echo # Summary only:
echo python harmony_parser.py tokens.bin --summary-only
echo.
echo ========================================

REM Check if arguments were provided
if "%~1"=="" (
    echo No arguments provided. Starting interactive mode...
    echo.
    echo Available binary files in current directory:
    dir *.bin /b 2>nul
    if errorlevel 1 (
        echo No .bin files found in current directory.
    )
    echo.
    echo Enter your command or type 'help' for usage:
    set /p "user_input=harmony_parser> "
    
    if /i "%user_input%"=="help" (
        python harmony_parser.py --help
    ) else if /i "%user_input%"=="exit" (
        echo Goodbye!
        goto :end
    ) else if not "%user_input%"=="" (
        python harmony_parser.py %user_input%
    )
) else (
    REM Pass all arguments to the Python script
    python harmony_parser.py %*
)

:end
echo.
pause
