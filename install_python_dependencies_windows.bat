@echo off
REM Install Python dependencies for OpenAI Harmony - Windows
REM This script sets up the Python virtual environment and installs required packages

echo ========================================
echo OpenAI Harmony Python Dependencies Installer
echo ========================================
echo.

REM Check if Python is installed
where python >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Python not found. Please install Python first.
    exit /b 1
)

echo Python version:
python --version
echo.

REM Check if virtual environment exists
if not exist ".venv" (
    echo [ERROR] Virtual environment not found.
    echo Please run: python -m venv .venv
    exit /b 1
)

echo [OK] Virtual environment found: .venv
echo.

REM Activate virtual environment
echo Activating virtual environment...
call .venv\Scripts\activate.bat
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to activate virtual environment
    exit /b 1
)

echo [OK] Virtual environment activated
echo.

REM Upgrade pip first
echo Upgrading pip...
python -m pip install --upgrade pip
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to upgrade pip
    exit /b 1
)

echo [OK] Pip upgraded
echo.

REM Install maturin (required for building Rust-Python bindings)
echo Installing maturin...
pip install "maturin>=1.8,<2.0"
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to install maturin
    exit /b 1
)

echo [OK] Maturin installed
echo.

REM Install pydantic (required dependency)
echo Installing pydantic...
pip install "pydantic>=2.11.7"
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to install pydantic
    exit /b 1
)

echo [OK] Pydantic installed
echo.

REM Install pytest (for testing)
echo Installing pytest...
pip install pytest
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to install pytest
    exit /b 1
)

echo [OK] Pytest installed
echo.

REM Install optional demo dependencies
echo Installing optional demo dependencies...
pip install uvicorn fastapi
if %ERRORLEVEL% neq 0 (
    echo [WARNING] Failed to install demo dependencies (uvicorn, fastapi)
    echo This is optional and won't affect core functionality
) else (
    echo [OK] Demo dependencies installed
)

echo.

REM Show installed packages
echo Installed packages:
pip list
echo.

echo ========================================
echo Python dependencies installed successfully! [OK]
echo ========================================
echo.
echo Virtual environment: .venv
echo.
echo Next steps:
echo - Build Python bindings: build_python_rust_windows.bat
echo - Run Python tests: test_python_rust_windows.bat
echo.
echo To activate the virtual environment manually:
echo   .venv\Scripts\activate.bat
echo.
