@echo off
REM Enhanced cross-platform build script for Windows
REM This script installs dependencies and builds the project using Conan and CMake

setlocal enabledelayedexpansion

echo === Enhanced Build Script for Windows ===
echo Detected OS: Windows
for /f "tokens=2 delims==" %%a in ('wmic os get name /value ^| find "Name"') do set OS_NAME=%%a
echo OS Version: %OS_NAME:~0,10%
echo Architecture: %PROCESSOR_ARCHITECTURE%
echo.

REM Enhanced error handling
if "%1"=="--help" (
    echo Usage: %0 [options]
    echo Options:
    echo   --help          Show this help message
    echo   --clean         Clean build directory before building  
    echo   --verbose       Enable verbose output
    echo   --jobs=N        Set number of parallel build jobs
    exit /b 0
)

set CLEAN_BUILD=0
set VERBOSE=0
set BUILD_JOBS=%NUMBER_OF_PROCESSORS%

REM Parse command line arguments
:parse_args
if "%1"=="--clean" (
    set CLEAN_BUILD=1
    shift
    goto parse_args
)
if "%1"=="--verbose" (
    set VERBOSE=1
    shift  
    goto parse_args
)
if "%1" neq "" (
    if "%1:~0,7%"=="--jobs=" (
        set BUILD_JOBS=%1:~7%
        shift
        goto parse_args
    )
)
if "%1" neq "" shift & goto parse_args

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
REM Clean build directory if requested
if %CLEAN_BUILD%==1 (
    echo Step 1a: Cleaning build directory...
    if exist Build rmdir /s /q Build
    echo Build directory cleaned.
    echo.
)

echo Step 5: Creating Conan default profile...
conan profile detect --force
if %errorlevel% neq 0 (
    echo ERROR: Failed to create Conan profile
    pause
    exit /b 1
)
echo ✓ Conan profile created successfully
echo.

REM Detect compiler automatically  
set DETECTED_COMPILER=msvc
set COMPILER_VERSION=193

REM Try to detect Visual Studio version
if defined VS170COMNTOOLS set COMPILER_VERSION=193
if defined VS160COMNTOOLS set COMPILER_VERSION=192  
if defined VS150COMNTOOLS set COMPILER_VERSION=191

echo Detected compiler: %DETECTED_COMPILER% version %COMPILER_VERSION%
echo.

echo Step 6: Installing dependencies with Conan...
conan install . --output-folder=Build --build=missing ^
    --settings=build_type=Release ^
    --settings=compiler.cppstd=20 ^
    --settings=compiler=%DETECTED_COMPILER% ^
    --settings=compiler.version=%COMPILER_VERSION% ^
    -c tools.system.package_manager:mode=install
if %errorlevel% neq 0 (
    echo ERROR: Conan install failed
    pause
    exit /b 1
)
echo ✓ Conan dependencies installed successfully
echo.

echo Step 7: Checking for CMake...
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: CMake is not installed or not in PATH
    echo Please install CMake from https://cmake.org/download/
    echo Or install Visual Studio with CMake tools
    pause
    exit /b 1
)
echo ✓ CMake found
echo.

echo Step 8: Configuring CMake with Conan integration...
cmake -B Build -S . ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
    -DCMAKE_TOOLCHAIN_FILE=Build\build\Release\generators\conan_toolchain.cmake
if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed
    pause
    exit /b 1
)
echo ✓ CMake configured successfully
echo.

echo Step 9: Building the project with %BUILD_JOBS% parallel jobs...
cmake --build Build --config Release --parallel %BUILD_JOBS%
if %errorlevel% neq 0 (
    echo ERROR: Build failed
    pause
    exit /b 1
)
echo ✓ Project built successfully
echo.

echo.
echo ============================================
echo ✓ BUILD COMPLETED SUCCESSFULLY! ✓
echo ============================================
echo.
echo Your built project is available in the Build directory.
echo.

echo Available executables:
if exist Build\bin (
    for %%f in (Build\bin\*.exe) do (
        echo   • %%~nxf
    )
) else (
    echo   No executables found in Build\bin
)
echo.

echo Available libraries:
if exist Build\lib (
    for %%f in (Build\lib\*.lib Build\lib\*.dll) do (
        echo   • %%~nxf  
    )
)
if exist Build\libs (
    for %%f in (Build\libs\*.lib Build\libs\*.dll) do (
        echo   • %%~nxf
    )
)
echo.

echo Quick start commands:
echo   To run tests: cd Build ^&^& bin\TKDEngineTests.exe
echo   To run client: cd Build ^&^& bin\TKDEngineClient.exe
echo   To run server: cd Build ^&^& bin\TKDEngineServer.exe
echo.

if %VERBOSE%==1 (
    echo Build configuration:
    echo   Compiler: %DETECTED_COMPILER% %COMPILER_VERSION%
    echo   Build type: Release  
    echo   C++ standard: 20
    echo   Parallel jobs: %BUILD_JOBS%
    echo.
)

echo Press any key to exit...
pause >nul
