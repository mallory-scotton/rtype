# 🚀 TKD Engine (TekyoDrift Engine)

[![Version](https://img.shields.io/badge/version-1.0.0--alpha-orange)](https://github.com/mallory-scotton/rtype)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE.md)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](#building)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey)](#platform-support)

> A modern, modular, and high-performance game engine designed for networked multiplayer games and real-time applications.

---

## 📋 Table of Contents

- [✨ Features](#-features)
- [🎯 Architecture Overview](#-architecture-overview)
- [🛠️ Building](#-building)
- [📖 Usage](#-usage)
- [📚 Documentation](#-documentation)
- [⚡ Dependencies](#-dependencies)
- [🏗️ Platform Support](#-platform-support)
- [👥 Authors](#-authors)
- [🙏 Special Thanks](#-special-thanks)
- [📄 License](#-license)

---

## ✨ Features

### 🎮 **Core Engine Systems**
- **Modern C++20** architecture with type safety and performance
- **Dual-Mode Architecture**: Client (with graphics) and Server (headless) variants
- **Thread-Safe Components** with built-in concurrency management
- **Memory Management** with smart pointers and RAII principles
- **Cross-Platform Support** for Windows, Linux, and macOS

### 🌐 **Networking & Multiplayer**
- **High-Performance Networking** powered by Asio
- **Binary Serialization** with automatic packet management
- **Client-Server Architecture** with dedicated server support
- **Network Statistics** and connection monitoring
- **Custom Packet System** with type-safe message handling

### 🎨 **Graphics & Rendering**
- **SFML-Based Rendering** with modern OpenGL backend
- **Component-Based Rendering** system
- **Camera Management** with viewport control
- **Texture & Material System** with resource management
- **Shader Support** for advanced visual effects

### 🔧 **Development Tools**
- **Comprehensive Testing** with Catch2 framework
- **Debug Utilities** and logging system
- **Configuration Management** with INI file support
- **Archive System** for save/load functionality
- **Math Library** with vectors, matrices, and geometry primitives

### ⚙️ **Runtime Features**
- **Actor-Component System** for modular game objects
- **World Management** with scene organization
- **Time Management** with delta time and tick systems
- **Event System** with type-safe callbacks
- **Input Handling** for keyboard, mouse, and gamepad

---

## 🎯 Architecture Overview

The TKD Engine follows a modular architecture organized into several key subsystems:

```
TKD Engine
├── 🧱 Core          - Foundation systems (memory, containers, math)
├── ⏱️ Runtime       - Game loop, actors, components, world management
├── 🌐 Network       - Client-server communication and packet handling
├── 🎨 Renderer      - Graphics, materials, shaders, and rendering pipeline
├── 🔊 Audio         - Sound management and 3D audio processing
├── ⚙️ Config        - Configuration, constants, and build settings
└── 🏗️ Static        - Engine initialization and game integration
```

### **Dual Library System**
- **TKDEngineClient**: Full engine with graphics support for game clients
- **TKDEngineServer**: Lightweight engine for dedicated servers (headless)

---

## 🛠️ Building

### **Prerequisites**
- **CMake** 3.21 or higher
- **C++ Compiler** with C++20 support:
  - Linux: GCC 10+ or Clang 12+
  - Windows: Visual Studio 2019+ or MinGW-w64
  - macOS: Xcode 12+ or Clang 12+
- **Python** 3.7+ with pip (for Conan)

### **Quick Build**

```bash
# Clone the repository
git clone https://github.com/mallory-scotton/rtype.git
cd rtype/Engine

# Install Conan (if not already installed)
pip install conan

# Configure Conan profile
conan profile detect --force

# Install dependencies
mkdir -p Build/build/Release
cd Build/build/Release
conan install ../../../ --output-folder=. --build=missing -s build_type=Release

# Build the engine
cd ../../..
cmake --preset conan-release
cmake --build --preset conan-release

# Run tests (optional)
cd Build
ctest -C Release
```

### **Build Targets**

```bash
# Build client library (with graphics)
cmake --build --preset conan-release --target TKDEngineClient

# Build server library (headless)
cmake --build --preset conan-release --target TKDEngineServer

# Build and run tests
cmake --build --preset conan-release --target TKDEngineTests
./bin/TKDEngineTests

# Build with debugging symbols
cmake --preset conan-debug
cmake --build --preset conan-debug
```

### **Integration with Games**

To use the engine in your game project:

```cmake
# In your CMakeLists.txt
find_package(TKDEngine REQUIRED)

# Link against the appropriate engine variant
target_link_libraries(YourGame
    PRIVATE
        TKDEngineClient  # For client builds
        # OR
        TKDEngineServer  # For server builds
)
```

---

## 📖 Usage

### **Creating a Basic Game**

```cpp
#include <Engine/Engine.hpp>

class MyGame : public tkd::IGame
{
public:
    bool Initialize() override {
        // Initialize your game systems
        return true;
    }
    
    void Update(float deltaTime) override {
        // Update game logic
    }
    
    void Render() override {
        // Render your game (client only)
    }
    
    void Shutdown() override {
        // Cleanup resources
    }
};

// Export your game
TKD_EXPORT_GAME(
    MyGame,           // Game class
    "1.0.0",         // Engine version
    "My Awesome Game", // Game name
    "1.0.0",         // Game version
    "A cool game built with TKD Engine" // Description
);
```

### **Network Communication**

```cpp
// Server side
auto server = std::make_unique<tkd::FNetworkServer>();
server->Start(8080);

// Client side
auto client = std::make_unique<tkd::FNetworkBase>();
client->Connect("127.0.0.1", 8080);

// Send custom packet
auto packet = std::make_unique<MyCustomPacket>();
client->SendPacket(std::move(packet));
```

### **Component System**

```cpp
// Create an actor
auto actor = World::CreateActor();

// Add components
auto meshComponent = actor->AddComponent<MeshComponent>();
auto transformComponent = actor->AddComponent<TransformComponent>();

// Update component
transformComponent->SetPosition({100.0f, 50.0f, 0.0f});
```

---

## 📚 Documentation

### **Engine Documentation**
- [📖 Engine Overview](../Documentation/Engine/01.%20Overview.md)
- [🛠️ Building Guide](../Documentation/Engine/02.%20Building.md)
- [💡 Examples](../Documentation/Engine/03.%20Examples.md)
- [🧪 Testing](../Documentation/Engine/04.%20Tests.md)

### **Module References**
- [🧱 Core Systems](../Documentation/Engine/Modules/01.%20Core.md) - Memory, containers, math utilities
- [⏱️ Runtime](../Documentation/Engine/Modules/02.%20Runtime.md) - Game loop, actors, components
- [🌐 Network](../Documentation/Engine/Modules/03.%20Network.md) - Client-server communication
- [🎨 Renderer](../Documentation/Engine/Modules/04.%20Renderer.md) - Graphics and rendering pipeline
- [🔊 Audio](../Documentation/Engine/Modules/05.%20Audio.md) - Sound and music systems
- [⚙️ Config](../Documentation/Engine/Modules/06.%20Config.md) - Configuration management
- [🏗️ Static](../Documentation/Engine/Modules/07.%20Static.md) - Engine initialization

### **API Reference**
Complete API documentation is generated from source code comments. Build the documentation with:
```bash
# Generate API docs (if Doxygen is installed)
doxygen Doxyfile
```

---

## ⚡ Dependencies

### **Core Dependencies**
| Library | Version | Purpose | License |
|---------|---------|---------|---------|
| **SFML** | 2.6.2+ | Graphics, Audio, Windowing | zlib/png |
| **Asio** | 1.36.0+ | Networking | Boost License |
| **Catch2** | 3.10.0+ | Unit Testing | Boost License |
| **ImGui** | 1.91.8+ | Debug UI (optional) | MIT |
| **zlib** | 1.3.1+ | Compression | zlib License |

### **Build Dependencies**
- **CMake** 3.21+ - Build system
- **Conan** 2.0+ - Package manager
- **Python** 3.7+ - For build tools

### **System Dependencies**
```bash
# Ubuntu/Debian
sudo apt install build-essential cmake python3 python3-pip

# Fedora/RHEL
sudo dnf install gcc-c++ cmake python3 python3-pip

# macOS
brew install cmake python

# Windows
# Install Visual Studio 2019+ with C++ workload
# Install Python from python.org
```

---

## 🏗️ Platform Support

| Platform | Architecture | Status | Notes |
|----------|-------------|---------|--------|
| **Linux** | x86_64 | ✅ Full Support | Primary development platform |
| **Linux** | ARM64 | ✅ Full Support | Tested on Raspberry Pi |
| **Windows** | x86_64 | ✅ Full Support | Visual Studio 2019+ |
| **Windows** | ARM64 | ⚠️ Experimental | Limited testing |
| **macOS** | x86_64 | ✅ Full Support | macOS 10.15+ |
| **macOS** | ARM64 (M1/M2) | ✅ Full Support | Native ARM support |
| **FreeBSD** | x86_64 | ⚠️ Community | Community maintained |

---

## 👥 Authors

### 👑 **Project Leadership & Core Development**
**[Mallory SCOTTON](https://github.com/mallory-scotton)** - *Project Leader & Lead Engine Developer*

### 🛠 **Core Development Team**
- **[Nathan FIEVET](https://github.com/natan-fievet)** - *Team Coordinator & Engine Developer*
- **[Hugo CATHELAIN](https://github.com/Hugo-Cathelain)** - *Engine & Network Developer*

### 🎮 **Game Development Team**
- **[Ossan MSOILI](https://github.com/omegalpha28)** - *Game Developer*
- **[Raphael OSTIER](https://github.com/bombabobo)** - *Game Developer*

*Part of the **TekyoDrift** development team*

---

## 🙏 Special Thanks

### **🏢 Organizations**
- **[Epitech](https://www.epitech.eu/)** - For providing the educational framework and project opportunity
- **[SFML Community](https://www.sfml-dev.org/)** - For the excellent multimedia library
- **[Asio Community](https://think-async.com/Asio/)** - For the robust networking foundation

### **📚 Inspirations**
- **Unreal Engine** - For architectural patterns and component system design
- **Unity Engine** - For modular system organization
- **Godot Engine** - For open-source game engine best practices

### **🔧 Tools & Libraries**
- **[Conan](https://conan.io/)** - For dependency management
- **[Catch2](https://github.com/catchorg/Catch2)** - For comprehensive testing framework
- **[CMake](https://cmake.org/)** - For cross-platform build system

### **🌟 Community**
- All contributors who have helped improve the engine
- Beta testers and early adopters
- The broader game development community for inspiration and feedback

---

## 📄 License

This project is licensed under the **MIT License** - see the [LICENSE.md](LICENSE.md) file for complete details.

```
Copyright (c) 2024 TekyoDrift Team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
```

---

<div align="center">

**🚀 Built with ❤️ by the TekyoDrift Team**

[🌐 Website](https://github.com/mallory-scotton/rtype) • [📚 Documentation](../Documentation/) • [🐛 Issues](https://github.com/mallory-scotton/rtype/issues) • [💬 Discussions](https://github.com/mallory-scotton/rtype/discussions)

*Making game development accessible, powerful, and fun!* 🎮

</div>
