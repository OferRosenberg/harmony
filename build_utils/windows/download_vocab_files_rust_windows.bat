@echo off
REM Download vocabulary files for OpenAI Harmony - Windows build
REM This script downloads the required .tiktoken files to avoid reqwest dependency

echo ========================================
echo OpenAI Harmony Vocabulary File Downloader
echo ========================================
echo.

REM Create vocab_files directory
if not exist "vocab_files" (
    echo Creating vocab_files directory...
    mkdir vocab_files
)

echo Downloading vocabulary files...
echo.

REM Download o200k_base.tiktoken (used by both O200kBase and O200kHarmony)
echo [1/2] Downloading o200k_base.tiktoken...
powershell -Command "try { Invoke-WebRequest -Uri 'https://openaipublic.blob.core.windows.net/encodings/o200k_base.tiktoken' -OutFile 'vocab_files\o200k_base.tiktoken' -UseBasicParsing; Write-Host '[OK] o200k_base.tiktoken downloaded successfully' } catch { Write-Host '[ERROR] Failed to download o200k_base.tiktoken:' $_.Exception.Message; exit 1 }"
if %ERRORLEVEL% neq 0 exit /b 1

REM Download cl100k_base.tiktoken
echo [2/2] Downloading cl100k_base.tiktoken...
powershell -Command "try { Invoke-WebRequest -Uri 'https://openaipublic.blob.core.windows.net/encodings/cl100k_base.tiktoken' -OutFile 'vocab_files\cl100k_base.tiktoken' -UseBasicParsing; Write-Host '[OK] cl100k_base.tiktoken downloaded successfully' } catch { Write-Host '[ERROR] Failed to download cl100k_base.tiktoken:' $_.Exception.Message; exit 1 }"
if %ERRORLEVEL% neq 0 exit /b 1

echo.
echo ========================================
echo Vocabulary files downloaded successfully!
echo ========================================
echo.
echo Files saved to vocab_files\ directory:
dir vocab_files\*.tiktoken
echo.
echo You can now build OpenAI Harmony without reqwest dependency.
echo Run: build_rust_windows.bat
echo.
