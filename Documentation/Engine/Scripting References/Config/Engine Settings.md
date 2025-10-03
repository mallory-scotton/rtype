# FEngineSettings

## Overview

The `FEngineSettings` struct is a comprehensive configuration container for the TKD Engine. It encapsulates all engine-wide settings, including window properties, network configuration, logging options, game metadata, audio settings, input mappings, and accessibility features. This struct serves as the central hub for customizing engine behavior and game parameters.

All members are public and can be directly accessed and modified. The struct is defined in the `tkd` namespace.

## Members

### version

**Type:** `std::string`  
**Default Value:** `TKD_VERSION_STRING`  
**Description:** Represents the current version of the TKD Engine. This is automatically set to the engine's version string and should not be modified by users.

**Example:**
```cpp
tkd::FEngineSettings settings;
std::cout << "Engine Version: " << settings.version << std::endl;
```

### window

**Type:** `struct Window`  
**Description:** Contains all window-related configuration options.

#### Members of Window:

- `enableVSync` (bool, default: true): Enables or disables vertical synchronization to prevent screen tearing.
- `targetFPS` (int, default: -1): Target frames per second. Set to -1 for unlimited FPS.
- `windowWidth` (int, default: 1280): Default window width in pixels.
- `windowHeight` (int, default: 720): Default window height in pixels.
- `isFullscreen` (bool, default: false): Determines if the application starts in fullscreen mode.
- `brightness` (float, default: 1.0f): Default brightness level (1.0 = normal).
- `showCursor` (bool, default: true): Controls cursor visibility.
- `resizable` (bool, default: true): Allows window resizing.
- `borderless` (bool, default: false): Enables borderless window mode.

**Example:**
```cpp
tkd::FEngineSettings settings;

// Configure window for 1920x1080 fullscreen with V-Sync disabled
settings.window.enableVSync = false;
settings.window.windowWidth = 1920;
settings.window.windowHeight = 1080;
settings.window.isFullscreen = true;
settings.window.brightness = 0.8f;
```

### network

**Type:** `struct Network`  
**Description:** Contains network-related configuration options for multiplayer functionality.

#### Members of Network:

- `enabled` (bool, default: true): Enables or disables networking features.
- `maxClients` (int, default: -1): Maximum number of clients allowed. Set to -1 for unlimited.
- `port` (int, default: 8080): Default network port for connections.
- `timeout` (int, default: 5000): Network timeout in milliseconds.
- `maxPacketSize` (int, default: 1452): Maximum UDP packet size in bytes.
- `protocolVersion` (int, default: 1): Network protocol version for compatibility.

**Example:**
```cpp
tkd::FEngineSettings settings;

// Configure for a multiplayer game server
settings.network.enabled = true;
settings.network.maxClients = 64;
settings.network.port = 7777;
settings.network.timeout = 10000;
settings.network.maxPacketSize = 1452;
```

### logging

**Type:** `struct Logging`  
**Description:** Controls logging behavior and output destinations.

#### Members of Logging:

- `enabled` (bool, default: true): Enables or disables all logging.
- `enableConsoleLogging` (bool, default: true): Enables logging to console output.
- `enableFileLogging` (bool, default: false): Enables logging to file.
- `maxLogFileSizeMB` (int, default: 5): Maximum log file size in megabytes.
- `logRetentionDays` (int, default: 7): Number of days to retain log files.

**Example:**
```cpp
tkd::FEngineSettings settings;

// Enable file logging with custom retention
settings.logging.enabled = true;
settings.logging.enableConsoleLogging = true;
settings.logging.enableFileLogging = true;
settings.logging.maxLogFileSizeMB = 10;
settings.logging.logRetentionDays = 30;
```

### game

**Type:** `struct Game`  
**Description:** Contains game metadata and information.

#### Members of Game:

- `title` (std::string, default: ""): The title of the game.
- `description` (std::string, default: ""): A description of the game.
- `version` (std::string, default: ""): The version of the game.
- `author` (std::string, default: ""): The author or developer of the game.
- `license` (std::string, default: ""): License information for the game.

**Example:**
```cpp
tkd::FEngineSettings settings;

// Set game metadata
settings.game.title = "My Awesome Game";
settings.game.description = "An exciting adventure game";
settings.game.version = "1.0.0";
settings.game.author = "Game Developer";
settings.game.license = "MIT License";
```

### audio

**Type:** `struct Audio`  
**Description:** Configures audio settings including volume levels and 3D audio.

#### Members of Audio:

- `enabled` (bool, default: true): Enables or disables audio system.
- `masterVolume` (int, default: 100): Master volume level (0-100).
- `musicVolume` (int, default: 80): Music volume level (0-100).
- `sfxVolume` (int, default: 80): Sound effects volume level (0-100).
- `enable3DAudio` (bool, default: true): Enables 3D positional audio.

**Example:**
```cpp
tkd::FEngineSettings settings;

// Configure audio settings
settings.audio.enabled = true;
settings.audio.masterVolume = 90;
settings.audio.musicVolume = 70;
settings.audio.sfxVolume = 85;
settings.audio.enable3DAudio = true;
```

### inputs

**Type:** `struct Inputs`  
**Description:** Manages input device settings and key mappings.

#### Members of Inputs:

- `enableGamepad` (bool, default: true): Enables gamepad input support.
- `mouseSensitivity` (float, default: 1.0f): Mouse sensitivity multiplier.
- `invertYAxis` (bool, default: false): Inverts Y-axis for mouse and gamepad.
- `inputActions` (std::unordered_map<std::string, std::vector<EInput>>, default: {}): Maps action names to input keys/buttons.
- `inputAxes` (std::unordered_map<std::string, std::vector<std::pair<EInput, float>>>, default: {}): Maps axis names to input axes with scales.

**Example:**
```cpp
tkd::FEngineSettings settings;

// Configure input settings
settings.inputs.enableGamepad = true;
settings.inputs.mouseSensitivity = 1.5f;
settings.inputs.invertYAxis = false;

// Map jump action to space key and gamepad A button
settings.inputs.inputActions["Jump"] = {tkd::EInput::Keyboard_Space, tkd::EInput::GamePad_A};

// Map movement axes
settings.inputs.inputAxes["MoveX"] = {{tkd::EInput::Keyboard_D, 1.0f}, {tkd::EInput::Keyboard_A, -1.0f}};
settings.inputs.inputAxes["MoveY"] = {{tkd::EInput::Keyboard_W, 1.0f}, {tkd::EInput::Keyboard_S, -1.0f}};
```

### accessibility

**Type:** `struct Accessibility`  
**Description:** Provides accessibility options for improved user experience.

#### Members of Accessibility:

- `highContrastMode` (bool, default: false): Enables high contrast visual mode.
- `colorblindMode` (int, default: 0): Colorblind mode setting (0 = disabled).
- `uiScale` (float, default: 1.0f): UI scaling factor for readability.
- `subtitles` (bool, default: true): Enables subtitle display.
- `subtitleSize` (int, default: 24): Font size for subtitles.

**Example:**
```cpp
tkd::FEngineSettings settings;

// Configure accessibility options
settings.accessibility.highContrastMode = true;
settings.accessibility.colorblindMode = 1; // Deuteranopia mode
settings.accessibility.uiScale = 1.2f;
settings.accessibility.subtitles = true;
settings.accessibility.subtitleSize = 28;
```

## Usage Example

```cpp
#include <Engine/Config/FEngineSettings.hpp>

int main() {
    // Create default settings
    tkd::FEngineSettings settings;
    
    // Customize for a specific game
    settings.game.title = "Space Adventure";
    settings.game.version = "2.1.0";
    
    settings.window.windowWidth = 1600;
    settings.window.windowHeight = 900;
    settings.window.isFullscreen = false;
    
    settings.network.port = 9001;
    settings.network.maxClients = 8;
    
    // Apply settings to engine...
    
    return 0;
}
```

## Notes

- All settings are initialized with sensible defaults.
- The `version` member should not be modified as it reflects the engine version.
- Input mappings use the `EInput` enumeration defined in `EInputs.hpp`.
- Changes to settings may require engine restart for full effect.
