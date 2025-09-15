#!/bin/bash

# Cross-platform build script for Unix-like systems (Linux, macOS)
# This script installs dependencies and builds the project using Conan and CMake

set -e  # Exit on any error

echo "=== Cross-Platform Build Script ==="
echo "Detected OS: $(uname -s)"

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
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "Installing packages for Linux..."
        if command_exists apt; then
            sudo apt update
            sudo apt install -y pipx python3-pip cmake build-essential
        elif command_exists yum; then
            sudo yum install -y python3-pip cmake gcc-c++ make
            python3 -m pip install --user pipx
        elif command_exists pacman; then
            sudo pacman -S --noconfirm python-pipx cmake base-devel
        else
            echo "Unsupported Linux distribution. Please install pipx and cmake manually."
            exit 1
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "Installing packages for macOS..."
        if command_exists brew; then
            brew install pipx cmake
        else
            echo "Homebrew not found. Installing Homebrew first..."
            /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
            brew install pipx cmake
        fi
    else
        echo "Unsupported operating system: $OSTYPE"
        exit 1
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

# Main execution
echo "Step 1: Creating Build directory..."
mkdir -p Build

echo "Step 2: Installing system dependencies..."
install_packages

echo "Step 3: Ensuring pipx is available..."
ensure_pipx_path

echo "Step 4: Installing Conan via pipx..."
if ! command_exists conan; then
    pipx install conan
    echo "Conan installed successfully"
else
    echo "Conan is already installed"
fi

echo "Step 5: Creating Conan default profile..."

conan profile detect --force

echo "Step 6: Installing dependencies with Conan..."
conan install . --output-folder=Build --build=missing --settings=build_type=Release --settings=compiler.cppstd=20 -c tools.system.package_manager:mode=install

echo "Step 7: Configuring CMake..."
cmake -B Build -S . -DCMAKE_BUILD_TYPE=Release

echo "Step 8: Building the project..."
cmake --build Build --config Release

echo "=== Build completed successfully! ==="
echo "Your built project should be available in the Build directory."
