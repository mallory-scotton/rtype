#!/bin/bash

# Cross-platform build script for Unix-like systems (Linux, macOS)
# This script installs dependencies and builds the project using Conan and CMake

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Enhanced error handling
trap 'echo -e "${RED}Error: Build failed at line $LINENO${NC}"; exit 1' ERR

# Function to print colored messages
print_status() {
    echo -e "${BLUE}=== $1 ===${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_status "Cross-Platform Build Script"
echo "Detected OS: $(uname -s)"
echo "Detected Architecture: $(uname -m)"

# Parse command line arguments
INSTALLER=""
SKIP_PACKAGE_INSTALLATION=false
CLEAN_BUILD=false
PRINT_HELP=false
while [[ $# -gt 0 ]]; do
    case $1 in
        --installer=*)
            INSTALLER="${1#*=}"
            shift
            ;;
        --skip-package-installation)
            SKIP_PACKAGE_INSTALLATION=true
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --help)
            PRINT_HELP=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

if [ "$PRINT_HELP" = true ]; then
    echo "Usage: ./Scripts/Build.sh [options]"
    echo ""
    echo "Options:"
    echo "  --installer=<installer>          Specify package manager (zypper, dnf, apt, yum, pacman, brew)"
    echo "  --skip-package-installation      Skip installation of system packages"
    echo "  --clean                          Clean existing Build directory before building"
    echo "  --help                           Show this help message"
    exit 0
fi

# Copy Pre-Commit Hook to .git/hooks
if [ -d ".git" ]; then
    echo "Setting up pre-commit hook..."
    cp .github/hooks/pre-commit .git/hooks/pre-commit
    chmod +x .git/hooks/pre-commit
else
    echo "Warning: .git directory not found. Skipping pre-commit hook setup."
fi

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to install packages on different systems
install_packages() {
    if [[ -n "$INSTALLER" ]]; then
        echo "Installing packages using $INSTALLER..."
        case $INSTALLER in
            zypper)
                sudo zypper refresh
                sudo zypper install -y python3-pip cmake gcc-c++ make pkg-config
                python3 -m pip install --user pipx
                ;;
            dnf)
                sudo dnf install -y python3-pip cmake gcc-c++ make pkg-config
                python3 -m pip install --user pipx
                ;;
            apt)
                sudo apt update
                sudo apt install -y pipx python3-pip cmake build-essential pkg-config
                ;;
            yum)
                sudo yum install -y python3-pip cmake gcc-c++ make pkg-config
                python3 -m pip install --user pipx
                ;;
            pacman)
                sudo pacman -S --noconfirm python-pipx cmake base-devel
                ;;
            brew)
                if [[ "$OSTYPE" == "darwin"* ]]; then
                    brew install pipx cmake pkg-config
                else
                    echo "brew is not supported on this OS"
                    exit 1
                fi
                ;;
            *)
                echo "Unsupported installer: $INSTALLER"
                exit 1
                ;;
        esac
    else
        if [[ "$OSTYPE" == "linux-gnu"* ]]; then
            echo "Installing packages for Linux..."
            if command_exists zypper; then
                sudo zypper refresh
                sudo zypper install -y python3-pip cmake gcc-c++ make pkg-config
                python3 -m pip install --user pipx
            elif command_exists dnf; then
                sudo dnf install -y python3-pip cmake gcc-c++ make pkg-config
                python3 -m pip install --user pipx
            elif command_exists apt; then
                sudo apt update
                sudo apt install -y pipx python3-pip cmake build-essential pkg-config
            elif command_exists yum; then
                sudo yum install -y python3-pip cmake gcc-c++ make pkg-config
                python3 -m pip install --user pipx
            elif command_exists pacman; then
                sudo pacman -S --noconfirm python-pipx cmake base-devel pkg-config
            else
                echo "Unsupported Linux distribution. Please install pipx and cmake manually."
                exit 1
            fi
        elif [[ "$OSTYPE" == "darwin"* ]]; then
            echo "Installing packages for macOS..."
            if command_exists brew; then
                brew install pipx cmake pkg-config pkg-config
            else
                echo "Homebrew not found. Installing Homebrew first..."
                /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
                brew install pipx cmake pkg-config pkg-config
            fi
        else
            echo "Unsupported operating system: $OSTYPE"
            exit 1
        fi
    fi
}

# Function to ensure pipx is in PATH
ensure_pipx_path() {
    if ! command_exists pipx; then
        echo "Adding pipx to PATH..."
        if [[ "$OSTYPE" == "linux-gnu"* ]]; then
            export PATH="$HOME/.local/bin:$PATH"
            echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
        elif [[ "$OSTYPE" == "darwin"* ]]; then
            export PATH="$HOME/.local/bin:$PATH"
            echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.zshrc
        fi

        # Reload the shell configuration
        if command_exists pipx; then
            pipx ensurepath
        else
            echo "Failed to add pipx to PATH. Please restart your terminal or run: export PATH=\"\$HOME/.local/bin:\$PATH\""
            exit 1
        fi
    fi
}

# Verify required tools
verify_requirements() {
    print_status "Verifying requirements"

    if ! command_exists cmake; then
        print_error "CMake is required but not found"
        return 1
    fi

    if ! command_exists gcc && ! command_exists clang; then
        print_error "A C++ compiler (gcc or clang) is required"
        return 1
    fi

    if ! command_exists pipx; then
        print_error "pipx is required but not found"
        return 1
    fi

    if ! command_exists conan; then
        print_error "Conan is required but not found"
        return 1
    fi

    print_success "All basic requirements met"
}

# Main execution
print_status "Step 1: Creating Build directory"
if [ "$CLEAN_BUILD" = true ] && [ -d "Build" ]; then
    rm -rf Build
    print_warning "Cleaned existing Build directory"
fi
mkdir -p Build
print_success "Build directory created"

print_status "Step 2: Installing system dependencies"
if [ "$SKIP_PACKAGE_INSTALLATION" = false ]; then
    install_packages
else
    print_warning "Skipping package installation as per user request."
fi
print_success "System dependencies installed"

print_status "Step 3: Ensuring pipx is available"
ensure_pipx_path
print_success "pipx is available"

print_status "Step 4: Installing Conan via pipx"
if ! command_exists conan; then
    pipx install conan
    print_success "Conan installed successfully"
else
    print_success "Conan is already installed"
fi

if command_exists gcc-13.4; then
    export CC=/usr/bin/gcc-13.4
    export CXX=/usr/bin/g++-13.4
fi

verify_requirements
CONAN=$(which conan)
print_status "Step 5: Creating Conan default profile"
if [ -z "$CONAN" ]; then
    print_error "Conan installation not found"
    exit 1
fi
if [ ! -f "/root/.conan2/profiles/default" ]; then
    print_warning "Conan default profile not found. Writing..."
    $CONAN profile detect --force
fi
print_success "Conan profile created"

# Detect compiler and version automatically
if command_exists gcc; then
    COMPILER="gcc"
    COMPILER_VERSION=$(gcc -dumpversion | cut -d. -f1)
elif command_exists clang; then
    COMPILER="clang"
    COMPILER_VERSION=$(clang --version | head -n1 | sed 's/.*version \([0-9]\+\).*/\1/')
else
    print_error "No supported compiler found"
    exit 1
fi

print_success "Detected compiler: $COMPILER version $COMPILER_VERSION"

print_status "Step 6: Installing dependencies with Conan"
if command_exists gcc-13.4; then
    COMPILER_VERSION=13
fi
$CONAN install . --output-folder=Build --build=missing --profile:build=default --profile:host=default \
    --settings=build_type=Release \
    --settings=compiler.cppstd=20 \
    --settings=compiler=$COMPILER \
    --settings=compiler.version=$COMPILER_VERSION \
    -c tools.system.package_manager:mode=install
print_success "Conan dependencies installed"

print_status "Step 7: Configuring CMake with Conan integration"
cmake -B Build -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_TOOLCHAIN_FILE=Build/build/Release/generators/conan_toolchain.cmake
print_success "CMake configured"

print_status "Step 8: Building the project"
NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
cmake --build Build --config Release -j$NPROC
print_success "Project built successfully"

# Update VS Code configuration with current Conan paths
if [ -d ".vscode" ]; then
    print_status "Updating VS Code configuration"

    # Find current Conan include paths
    CONAN_PATHS=$(find ~/.conan2/p/b -name include -type d 2>/dev/null | grep "/p/include" | sort -u | head -10)

    if [ -n "$CONAN_PATHS" ]; then
        print_success "Updated IDE configuration with current Conan paths"
    fi
fi

print_success "Build completed successfully!"
echo ""
print_status "Build Summary"
echo "Your built project is available in the Build directory."
echo ""

# Show available binaries
if [ -d "Build/bin" ]; then
    echo "Available executables:"
    find Build/bin -type f -executable 2>/dev/null | while read -r file; do
        if file "$file" | grep -q "executable"; then
            echo "  • $(basename "$file")"
        fi
    done
fi

# Show available libraries
if [ -d "Build/libs" ] || [ -d "Build/lib" ]; then
    echo ""
    echo "Available libraries:"
    find Build/libs Build/lib -name "*.so" -o -name "*.a" -o -name "*.dylib" 2>/dev/null | while read -r file; do
        echo "  • $(basename "$file")"
    done
fi

echo ""
print_success "To run tests: cd Build && ./bin/TKDEngineTests"
print_success "To run client: cd Build && ./bin/TKDEngineClient"
print_success "To run server: cd Build && ./bin/TKDEngineServer"
