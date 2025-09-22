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

## Development Tools

### Code Formatting with clang-format-19

This project uses clang-format-19 for consistent code formatting. Here's how to install and set it up:

#### Installing clang-format-19

**Ubuntu/Debian:**
```bash
# Add LLVM repository
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
sudo add-apt-repository "deb http://apt.llvm.org/$(lsb_release -cs)/ llvm-toolchain-$(lsb_release -cs)-19 main"
sudo apt update
sudo apt install clang-format-19
```

**macOS:**
```bash
brew install clang-format
# or specifically for version 19
brew install llvm@19
```

**Windows:**
Download from the [LLVM releases page](https://releases.llvm.org/download.html) or install via chocolatey:
```powershell
choco install llvm
```

#### Manual Formatting

Format a single file:
```bash
clang-format-19 -i path/to/your/file.cpp
```

Format all C++ files in the project:
```bash
find . -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" | xargs clang-format-19 -i
```

### VS Code Auto-Format Setup

To enable automatic formatting in VS Code:

1. **Install the C/C++ Extension:**
   - Open VS Code
   - Go to Extensions (Ctrl+Shift+X)
   - Search for "C/C++" by Microsoft and install it

2. **Configure VS Code Settings:**
   Add the following to your VS Code settings (`.vscode/settings.json` in the project root):
   ```json
   {
     "C_Cpp.clang_format_path": "clang-format-19",
     "C_Cpp.clang_format_style": "file",
     "editor.formatOnSave": true,
     "[cpp]": {
       "editor.defaultFormatter": "ms-vscode.cpptools"
     },
     "[c]": {
       "editor.defaultFormatter": "ms-vscode.cpptools"
     }
   }
   ```

3. **Alternative: Using the clang-format Extension:**
   - Install the "Clang-Format" extension by xaver
   - Configure it to use clang-format-19:
   ```json
   {
     "clang-format.executable": "clang-format-19",
     "clang-format.style": "file",
     "editor.formatOnSave": true
   }
   ```

#### Format on Save

With the above configuration, your code will automatically format when you save files. You can also manually format by:
- **Format Document:** Ctrl+Shift+I (Linux/Windows) or Cmd+Shift+I (macOS)
- **Format Selection:** Ctrl+K Ctrl+F (Linux/Windows) or Cmd+K Cmd+F (macOS)
