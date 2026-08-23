@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
set "VENV_DIR=%SCRIPT_DIR%.venv"
set "PYTHON=%VENV_DIR%\Scripts\python.exe"

if not exist "%PYTHON%" (
    python -m venv "%VENV_DIR%"
    if errorlevel 1 exit /b %errorlevel%

    "%PYTHON%" -m pip install -r "%SCRIPT_DIR%requirements.txt"
    if errorlevel 1 exit /b %errorlevel%
)

"%PYTHON%" "%SCRIPT_DIR%LogDecrypt.py" %*
exit /b %errorlevel%
