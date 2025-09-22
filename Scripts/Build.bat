@echo off
REM Cross-platform build script for Windows
REM This script installs dependencies and builds the project using Conan and CMake

setlocal enabledelayedexpansion

echo === Cross-Platform Build Script for Windows ===

REM Copy Pre-Commit Hook to .git/hooks
if exist .git (
    echo Setting up pre-commit hook...
    copy .github\hooks\pre-commit .git\hooks\pre-commit
) else (
    echo WARNING: .git directory not found. Skipping pre-commit hook setup.
)

REM Function to check if command exists
where python >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Python is not installed or not in PATH
    echo Please install Python from https://python.org
    pause
    exit /b 1
)

REM Check for pip
python -m pip --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: pip is not available
    echo Please ensure pip is installed with Python
    pause
    exit /b 1
)

echo Step 1: Creating Build directory...
if not exist Build mkdir Build

echo Step 2: Installing pipx...
python -m pip install --user pipx
if %errorlevel% neq 0 (
    echo WARNING: pipx installation failed, trying alternative method...
    pip install pipx
)

REM Ensure pipx is in PATH
set "PIPX_BIN=%USERPROFILE%\.local\bin"
set "PATH=%PIPX_BIN%;%PATH%"

REM Alternative: Check AppData path for pipx
set "PIPX_BIN_ALT=%LOCALAPPDATA%\Programs\Python\Python*\Scripts"
for /d %%i in (%PIPX_BIN_ALT%) do (
    if exist "%%i\pipx.exe" (
        set "PATH=%%i;!PATH!"
        goto pipx_found
    )
)

:pipx_found
echo Step 3: Ensuring pipx path...
pipx ensurepath
if %errorlevel% neq 0 (
    echo WARNING: pipx ensurepath failed, continuing...
)

echo Step 4: Installing Conan via pipx...
pipx install conan
if %errorlevel% neq 0 (
    echo ERROR: Failed to install Conan with pipx
    echo Trying alternative installation method...
    python -m pip install --user conan
    if !errorlevel! neq 0 (
        echo ERROR: Failed to install Conan
        pause
        exit /b 1
    )
)

REM Ensure conan is in PATH
where conan >nul 2>&1
if %errorlevel% neq 0 (
    echo Adding Conan to PATH...
    set "CONAN_BIN=%USERPROFILE%\.local\bin"
    set "PATH=%CONAN_BIN%;%PATH%"
    
    REM Check alternative locations
    for /d %%i in ("%LOCALAPPDATA%\Programs\Python\Python*\Scripts") do (
        if exist "%%i\conan.exe" (
            set "PATH=%%i;!PATH!"
            goto conan_found
        )
    )
)

:conan_found
echo Step 5: Creating Conan default profile...
conan profile detect --force
if %errorlevel% neq 0 (
    echo ERROR: Failed to create Conan profile
    pause
    exit /b 1
)

echo Step 6: Installing dependencies with Conan...
conan install . --output-folder=Build --build=missing --settings=build_type=Release --settings=compiler.cppstd=20 --settings=compiler=gcc --settings=compiler.version=11 -c tools.system.package_manager:mode=install
if %errorlevel% neq 0 (
    echo ERROR: Conan install failed
    pause
    exit /b 1
)

echo Step 7: Checking for CMake...
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: CMake is not installed or not in PATH
    echo Please install CMake from https://cmake.org/download/
    echo Or install Visual Studio with CMake tools
    pause
    exit /b 1
)

echo Step 8: Configuring CMake...
cmake -B Build -S . -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed
    pause
    exit /b 1
)

echo Step 9: Building the project...
cmake --build Build --config Release
if %errorlevel% neq 0 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo === Build completed successfully! ===
echo Your built project should be available in the Build directory.

echo Available binaries:
for /r Build\bin %%f in (*.exe) do (
    echo     %%~nxf
)
pause
