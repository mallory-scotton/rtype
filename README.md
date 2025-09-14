# 🤖 R-Type

## Installation

### Quick Start (Automated)

The easiest way to build R-Type is using our automated build scripts that handle all dependencies and configuration:

```bash
# Linux, FreeBSD, OpenBSD, NetBSD, macOS
chmod +x Scripts/Build.sh
./Scripts/Build.sh

# Windows
./Scripts/Build.bat
```

These scripts will automatically:
- Install system dependencies (pipx, cmake, build tools)
- Install and configure Conan
- Set up the build environment
- Configure and build the project

### Manual Installation

If you prefer to build manually or need more control over the process:

#### Prerequisites

- **CMake** (3.15 or higher)
- **Python** (3.7 or higher) with pip
- **C++ Compiler** with C++20 support
  - Linux: GCC 10+ or Clang 12+
  - Windows: Visual Studio 2019+ or MinGW-w64
  - macOS: Xcode 12+ or Clang 12+

#### Step-by-Step Build

```bash
# 1. Create build directory
mkdir Build

# 2. Install Conan package manager
# Linux/macOS:
sudo apt install pipx -y  # or equivalent package manager
pipx ensurepath
pipx install conan

# Windows: Use Python pip
python -m pip install conan

# 3. Create Conan default profile
conan profile detect --force

# 4. Install project dependencies
conan install . --output-folder=Build --build=missing --settings=build_type=Release --settings=compiler.cppstd=20 -c tools.system.package_manager:mode=install

# 5. Configure CMake
cmake -B Build -S . -DCMAKE_BUILD_TYPE=Release

# 6. Build the project
cmake --build Build --config Release
```

#### Platform-Specific Notes

**Linux (Ubuntu/Debian):**
```bash
sudo apt update
sudo apt install build-essential cmake python3-pip pipx
```

**Linux (CentOS/RHEL):**
```bash
sudo yum groupinstall "Development Tools"
sudo yum install cmake python3-pip
python3 -m pip install --user pipx
```

**macOS:**
```bash
# Install Homebrew if not present
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
brew install cmake pipx
```

**Windows:**
- Install [Visual Studio 2019+](https://visualstudio.microsoft.com/) with C++ development tools
- Install [CMake](https://cmake.org/download/) and add to PATH
- Install [Python](https://python.org) with pip

### Build Output

After successful compilation, you'll find the executables in:
- `Build/bin/` - Main executables
- `Build/lib/` - Libraries (if any)

### Troubleshooting

**Common Issues:**

1. **Conan profile detection fails:**
   ```bash
   conan profile detect --force
   ```

2. **Missing C++20 support:**
   - Ensure you're using a compatible compiler version
   - Check CMake is detecting the correct compiler

3. **Permission errors with Conan:**
   - Don't use `sudo` with Conan commands (except for system package installation)
   - Conan should run as your regular user

4. **Build dependencies missing:**
   - Run the automated script which handles all dependencies
   - Manually install missing system packages for your platform

**Need Help?**
- Check the build logs for specific error messages
- Ensure all prerequisites are properly installed
- Try the automated build scripts for a cleaner setup
