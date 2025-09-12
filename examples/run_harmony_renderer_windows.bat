@echo off
echo ========================================
echo OpenAI Harmony Command-Line Renderer (Windows)
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
echo Harmony Renderer Usage Examples:
echo ========================================
echo.
echo # Render a prompt and save as JSON:
echo python harmony_renderer.py "What is 2+2?" -o output.json
echo.
echo # Render from file and save as text:
echo python harmony_renderer.py --input prompt.txt -o output.txt
echo.
echo # Save raw tokens:
echo python harmony_renderer.py "Hello world" -o tokens.bin
echo.
echo # Verbose output with first 10 tokens:
echo python harmony_renderer.py "Explain AI" -o result.json --verbose --show-tokens 10
echo.
echo ========================================

REM Check if arguments were provided
if "%~1"=="" (
    echo No arguments provided. Starting interactive mode...
    echo.
    echo Enter your command or type 'help' for usage:
    set /p "user_input=harmony_renderer> "
    
    if /i "%user_input%"=="help" (
        python harmony_renderer.py --help
    ) else if /i "%user_input%"=="exit" (
        echo Goodbye!
        goto :end
    ) else if not "%user_input%"=="" (
        python harmony_renderer.py %user_input%
    )
) else (
    REM Pass all arguments to the Python script
    python harmony_renderer.py %*
)

:end
echo.
pause
