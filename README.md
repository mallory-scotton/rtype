<div align="center">

# 🚀 R-Type Game Engine Project

[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](VERSION)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE.md)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C.svg)](https://en.cppreference.com/w/cpp/20)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)](#-requirements)
[![Build](https://img.shields.io/badge/build-CMake%203.21%2B-red.svg)](#-installation-guide)

**A modern, enterprise-grade game engine and complete multiplayer gaming framework built with C++20**

[Features](#-features) • [Gallery](#-gallery) • [Installation](#-installation-guide) • [Documentation](#-documentation) • [Games](#-games)

---

</div>

## 📋 Table of Contents

- [📖 Description](#-description)
- [✨ Features](#-features)
- [🎨 Gallery](#-gallery)
- [💻 Requirements](#-requirements)
- [📦 Dependencies](#-dependencies)
- [📚 Documentation](#-documentation)
- [👥 Contributors](#-contributors)
- [🛠️ Installation Guide](#️-installation-guide)
- [🎮 Games](#-games)
  - [R-Type](#r-type)
  - [BeatSaber](#beatsaber)
- [📄 License](#-license)
- [❤️ Coded with Love](#️-coded-with-love)

---

## 📖 Description

The **R-Type Project** is a comprehensive game development ecosystem centered around the **TKD Engine (TekyoDrift Engine)**, a production-ready, modular game engine designed specifically for networked multiplayer applications. This project demonstrates enterprise-grade software architecture principles through its implementation of complete game prototypes and development tools.

Built entirely in modern **C++20**, the project encompasses:

- **🎯 TKD Engine**: A dual-mode (client/server) game engine with robust networking capabilities
- **🎮 Complete Games**: Two fully functional game implementations showcasing engine capabilities
- **🔧 Development Tools**: Specialized utilities for asset management, resource packing, and debugging
- **🌐 Network Architecture**: High-performance client-server multiplayer system with real-time synchronization
- **🏗️ Modular Design**: Reusable components following SOLID principles and modern C++ best practices

The engine leverages cutting-edge technologies including **Entity-Component-System (ECS)** architecture, **Asio-based networking**, **SFML graphics**, and experimental **OpenVR integration** for virtual reality experiences. Designed with cross-platform compatibility, performance optimization, and developer productivity in mind, the TKD Engine provides a solid foundation for building complex, networked multiplayer games.

---

## ✨ Features

### 🎮 Core Engine Systems

- **Modern C++20 Architecture**: Leveraging latest language features for type safety, performance, and maintainability
- **Dual-Build System**: Single codebase producing both client (with graphics) and server (headless) executables
- **Entity-Component-System (ECS)**: Flexible, composition-based game object management with runtime behavior modification
- **Thread-Safe Components**: Built-in concurrency primitives and synchronization for parallel execution
- **Memory Management**: RAII principles, smart pointers, and deterministic resource management
- **Cross-Platform Support**: Consistent behavior across Linux, Windows, and macOS

### 🌐 Advanced Networking

- **High-Performance UDP Protocol**: Custom binary protocol optimized for minimal bandwidth and latency
- **Asio-Powered I/O**: Asynchronous, non-blocking network operations for maximum throughput
- **Reliable Transmission Layer**: ACK/NACK system with automatic retransmission and duplicate detection
- **Client-Server Architecture**: Dedicated server support with multiple concurrent client connections
- **Real-Time Synchronization**: Delta compression and snapshot interpolation for smooth gameplay
- **Network Statistics**: Built-in monitoring for latency, packet loss, and bandwidth utilization
- **Type-Safe Packet System**: Automatic serialization/deserialization with virtual dispatch

### 🎨 Graphics & Rendering

- **SFML-Based Pipeline**: Modern OpenGL backend with shader support
- **Component-Based Rendering**: Modular rendering system with material and texture management
- **Camera System**: Multi-viewport support with 2D/3D transformations
- **Sprite & Animation Management**: FlipBook system for 2D sprite animations
- **Shader System**: Custom shader support for advanced visual effects
- **ImGui Integration**: Immediate-mode GUI for debug tools and in-game interfaces

### 🔊 Audio System

- **3D Positional Audio**: Spatial sound processing with listener management
- **Multi-Format Support**: WAV, OGG, FLAC audio playback
- **Audio Streaming**: Efficient streaming for large audio files
- **Effect Processing**: Real-time audio effects and filters
- **Sound Source Management**: Multiple simultaneous audio sources with priority system

### 🧮 Mathematics & Physics

- **Comprehensive Math Library**: Vectors (2D/3D/4D), matrices, quaternions, transforms
- **Geometric Primitives**: Rectangles, circles, spheres, planes, boxes, capsules, rays
- **Collision Detection**: AABB, OBB, sphere, and ray-based collision systems
- **Interpolation Functions**: Linear, spherical, cubic, and custom interpolation utilities
- **Color Management**: 8-bit and floating-point color spaces with conversion utilities

### 🔧 Development Tools Suite

- **TKDPack**: Asset packaging utility for bundling resources into optimized `.pak` archives with compression
- **TKDEmbed**: Binary-to-C++ header converter for compile-time asset embedding
- **TKDGraph**: Visual debugging and profiling tools with ImGui-based interface
- **Testing Framework**: Comprehensive unit tests using Catch2
- **Logging System**: Multi-level logging (Debug, Info, Warning, Error, Critical) with file output

### 🎯 Runtime Features

- **Actor-Component Model**: Flexible game object composition with inheritance and polymorphism
- **World Management**: Scene organization, level loading, and object lifecycle management
- **Time Management**: Delta time calculation, fixed timestep support, and time scaling
- **Input System**: Unified keyboard, mouse, and gamepad input handling with action mapping
- **Event System**: Type-safe event broadcasting and subscription
- **Configuration Management**: INI-based configuration with runtime modification
- **Save/Load System**: Binary and JSON serialization for game state persistence

### 🥽 Virtual Reality Support (Experimental)

- **OpenVR Integration**: Steam VR support for HTC Vive, Valve Index, and compatible headsets
- **VR Camera System**: Head-mounted display tracking and stereoscopic rendering
- **Motion Controllers**: Hand tracking and controller input
- **Room-Scale Experiences**: Chaperone system integration for safe VR gameplay

### 📦 Asset Management

- **Resource Handles**: Smart pointer-based asset management with automatic lifetime tracking
- **Pak File System**: Custom archive format for efficient asset distribution
- **Hot Reloading**: Runtime asset reloading for rapid iteration during development
- **Texture Atlas Support**: Sprite sheet management with automatic sub-texture extraction
- **Async Loading**: Background asset loading to prevent frame drops

---

## 🎨 Gallery

<div align="center">

### OpenVR Integration Tests

|  |  |
|:-------------------------:|:-------------------------:|
| ![OpenVR Test 1](Screenshots/OpenVRTest-01.mp4) | ![OpenVR Test 2](Screenshots/OpenVRTest-02.mp4) |
| *VR Camera Tracking & Rendering* | *Motion Controller Integration* |

> 🎥 **Note**: Video demonstrations showcasing experimental VR features with the TKD Engine

</div>

---

## 💻 Requirements

### Minimum System Requirements

| Component | Specification |
|-----------|--------------|
| **Operating System** | Windows 10/11, Ubuntu 20.04+, macOS 11+ |
| **Processor** | Dual-core 2.0 GHz |
| **Memory** | 4 GB RAM |
| **Graphics** | OpenGL 3.3 compatible GPU |
| **Storage** | 500 MB available space |
| **Network** | Broadband internet connection (for multiplayer) |

### Recommended System Requirements

| Component | Specification |
|-----------|--------------|
| **Operating System** | Windows 11, Ubuntu 22.04+, macOS 12+ |
| **Processor** | Quad-core 3.0 GHz or higher |
| **Memory** | 8 GB RAM or more |
| **Graphics** | Dedicated GPU with OpenGL 4.5 support |
| **Storage** | 1 GB available space (SSD recommended) |
| **Network** | Low-latency broadband connection |

### Development Requirements

- **CMake** 3.21 or higher
- **Python** 3.7+ with pip
- **C++ Compiler** with C++20 support:
  - **Linux**: GCC 10+ or Clang 12+
  - **Windows**: Visual Studio 2019+ or MinGW-w64
  - **macOS**: Xcode 12+ or Clang 12+
- **Git** for version control
- **Conan** 2.x package manager

---

## 📦 Dependencies

The R-Type project utilizes the following third-party libraries, automatically managed through Conan:

### Core Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| **[SFML](https://www.sfml-dev.org/)** | 2.6.2 | Graphics, windowing, audio, and input handling |
| **[Asio](https://think-async.com/Asio/)** | 1.36.0 | Asynchronous networking and I/O operations |
| **[ImGui](https://github.com/ocornut/imgui)** | 1.91.8 | Immediate-mode graphical user interface |
| **[ImGui-SFML](https://github.com/SFML/imgui-sfml)** | 2.6.1 | SFML rendering backend for ImGui |
| **[nlohmann_json](https://github.com/nlohmann/json)** | 3.12.0 | Modern JSON serialization and parsing |
| **[Catch2](https://github.com/catchorg/Catch2)** | 3.10.0 | Modern C++ testing framework |

### Graphics & Rendering

| Library | Version | Purpose |
|---------|---------|---------|
| **[OpenGL](https://www.opengl.org/)** | system | Hardware-accelerated graphics rendering |
| **[GLEW](http://glew.sourceforge.net/)** | 2.2.0 | OpenGL extension loading library |
| **[Mesa GLU](https://www.mesa3d.org/)** | 9.0.3 | OpenGL utility library (Linux/macOS) |

### Virtual Reality

| Library | Version | Purpose |
|---------|---------|---------|
| **[OpenVR](https://github.com/ValveSoftware/openvr)** | 1.16.8 | Virtual reality headset integration |

### Utilities

| Library | Version | Purpose |
|---------|---------|---------|
| **[zlib](https://www.zlib.net/)** | 1.3.1 | Data compression for asset archives |

All dependencies are automatically downloaded, configured, and linked during the build process via Conan package manager. No manual installation required.

---

## 📚 Documentation

Comprehensive documentation is available in the `Documentation/` directory:

### 📖 General Documentation

- **[Project Overview](Documentation/01.%20Project%20Overview.md)** - Executive summary, architecture, and technical stack
- **[Getting Started](Documentation/02.%20Getting%20Started.md)** - Quick start guide and first steps
- **[Features](Documentation/03.%20Features.md)** - Detailed feature descriptions and technical capabilities
- **[Directory Layout](Documentation/04.%20Directory%20Layout.md)** - Project structure and organization
- **[Changelog](Documentation/05.%20Changelog.md)** - Version history and release notes
- **[Contributing](Documentation/06.%20Contributing.md)** - Contribution guidelines and coding standards
- **[License](Documentation/07.%20License.md)** - Licensing information

### 🚀 Engine Documentation

Located in `Documentation/Engine/`:

- **[Engine Overview](Documentation/Engine/01.%20Overview.md)** - TKD Engine architecture and design
- **[Building the Engine](Documentation/Engine/02.%20Building.md)** - Compilation instructions and configuration
- **[Usage Examples](Documentation/Engine/03.%20Examples.md)** - Code samples and tutorials
- **[Testing](Documentation/Engine/04.%20Tests.md)** - Unit testing and quality assurance

#### 📦 Module References

Detailed API documentation for each engine module in `Documentation/Engine/Modules/`:

- **Core** - Foundation systems, containers, mathematics, concurrency
- **Runtime** - Actor-component system, world management, game loop
- **Network** - Client-server architecture, packet system, protocols
- **Renderer** - Graphics pipeline, materials, shaders, cameras
- **Audio** - Sound management, 3D audio, streaming
- **Config** - Configuration system and engine settings
- **Static** - Engine initialization and bootstrapping
- **Debug** - Logging, profiling, and debugging utilities

#### 🎓 Scripting References

Complete API references in `Documentation/Engine/Scripting References/`:

- **Actors** - Base actor classes, cameras, pawns, HUD
- **Animations** - 2D animations, flipbooks, state machines
- **Assets** - Resource management, pak files, loading system
- **Audio** - Audio buffers, sources, effects, listeners
- **Controllers** - Player and AI controller systems
- **Math** - Vectors, matrices, quaternions, transforms, geometry
- **Network** - Networking classes, packets, binary serialization
- **Renderer** - Rendering components, materials, shaders
- **Widgets** - UI system and widget classes

### 🔧 Tools Documentation

Located in `Documentation/Tools/`:

- **[TKDPack](Documentation/Tools/Pack/)** - Asset packaging tool documentation
- **[TKDEmbed](Documentation/Tools/Embed/)** - Binary embedding utility guide
- **[TKDGraph](Documentation/Tools/Graph/)** - Visual debugging tool manual

---

## 👥 Contributors

This project is the result of collaborative effort from our talented development team:

<div align="center">

### 👑 Project Leadership

| [<img src="https://github.com/mallory-scotton.png" width="100" height="100" style="border-radius:50%"/>](https://github.com/mallory-scotton) |
|:--:|
| **[Mallory SCOTTON](https://github.com/mallory-scotton)** |
| *Project Leader · Lead Engine Developer* |

### 🛠️ Core Development Team

| [<img src="https://github.com/natan-fievet.png" width="100" height="100" style="border-radius:50%"/>](https://github.com/natan-fievet) | [<img src="https://github.com/Hugo-Cathelain.png" width="100" height="100" style="border-radius:50%"/>](https://github.com/Hugo-Cathelain) |
|:--:|:--:|
| **[Nathan FIEVET](https://github.com/natan-fievet)** | **[Hugo CATHELAIN](https://github.com/Hugo-Cathelain)** |
| *Team Coordinator · Engine Developer* | *Engine & Network Developer* |

### 🎮 Game Development Team

| [<img src="https://github.com/omegalpha28.png" width="100" height="100" style="border-radius:50%"/>](https://github.com/omegalpha28) | [<img src="https://github.com/bombabobo.png" width="100" height="100" style="border-radius:50%"/>](https://github.com/bombabobo) |
|:--:|:--:|
| **[Ossan MSOILI](https://github.com/omegalpha28)** | **[Raphael OSTIER](https://github.com/bombabobo)** |
| *Game Developer* | *Game Developer* |

---

💡 **Contributions Welcome!**  
See our [Contributing Guide](CONTRIBUTING.md) and [Code of Conduct](CODE_OF_CONDUCT.md)

</div>

---

## 🛠️ Installation Guide

### Quick Start (Automated Build)

The easiest way to build R-Type is using our automated build scripts that handle all dependencies and configuration:

#### Linux, FreeBSD, OpenBSD, NetBSD, macOS

```bash
chmod +x Scripts/Build.sh
./Scripts/Build.sh
```

#### Windows

```cmd
.\Scripts\Build.bat
```

These scripts automatically:
- ✅ Install system dependencies (pipx, cmake, build tools)
- ✅ Install and configure Conan package manager
- ✅ Set up the build environment
- ✅ Configure CMake with optimal settings
- ✅ Build the project with all games and tools

### Manual Installation

If you prefer manual control over the build process:

#### Step 1: Install Prerequisites

**Linux (Ubuntu/Debian):**
```bash
sudo apt update
sudo apt install build-essential cmake python3-pip pipx git
```

**Linux (CentOS/RHEL/Fedora):**
```bash
sudo yum groupinstall "Development Tools"
sudo yum install cmake python3-pip git
python3 -m pip install --user pipx
```

**macOS:**
```bash
# Install Homebrew if not present
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake pipx git
```

**Windows:**
- Install [Visual Studio 2019+](https://visualstudio.microsoft.com/) with "Desktop development with C++" workload
- Install [CMake](https://cmake.org/download/) and add to PATH
- Install [Python 3.7+](https://python.org) with pip
- Install [Git for Windows](https://git-scm.com/download/win)

#### Step 2: Install Conan Package Manager

**Linux/macOS:**
```bash
pipx install conan
pipx ensurepath
```

**Windows:**
```cmd
python -m pip install conan
```

#### Step 3: Configure Conan Profile

```bash
conan profile detect --force
```

#### Step 4: Clone Repository

```bash
git clone https://github.com/mallory-scotton/rtype.git
cd rtype
```

#### Step 5: Install Dependencies

```bash
conan install . \
  --output-folder=Build \
  --build=missing \
  --settings=build_type=Release \
  --settings=compiler.cppstd=20 \
  -c tools.system.package_manager:mode=install
```

#### Step 6: Configure CMake

```bash
cmake -B Build -S . -DCMAKE_BUILD_TYPE=Release
```

#### Step 7: Build the Project

```bash
cmake --build Build --config Release
```

#### Step 8: Run the Executables

After successful compilation, executables are located in `Build/bin/`:

```bash
# Run R-Type game server
./Build/bin/RTypeGameServer

# Run R-Type game client
./Build/bin/RTypeGameClient

# Run TKDPack tool
./Build/bin/TKDPack --help
```

### Build Output Structure

```
Build/
├── bin/                      # Executable files
│   ├── RTypeGameClient       # R-Type client executable
│   ├── RTypeGameServer       # R-Type server executable
│   ├── BeatSaberGameClient   # BeatSaber client executable
│   ├── BeatSaberGameServer   # BeatSaber server executable
│   ├── TKDEngineClient       # Engine client library test
│   ├── TKDEngineServer       # Engine server library test
│   ├── TKDEngineTests        # Engine unit tests
│   ├── TKDPack               # Asset packing tool
│   └── TKDEmbed              # Binary embedding tool
├── lib/                      # Shared libraries (.dll, .so, .dylib)
└── Assets/                   # Game assets (copied from Games/)
```

### Troubleshooting

<details>
<summary><b>❌ Conan profile detection fails</b></summary>

**Solution:**
```bash
conan profile detect --force
```

If this persists, manually create a profile:
```bash
conan profile path default
# Edit the file and ensure C++20 is set
```
</details>

<details>
<summary><b>❌ Missing C++20 support</b></summary>

**Solution:**
- Ensure your compiler supports C++20:
  - GCC 10+
  - Clang 12+
  - MSVC 2019 (19.29)+
- Update your compiler if necessary
- Verify CMake detects the correct compiler: `cmake --version` and check compiler output
</details>

<details>
<summary><b>❌ Permission errors with Conan</b></summary>

**Solution:**
- **Do NOT use `sudo` with Conan commands** (except for system package installation)
- Conan should always run as your regular user
- Ensure your user has write permissions to the Conan cache directory (`~/.conan2/`)
</details>

<details>
<summary><b>❌ Build dependencies missing</b></summary>

**Solution:**
- Run the automated build script which handles all dependencies
- On Linux, install development packages: `sudo apt install build-essential cmake`
- On macOS, ensure Xcode Command Line Tools are installed: `xcode-select --install`
- On Windows, verify Visual Studio C++ tools are installed
</details>

<details>
<summary><b>❌ SFML or other library linking errors</b></summary>

**Solution:**
- Clean the build directory: `rm -rf Build/`
- Re-run Conan install with `--build=missing` flag
- Ensure all dependencies are built for the same architecture (x64)
</details>

**Need Additional Help?**
- Check the [Documentation](Documentation/)
- Review build logs for specific error messages
- Open an issue on [GitHub](https://github.com/mallory-scotton/rtype/issues)

### Development Tools Setup

#### Code Formatting with clang-format

This project uses `clang-format-19` for consistent code style.

**Installation:**

```bash
# Ubuntu/Debian
sudo apt install clang-format-19

# macOS
brew install clang-format

# Windows (via Chocolatey)
choco install llvm
```

**Usage:**

```bash
# Format single file
clang-format-19 -i path/to/file.cpp

# Format all C++ files
find . -name "*.cpp" -o -name "*.hpp" | xargs clang-format-19 -i
```

**VS Code Integration:**

Add to `.vscode/settings.json`:
```json
{
  "C_Cpp.clang_format_path": "clang-format-19",
  "C_Cpp.clang_format_style": "file",
  "editor.formatOnSave": true,
  "[cpp]": {
    "editor.defaultFormatter": "ms-vscode.cpptools"
  }
}
```

---

## 🎮 Games

The R-Type project includes two complete game implementations showcasing the TKD Engine's capabilities:

### R-Type

<div align="left">

**Version:** 1.0.0  
**Genre:** Side-Scrolling Shooter  
**Mode:** Networked Multiplayer  
**Status:** ✅ Stable Release

</div>

#### Overview

A complete recreation of the classic R-Type arcade shooter featuring networked multiplayer gameplay. Demonstrates the engine's capabilities in 2D game development, real-time networking, and client-server architecture.

#### Features

- ✨ **Classic Gameplay**: Faithful recreation of R-Type mechanics
- 🌐 **Multiplayer Support**: Up to 4 simultaneous players via client-server architecture
- 🎮 **Real-Time Synchronization**: Smooth gameplay with delta compression and interpolation
- 🎨 **Sprite Animation System**: FlipBook-based 2D animations
- 💥 **Particle Effects**: Explosions, weapon effects, and environmental particles
- 🎵 **Audio Integration**: Music and sound effects with 3D positional audio
- 🏆 **Scoring System**: Player statistics and leaderboards
- 📦 **Pak File Assets**: Optimized asset loading using TKDPack format

#### Technical Highlights

- Entity-Component-System for game object management
- Custom network protocol with reliable UDP transmission
- State synchronization with snapshot interpolation
- Client-side prediction for responsive controls
- Server-authoritative gameplay preventing cheating

#### Running R-Type

```bash
# Start dedicated server
cd Build/bin
./RTypeGameServer --port 4242

# Start client and connect
./RTypeGameClient --server localhost --port 4242
```

**Configuration:**
- Server settings: `Build/bin/Data/server.ini`
- Client settings: `Build/bin/Data/client.ini`

---

### BeatSaber

<div align="left">

**Version:** 0.0.1  
**Genre:** VR Rhythm Game  
**Mode:** Single Player (Experimental)  
**Status:** 🚧 In Development

</div>

#### Overview

An experimental VR rhythm game prototype showcasing the TKD Engine's OpenVR integration capabilities. Features head-mounted display tracking, motion controller support, and 3D audio for immersive gameplay.

#### Features (Experimental)

- 🥽 **VR Support**: OpenVR integration for SteamVR-compatible headsets
- 🎯 **Motion Controllers**: Hand tracking and gesture recognition
- 🎵 **Rhythm Gameplay**: Music-synchronized obstacle generation
- 🎨 **3D Rendering**: Stereoscopic rendering for VR displays
- 🔊 **Spatial Audio**: 3D positional audio for immersive sound
- 🏠 **Room-Scale**: Chaperone system integration

#### Technical Highlights

- OpenVR SDK integration for HMD and controller tracking
- Custom VR camera system with stereoscopic projection
- Motion controller input mapping
- Performance optimization for 90Hz VR rendering
- Experimental physics-based sword mechanics

#### Running BeatSaber

**Requirements:**
- SteamVR-compatible VR headset (HTC Vive, Valve Index, Oculus Rift, etc.)
- SteamVR runtime installed
- Motion controllers

```bash
# Ensure SteamVR is running, then start the game
cd Build/bin
./BeatSaberGameClient
```

**⚠️ Note:** This is an experimental prototype demonstrating VR capabilities. Full gameplay features are still in development.

---

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE.md](LICENSE.md) file for complete details.

```
MIT License

Copyright (c) 2025 TekyoDrift

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

### Third-Party Licenses

This project uses several open-source libraries, each with their own licenses:
- **SFML** - zlib/libpng License
- **Asio** - Boost Software License 1.0
- **ImGui** - MIT License
- **Catch2** - Boost Software License 1.0
- **OpenVR** - BSD 3-Clause License
- **nlohmann_json** - MIT License

---

<div align="center">

## ❤️ Coded with Love

**Made with ❤️ by the TekyoDrift Team**

*Building the future of game development, one commit at a time.*

---

### 🌟 Star this project if you find it useful!

[![GitHub stars](https://img.shields.io/github/stars/mallory-scotton/rtype?style=social)](https://github.com/mallory-scotton/rtype/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/mallory-scotton/rtype?style=social)](https://github.com/mallory-scotton/rtype/network/members)
[![GitHub watchers](https://img.shields.io/github/watchers/mallory-scotton/rtype?style=social)](https://github.com/mallory-scotton/rtype/watchers)

---

**[⬆ Back to Top](#-r-type-game-engine-project)**

</div>
