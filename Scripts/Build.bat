@echo off
REM Enhanced Windows Build Script - Installs Everything from ZERO
REM Requires: Windows 10/11 with winget (built-in)
REM This script will install Python, CMake, Visual Studio Build Tools, and build the project

setlocal enabledelayedexpansion

REM ============================================
REM Navigate to project root
REM ============================================
REM Get the directory where this script is located
set SCRIPT_DIR=%~dp0
REM Remove trailing backslash
set SCRIPT_DIR=%SCRIPT_DIR:~0,-1%

REM Navigate to parent directory (project root)
cd /d "%SCRIPT_DIR%\.."
if %errorlevel% neq 0 (
  echo ERROR: Failed to navigate to project root
  pause
  exit /b 1
)

REM Verify we're in the right place by checking for conanfile
if not exist "conanfile.txt" (
  if not exist "conanfile.py" (
      echo ERROR: Cannot find conanfile.txt or conanfile.py in project root
      echo Current directory: %CD%
      echo Script location: %SCRIPT_DIR%
      echo.
      echo Please ensure this script is in the Scripts folder of your project
      pause
      exit /b 1
  )
)

echo ========================================
echo Enhanced Build Script for Windows
echo Starting from ZERO dependencies
echo ========================================
echo Project Root: %CD%
echo Detected OS: Windows
echo Architecture: %PROCESSOR_ARCHITECTURE%
echo.

REM Parse command line arguments
set CLEAN_BUILD=0
set VERBOSE=0
set BUILD_JOBS=%NUMBER_OF_PROCESSORS%
set SKIP_DEPS=0

:parse_args
if "%1"=="--help" (
  echo Usage: %0 [options]
  echo Options:
  echo   --help         Show this help message
  echo   --clean        Clean build directory before building
  echo   --verbose      Enable verbose output
  echo   --jobs=N       Set number of parallel build jobs
  echo   --skip-deps    Skip dependency installation
  exit /b 0
)
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
if "%1"=="--skip-deps" (
  set SKIP_DEPS=1
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

REM ============================================
REM STEP 0: Check for winget (required)
REM ============================================
echo [STEP 0] Checking for winget package manager...
where winget >nul 2>&1
if %errorlevel% neq 0 (
  echo ERROR: winget is not available!
  echo winget is required and comes pre-installed with Windows 10 ^(1809+^) and Windows 11.
  echo.
  echo If you're on Windows 10, update to the latest version or install App Installer from Microsoft Store:
  echo https://www.microsoft.com/p/app-installer/9nblggh4nns1
  echo.
  pause
  exit /b 1
)
echo [OK] winget found
echo.

if %SKIP_DEPS%==1 goto skip_installation

REM ============================================
REM STEP 1: Install Python if not present
REM ============================================
echo [STEP 1] Checking for Python...
where python >nul 2>&1
if %errorlevel% neq 0 (
  echo Python not found. Installing Python 3.12...
  winget install Python.Python.3.12 --silent --accept-source-agreements --accept-package-agreements
  if !errorlevel! neq 0 (
      echo ERROR: Failed to install Python
      pause
      exit /b 1
  )
  echo Python installed successfully
  echo Refreshing environment variables...
  call refreshenv 2>nul
  timeout /t 3 /nobreak >nul
) else (
  echo [OK] Python already installed
)
echo.

REM ============================================
REM STEP 2: Install CMake if not present
REM ============================================
echo [STEP 2] Checking for CMake...
where cmake >nul 2>&1
if %errorlevel% neq 0 (
  echo CMake not found. Installing CMake...
  winget install Kitware.CMake --silent --accept-source-agreements --accept-package-agreements
  if !errorlevel! neq 0 (
      echo ERROR: Failed to install CMake
      pause
      exit /b 1
  )
  echo CMake installed successfully
  echo Refreshing environment variables...
  call refreshenv 2>nul
  timeout /t 3 /nobreak >nul
) else (
  echo [OK] CMake already installed
)
echo.

REM ============================================
REM STEP 3: Install Visual Studio Build Tools
REM ============================================
echo [STEP 3] Checking for Visual Studio Build Tools with C++ support...
set VS_FOUND=0
set MSVC_FOUND=0

REM Check for Visual Studio 2022 installations
if exist "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC" set VS_FOUND=1& set MSVC_FOUND=1
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC" set VS_FOUND=1& set MSVC_FOUND=1
if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC" set VS_FOUND=1& set MSVC_FOUND=1
if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC" set VS_FOUND=1& set MSVC_FOUND=1

REM Check for Visual Studio 2019 installations
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Tools\MSVC" set VS_FOUND=1& set MSVC_FOUND=1
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC" set VS_FOUND=1& set MSVC_FOUND=1

if %MSVC_FOUND%==1 (
  echo [OK] Visual Studio with C++ tools already installed
  echo.
) else (
  if %VS_FOUND%==1 (
      echo [WARNING] Visual Studio found but C++ tools not detected
      echo Installing C++ workload...
      echo This may take 10-15 minutes. Please be patient...
      echo.
  ) else (
      echo Visual Studio Build Tools not found. Installing VS 2022 Build Tools...
      echo This may take 10-15 minutes. Please be patient...
      echo.
  )
  
    winget install Microsoft.VisualStudio.2022.BuildTools --silent --override "--wait --quiet --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended" --accept-source-agreements --accept-package-agreements
  
    REM Check if installation was successful by verifying MSVC exists
    timeout /t 5 /nobreak >nul
    set MSVC_FOUND=0
    if exist "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC" set MSVC_FOUND=1
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC" set MSVC_FOUND=1
  
    if !MSVC_FOUND!==0 (
      echo.
      echo [WARNING] Automatic installation completed but C++ tools may not be fully configured.
      echo.
      echo This is usually fine - the build will continue and may work correctly.
      echo If the build fails with compiler errors, please manually install:
      echo https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
      echo.
      echo Make sure to select "Desktop development with C++" workload
      echo.
      echo Press any key to continue with the build...
      pause >nul
    ) else (
      echo [OK] Visual Studio Build Tools with C++ installed successfully
      echo.
    )
)
echo.

:skip_installation

REM ============================================
REM STEP 4: Refresh PATH and verify installations
REM ============================================
echo [STEP 4] Refreshing environment and verifying installations...

REM Manually add common installation paths to current session
set "PATH=%PATH%;C:\Program Files\CMake\bin"
set "PATH=%PATH%;C:\Program Files\Python312\Scripts;C:\Program Files\Python312"
set "PATH=%PATH%;C:\Program Files\Python311\Scripts;C:\Program Files\Python311"
set "PATH=%PATH%;C:\Users\%USERNAME%\AppData\Local\Programs\Python\Python312\Scripts"
set "PATH=%PATH%;C:\Users\%USERNAME%\AppData\Local\Programs\Python\Python312"
set "PATH=%PATH%;C:\Users\%USERNAME%\AppData\Local\Programs\Python\Python311\Scripts"
set "PATH=%PATH%;C:\Users\%USERNAME%\AppData\Local\Programs\Python\Python311"
set "PATH=%PATH%;%USERPROFILE%\.local\bin"
set "PATH=%PATH%;%USERPROFILE%\AppData\Roaming\Python\Python312\Scripts"
set "PATH=%PATH%;%USERPROFILE%\AppData\Roaming\Python\Python311\Scripts"

REM Verify Python
where python >nul 2>&1
if %errorlevel% neq 0 (
  echo ERROR: Python is still not in PATH after installation
  echo Please close this terminal and open a new one, then run the script again
  pause
  exit /b 1
)
echo [OK] Python verified: 
python --version

REM Verify pip
python -m pip --version >nul 2>&1
if %errorlevel% neq 0 (
  echo ERROR: pip is not available
  pause
  exit /b 1
)
echo [OK] pip verified

REM Verify CMake
where cmake >nul 2>&1
if %errorlevel% neq 0 (
  echo ERROR: CMake is still not in PATH after installation
  echo Please close this terminal and open a new one, then run the script again
  pause
  exit /b 1
)
echo [OK] CMake verified:
cmake --version | findstr /C:"cmake version"
echo.

REM ============================================
REM STEP 5: Setup pre-commit hook
REM ============================================
echo [STEP 5] Setting up pre-commit hook...
if exist .git (
  if exist .github\hooks\pre-commit (
      copy /Y .github\hooks\pre-commit .git\hooks\pre-commit >nul 2>&1
      echo [OK] Pre-commit hook installed
  )
) else (
  echo [WARNING] .git directory not found. Skipping pre-commit hook.
)
echo.

REM ============================================
REM STEP 6: Clean build directory if requested
REM ============================================
if %CLEAN_BUILD%==1 (
  echo [STEP 6] Cleaning build directory...
  if exist Build (
      rmdir /s /q Build
      echo [OK] Build directory cleaned
  )
  echo.
) else (
  echo [STEP 6] Creating Build directory...
)
if not exist Build mkdir Build
echo.

REM ============================================
REM STEP 7: Install and configure pipx
REM ============================================
echo [STEP 7] Installing pipx...
python -m pip install --user --upgrade pipx
if %errorlevel% neq 0 (
  echo WARNING: pipx installation encountered issues, trying alternative...
  python -m pip install --upgrade pipx
)

echo Ensuring pipx path...
python -m pipx ensurepath
echo.

REM ============================================
REM STEP 8: Install Conan via pipx
REM ============================================
echo [STEP 8] Installing Conan package manager...

REM Check if conan is already available
where conan >nul 2>&1
if %errorlevel% equ 0 (
  echo [OK] Conan already installed
  conan --version
) else (
  echo Installing Conan via pipx...
  python -m pipx install conan
  if !errorlevel! neq 0 (
      echo WARNING: pipx install failed, trying pip install...
      python -m pip install --user conan
      if !errorlevel! neq 0 (
      echo ERROR: Failed to install Conan
      pause
      exit /b 1
      )
  )
  echo [OK] Conan installed
)

REM Update PATH to include Conan
set "PATH=%USERPROFILE%\.local\bin;%PATH%"
set "PATH=%USERPROFILE%\AppData\Roaming\Python\Python312\Scripts;%PATH%"
set "PATH=%USERPROFILE%\AppData\Roaming\Python\Python311\Scripts;%PATH%"

REM Final verification
where conan >nul 2>&1
if %errorlevel% neq 0 (
  echo ERROR: Conan is not accessible in PATH
  echo Please close this terminal and open a new one, then run the script again
  echo Or manually add to PATH: %USERPROFILE%\.local\bin
  pause
  exit /b 1
)
echo [OK] Conan verified:
conan --version
echo.

REM ============================================
REM STEP 9: Create Conan profile
REM ============================================
echo [STEP 9] Creating Conan profile...
conan profile detect --force
if %errorlevel% neq 0 (
  echo ERROR: Failed to create Conan profile
  pause
  exit /b 1
)
echo [OK] Conan profile created
echo.

REM ============================================
REM STEP 10: (Removed)
REM Manual compiler detection removed. 
REM Relying on Conan profile from STEP 9.
REM ============================================
echo [STEP 10] Skipping manual compiler detection, using Conan profile.
echo.

REM ============================================
REM STEP 11: Install Conan dependencies
REM ============================================
echo [STEP 11] Installing project dependencies with Conan...
echo This may take several minutes on first run...
echo.

REM Using Conan default profile for compiler, but forcing Release and C++20
conan install . --output-folder=Build --build=missing ^
  --settings=build_type=Release ^
  --settings=compiler.cppstd=20 ^
  -c tools.system.package_manager:mode=install

if %errorlevel% neq 0 (
  echo ERROR: Conan dependency installation failed
  echo.
  echo Common solutions:
  echo 1. Check your internet connection
  echo 2. Manually check 'conan profile show default'
  echo 3. Check conanfile.txt or conanfile.py for errors
  echo.
  pause
  exit /b 1
)
echo [OK] Dependencies installed successfully
echo.

REM ============================================
REM STEP 12: Configure project with CMake
REM ============================================
echo [STEP 12] Configuring CMake...
cmake -B Build -S . ^
  -A x64 ^
  -T host=x64 ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ^
  -DCMAKE_TOOLCHAIN_FILE=Build\build\generators\conan_toolchain.cmake

if %errorlevel% neq 0 (
  echo ERROR: CMake configuration failed
  echo.
  echo Please check:
  echo 1. CMakeLists.txt exists in project root
  echo 2. Conan toolchain was generated correctly
  echo 3. Visual Studio Build Tools are properly installed
  echo.
  pause
  exit /b 1
)
echo [OK] CMake configured successfully
echo.

REM ============================================
REM STEP 13: Build the project
REM ============================================
echo [STEP 13] Building project with %BUILD_JOBS% parallel jobs...
echo This may take a few minutes...
echo.

echo Setting CL environment variable for reduced memory usage with ASIO...
rem Key flags for ASIO header-only library (notorious for memory issues):
rem /Zm200 = Moderate heap increase (200% of default)
rem /bigobj = Allows more sections in object files (CRITICAL for ASIO)
rem /d2ReducedOptimizeHugeFunctions = Reduces optimization memory for large functions
rem /constexpr:steps2000000 = Increases constexpr evaluation limit for complex templates
rem /D_WIN32_WINNT=0x0601 = Target Windows 7+ to reduce API warnings
rem /favor:blend = Balanced optimization (uses less memory)
rem ASIO_DISABLE_HANDLER_TYPE_REQUIREMENTS = Reduces ASIO template instantiation
rem ASIO_NO_TYPEID = Disables RTTI in ASIO (saves memory)
set CL=/Zm200 /bigobj /d2ReducedOptimizeHugeFunctions /constexpr:steps2000000 /D_WIN32_WINNT=0x0601 /favor:blend /DASIO_DISABLE_HANDLER_TYPE_REQUIREMENTS /DASIO_NO_TYPEID

echo Building with single-threaded compilation to prevent memory exhaustion...
rem Critical: /m:1 ensures only ONE file compiles at a time
rem /maxcpucount:1 limits MSBuild processes to 1
rem This is ESSENTIAL for ASIO compilation on systems with limited RAM
cmake --build Build --config Release -- /m:1 /maxcpucount:1

if %errorlevel% neq 0 (
  echo ERROR: Build failed
  echo.
  echo Check the error messages above for details.
  echo Common issues:
  echo 1. Missing source files
  echo 2. Compilation errors in code
  echo 3. Linker errors
  echo.
  pause
  exit /b 1
)

echo.
echo ============================================
echo     BUILD COMPLETED SUCCESSFULLY!
echo ============================================
echo.

REM ============================================
REM Build Summary
REM ============================================
echo Build Summary:
echo --------------
echo   Compiler: (From Conan default profile)
echo   Build Type: Release
echo   C++ Standard: 20
echo   Parallel Jobs: %BUILD_JOBS%
echo   Output Directory: Build\
echo.

echo Available Executables:
if exist Build\bin (
    for %%f in (Build\bin\*.exe) do (
      echo   [*] %%~nxf
  )
) else (
    if exist Build\Release\bin (
        for %%f in (Build\Release\bin\*.exe) do (
            echo   [*] %%~nxf
        )
    ) else (
        echo   No executables found
    )
)
echo.

echo Available Libraries:
set LIB_FOUND=0
if exist Build\lib (
  for %%f in (Build\lib\*.lib Build\lib\*.dll) do (
      echo   [*] %%~nxf
      set LIB_FOUND=1
  )
)
if exist Build\libs (
    for %%f in (Build\libs\*.lib Build\libs\*.dll) do (
        echo   [*] %%~nxf
        set LIB_FOUND=1
    )
)
if %LIB_FOUND%==0 echo   No libraries found
echo.

echo Quick Start Commands:
echo ---------------------
echo   Run tests:  cd Build ^&^& bin\TKDEngineTests.exe
echo   Run client: cd Build ^&^& bin\TKDEngineClient.exe
echo   Run server: cd Build ^&^& bin\TKDEngineServer.exe
echo.
echo ============================================
echo.

if %VERBOSE%==1 (
  echo Verbose Information:
  echo -------------------
  python --version
  cmake --version | findstr /C:"cmake version"
  conan --version
  echo.
)

echo Build script completed successfully!
echo Press any key to exit...
pause >nul
