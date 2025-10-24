# Logger System Documentation

## Overview

The TKD Engine `FLogger` class provides a comprehensive, thread-safe logging system designed for game development and application debugging. It supports multiple output destinations, configurable log levels, color-coded console output, file logging with automatic rotation, and advanced features like namespacing and timestamping.

### Key Features

- **Multiple Log Levels**: 8 log levels from Fatal to Trace for granular control
- **Dual Output**: Simultaneous console and file logging
- **Color-Coded Output**: Customizable ANSI color palette for console output
- **Thread-Safe**: Mutex-protected operations for multi-threaded applications
- **File Rotation**: Automatic log file rotation based on size limits
- **Namespace Support**: Categorize logs by subsystem or component
- **Timestamp Integration**: Optional timestamp prefixes for all log entries
- **Template Formatting**: Modern C++20 `std::format` integration
- **Performance Optimized**: Minimal overhead with level filtering

### Architecture

The logger system follows a static class design pattern with global configuration:

```
┌─────────────────┐
│   FLogger       │ ← Static utility class
│                 │
│ • Log levels    │
│ • Output config │
│ • Color palette │
│ • File rotation │
│ • Thread safety │
└─────────────────┘
         │
    ┌────▼────┐
    │ Console │ ← Color-coded output
    │ Output  │
    └─────────┘
         │
    ┌────▼────┐
    │  File   │ ← Rotation & archiving
    │ Output  │
    └─────────┘
```

## Core Components

### ELogLevel Enumeration

Defines the available log levels with increasing verbosity:

```cpp
enum class ELogLevel : UInt8
{
    Off,     // Disable all logging
    Fatal,   // Critical errors that cause program termination
    Error,   // Runtime errors that don't stop execution
    Warn,    // Warning conditions that should be addressed
    Info,    // Informational messages about program flow
    Debug,   // Detailed debugging information
    Trace,   // Very detailed execution tracing
    All      // Enable all log levels (same as Trace)
};
```

#### Log Level Descriptions

- **`Off`**: Completely disables logging. No output is generated.
- **`Fatal`**: Critical errors that cause immediate program termination. Used for unrecoverable errors.
- **`Error`**: Runtime errors that don't stop execution but indicate serious problems.
- **`Warn`**: Warning conditions that should be reviewed but don't prevent operation.
- **`Info`**: General informational messages about program flow and important events.
- **`Debug`**: Detailed debugging information useful during development.
- **`Trace`**: Very detailed execution tracing, typically used for low-level debugging.
- **`All`**: Enables all log levels (equivalent to Trace).

### FLoggerPalette Structure

Defines color codes for different log levels in console output:

```cpp
struct FLoggerPalette
{
    FString fatal = "\033[1;41;97m";   // Bold white text on red background
    FString error = "\033[1;31m";      // Bold red text
    FString warn = "\033[1;33m";       // Bold yellow text
    FString info = "\033[1;32m";       // Bold green text
    FString debug = "\033[1;36m";      // Bold cyan text
    FString trace = "\033[0;37m";      // Dim white text
    FString all = "\033[0m";           // Reset to default colors
};
```

### FLogger Class Declaration

```cpp
namespace tkd
{
class FLogger
{
private:
    // Static configuration members
    static Bool s_consoleLogging;
    static Bool s_fileLogging;
    static ELogLevel s_logLevel;
    static FilePath s_logDirectoryPath;
    static FilePath s_logFilePath;
    static Bool s_usePalette;
    static FLoggerPalette s_palette;
    static std::mutex s_mutex;
    static Bool s_useTimestamp;
    static std::ofstream s_logFileStream;
    static UInt64 s_maxLogFileSizeBytes;
    static Bool s_useNamespace;
    static FString s_currentNamespace;

public:
    // Configuration methods
    static void SetLogLevel(ELogLevel level);
    static void SetConsoleLogging(Bool enabled);
    static void SetFileLogging(Bool enabled);
    static void SetLogDirectoryPath(const FilePath& filePath);
    static void SetColorPalette(FLoggerPalette palette);
    static void SetPaletteUsage(Bool enabled);
    static void SetTimestampUsage(Bool enabled);
    static void SetMaxLogFileSize(UInt32 sizeMB);
    static void SetNamespaceUsage(Bool enabled);
    static void SetNamespace(const FString& namespaceName);
    static void ClearNamespace(void);

    // Lifecycle methods
    static Bool Initialize(void);
    static void Shutdown(void);

    // Logging methods
    template <typename... Args>
    static void Log(ELogLevel level, std::string_view format, Args&&... args);

    template <typename... Args>
    static void LogWithNamespace(ELogLevel level, const FString& namespaceName,
                                std::string_view format, Args&&... args);

    // Convenience methods
    template <typename... Args> static void Fatal(std::string_view format, Args&&... args);
    template <typename... Args> static void Error(std::string_view format, Args&&... args);
    template <typename... Args> static void Warn(std::string_view format, Args&&... args);
    template <typename... Args> static void Info(std::string_view format, Args&&... args);
    template <typename... Args> static void Debug(std::string_view format, Args&&... args);
    template <typename... Args> static void Trace(std::string_view format, Args&&... args);
};
}
```

## Configuration Methods

### Basic Configuration

#### `SetLogLevel(ELogLevel level)`
Sets the minimum log level that will be output.

**Parameters:**
- `level`: Minimum log level to display

**Example:**
```cpp
// Show only warnings and above
FLogger::SetLogLevel(ELogLevel::Warn);

// Show all messages including debug and trace
FLogger::SetLogLevel(ELogLevel::All);
```

#### `SetConsoleLogging(Bool enabled)`
Enables or disables console output.

**Parameters:**
- `enabled`: True to enable console logging, false to disable

**Example:**
```cpp
// Enable console output
FLogger::SetConsoleLogging(true);

// Disable console output (file logging only)
FLogger::SetConsoleLogging(false);
```

#### `SetFileLogging(Bool enabled)`
Enables or disables file output with automatic log file management.

**Parameters:**
- `enabled`: True to enable file logging, false to disable

**Example:**
```cpp
// Enable file logging
FLogger::SetFileLogging(true);

// Disable file logging
FLogger::SetFileLogging(false);
```

### Advanced Configuration

#### `SetLogDirectoryPath(const FilePath& filePath)`
Sets the directory where log files will be stored.

**Parameters:**
- `filePath`: Directory path for log files

**Example:**
```cpp
// Set custom log directory
FLogger::SetLogDirectoryPath("C:/MyGame/Logs");

// Use relative path
FLogger::SetLogDirectoryPath("./logs");
```

#### `SetColorPalette(FLoggerPalette palette)`
Customizes the color scheme for console output.

**Parameters:**
- `palette`: Custom color palette structure

**Example:**
```cpp
FLoggerPalette customPalette;
customPalette.fatal = "\033[1;45m";  // Magenta background
customPalette.error = "\033[1;35m";  // Magenta text
customPalette.warn = "\033[1;43m";   // Yellow background
customPalette.info = "\033[1;32m";   // Green text (default)

FLogger::SetColorPalette(customPalette);
```

#### `SetPaletteUsage(Bool enabled)`
Enables or disables color output in console.

**Parameters:**
- `enabled`: True to use colors, false for plain text

**Example:**
```cpp
// Enable colored output
FLogger::SetPaletteUsage(true);

// Disable colors (useful for log files or monochrome terminals)
FLogger::SetPaletteUsage(false);
```

#### `SetTimestampUsage(Bool enabled)`
Enables or disables timestamp prefixes.

**Parameters:**
- `enabled`: True to include timestamps, false to omit them

**Example:**
```cpp
// Include timestamps in all log messages
FLogger::SetTimestampUsage(true);

// Omit timestamps for cleaner output
FLogger::SetTimestampUsage(false);
```

#### `SetMaxLogFileSize(UInt32 sizeMB)`
Sets the maximum size for log files before rotation.

**Parameters:**
- `sizeMB`: Maximum file size in megabytes

**Example:**
```cpp
// Rotate log files when they reach 10MB
FLogger::SetMaxLogFileSize(10);

// Use smaller files for frequent rotation
FLogger::SetMaxLogFileSize(1);
```

### Namespace Configuration

#### `SetNamespaceUsage(Bool enabled)`
Enables or disables namespace prefixes in log messages.

**Parameters:**
- `enabled`: True to include namespaces, false to omit them

#### `SetNamespace(const FString& namespaceName)`
Sets a persistent namespace for all subsequent log messages.

**Parameters:**
- `namespaceName`: Namespace identifier (e.g., "Network", "Rendering")

**Example:**
```cpp
// Set namespace for network-related logs
FLogger::SetNamespace("Network");
FLogger::Info("Connected to server");  // Output: [Network] [INFO] Connected to server

// Clear namespace
FLogger::ClearNamespace();
```

#### `ClearNamespace(void)`
Removes the current namespace setting.

## Logging Methods

### Template-Based Logging

#### `Log(ELogLevel level, std::string_view format, Args&&... args)`
Logs a formatted message at the specified level.

**Parameters:**
- `level`: Log level for this message
- `format`: Format string using `std::format` syntax
- `args`: Variadic arguments for formatting

**Example:**
```cpp
FLogger::Log(ELogLevel::Info, "Player {} moved to position ({}, {})",
             playerId, x, y);

FLogger::Log(ELogLevel::Debug, "Processing {} entities at {} FPS",
             entityCount, framesPerSecond);
```

#### `LogWithNamespace(ELogLevel level, const FString& namespaceName, std::string_view format, Args&&... args)`
Logs a message with a temporary namespace override.

**Parameters:**
- `level`: Log level for this message
- `namespaceName`: Temporary namespace for this message
- `format`: Format string
- `args`: Formatting arguments

**Example:**
```cpp
FLogger::LogWithNamespace(ELogLevel::Error, "Physics",
                         "Collision detection failed for object {}",
                         objectId);
```

### Convenience Methods

#### `Fatal(std::string_view format, Args&&... args)`
Logs a fatal error message (highest priority).

**Example:**
```cpp
if (criticalResource == nullptr) {
    FLogger::Fatal("Critical resource failed to load: {}", resourcePath);
    std::abort();
}
```

#### `Error(std::string_view format, Args&&... args)`
Logs an error message.

**Example:**
```cpp
if (!texture.loadFromFile(filePath)) {
    FLogger::Error("Failed to load texture: {}", filePath);
    return false;
}
```

#### `Warn(std::string_view format, Args&&... args)`
Logs a warning message.

**Example:**
```cpp
if (frameTime > targetFrameTime) {
    FLogger::Warn("Frame time exceeded target: {}ms (target: {}ms)",
                  frameTime, targetFrameTime);
}
```

#### `Info(std::string_view format, Args&&... args)`
Logs an informational message.

**Example:**
```cpp
FLogger::Info("Game initialized successfully");
FLogger::Info("Loaded {} levels, {} textures, {} sounds",
              levelCount, textureCount, soundCount);
```

#### `Debug(std::string_view format, Args&&... args)`
Logs a debug message.

**Example:**
```cpp
FLogger::Debug("Entity {} updated: position=({}, {}, {}), health={}",
               entityId, x, y, z, health);
```

#### `Trace(std::string_view format, Args&&... args)`
Logs a trace message (most verbose).

**Example:**
```cpp
FLogger::Trace("Entering function CalculatePath with parameters: start=({}, {}), end=({}, {})",
               startX, startY, endX, endY);
```

## Lifecycle Management

### `Initialize(void)`
Initializes the logger system and opens log files if file logging is enabled.

**Returns:** True if initialization successful, false otherwise

**Example:**
```cpp
// Initialize logger at application startup
if (!FLogger::Initialize()) {
    std::cerr << "Failed to initialize logger!" << std::endl;
    return EXIT_FAILURE;
}
```

### `Shutdown(void)`
Shuts down the logger system and closes all open resources.

**Example:**
```cpp
// Clean shutdown at application exit
FLogger::Shutdown();
```

## Usage Examples

### Basic Setup and Usage

```cpp
#include <Engine/Core/Utils/FLogger.hpp>

// Initialize logger
FLogger::SetLogLevel(ELogLevel::Info);
FLogger::SetConsoleLogging(true);
FLogger::SetFileLogging(true);
FLogger::SetTimestampUsage(true);
FLogger::Initialize();

// Basic logging
FLogger::Info("Application started");
FLogger::Debug("Debug information: value = {}", someValue);

// Error handling
try {
    riskyOperation();
} catch (const std::exception& e) {
    FLogger::Error("Operation failed: {}", e.what());
}

// Cleanup
FLogger::Shutdown();
```

### Advanced Configuration

```cpp
// Configure for development
FLogger::SetLogLevel(ELogLevel::Debug);
FLogger::SetConsoleLogging(true);
FLogger::SetFileLogging(true);
FLogger::SetLogDirectoryPath("./dev_logs");
FLogger::SetMaxLogFileSize(5);  // 5MB files

// Custom color scheme
FLoggerPalette devPalette;
devPalette.debug = "\033[1;34m";  // Bright blue for debug
devPalette.trace = "\033[0;90m";  // Dark gray for trace
FLogger::SetColorPalette(devPalette);

// Enable all features
FLogger::SetTimestampUsage(true);
FLogger::SetPaletteUsage(true);
```

### Namespace Usage

```cpp
// Set global namespace
FLogger::SetNamespace("GameEngine");
FLogger::Info("Engine initialized");

// Temporary namespace override
FLogger::LogWithNamespace(ELogLevel::Debug, "Physics",
                         "Collision detected between {} and {}",
                         objectA, objectB);

// Subsystem logging
FLogger::SetNamespace("Network");
FLogger::Info("Server connection established");
FLogger::Debug("Received {} bytes from client", byteCount);

// Clear namespace
FLogger::ClearNamespace();
FLogger::Info("Cleanup completed");
```

### Performance Monitoring

```cpp
auto startTime = std::chrono::high_resolution_clock::now();

// Expensive operation
processLargeDataset();

auto endTime = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

FLogger::Debug("Dataset processing completed in {}ms", duration.count());
```

### Error Context Logging

```cpp
class ResourceManager {
public:
    bool loadResource(const std::string& path) {
        FLogger::SetNamespace("ResourceManager");

        try {
            FLogger::Debug("Loading resource: {}", path);

            // Loading logic...
            if (!fileExists(path)) {
                FLogger::Error("Resource file not found: {}", path);
                return false;
            }

            // Success
            FLogger::Info("Successfully loaded resource: {}", path);
            return true;

        } catch (const std::exception& e) {
            FLogger::Error("Failed to load resource {}: {}", path, e.what());
            return false;
        }
    }
};
```

## Output Formats

### Console Output Format

```
[2025-10-23 14:30:15] [Network] [INFO ] Connected to server at 192.168.1.100:8080
[2025-10-23 14:30:16] [Physics] [DEBUG] Collision detected: entity 42 vs entity 1337
[2025-10-23 14:30:17] [ERROR] Failed to load texture: missing_file.png
```

### File Output Format

```
========================================
TKD Engine Log File
Created: 2025-10-23_14-30-15
========================================
[2025-10-23 14:30:15] [Network] [INFO ] Connected to server at 192.168.1.100:8080
[2025-10-23 14:30:16] [Physics] [DEBUG] Collision detected: entity 42 vs entity 1337
[2025-10-23 14:30:17] [ERROR] Failed to load texture: missing_file.png
========================================
Log file closed
========================================
```

## Performance Considerations

### Level Filtering

The logger performs early level filtering to avoid unnecessary formatting:

```cpp
if (static_cast<UInt8>(level) > static_cast<UInt8>(s_logLevel)) {
    return;  // Early exit, no formatting performed
}
```

### Formatting Optimization

- **Lazy Formatting**: Messages are only formatted when they pass level filtering
- **String View**: Uses `std::string_view` for format strings to avoid copies
- **Forward Arguments**: Uses perfect forwarding to avoid unnecessary copies

### File I/O Optimization

- **Buffered Output**: Uses `std::ofstream` with internal buffering
- **Size-Based Rotation**: Prevents log files from growing too large
- **Append Mode**: Opens files in append mode to preserve existing logs

### Thread Safety

- **Mutex Protection**: All operations are protected by a static mutex
- **Minimal Lock Time**: Locks are held only during actual I/O operations
- **Atomic Configuration**: Configuration changes are atomic

## Best Practices

### Log Level Usage Guidelines

- **`Fatal`**: Use only for truly catastrophic failures that require immediate termination
- **`Error`**: Runtime errors that affect functionality but allow continued operation
- **`Warn`**: Conditions that should be addressed but don't prevent operation
- **`Info`**: Important state changes and milestones
- **`Debug`**: Detailed information useful during development
- **`Trace`**: Very verbose output, typically disabled in release builds

### Message Formatting

```cpp
// Good: Structured, searchable messages
FLogger::Info("Player {} connected from {}", playerId, ipAddress);

// Bad: Unstructured, hard to parse
FLogger::Info("Stuff happened with player and IP");
```

### Namespace Organization

```cpp
// Use consistent namespace naming
FLogger::SetNamespace("Network.TCP");      // Hierarchical
FLogger::SetNamespace("Audio.Engine");     // Component-based
FLogger::SetNamespace("Physics.Collision"); // Subsystem-based
```

### Configuration Patterns

```cpp
// Development configuration
void setupDevLogging() {
    FLogger::SetLogLevel(ELogLevel::Debug);
    FLogger::SetConsoleLogging(true);
    FLogger::SetFileLogging(true);
    FLogger::SetTimestampUsage(true);
    FLogger::SetPaletteUsage(true);
}

// Release configuration
void setupReleaseLogging() {
    FLogger::SetLogLevel(ELogLevel::Warn);
    FLogger::SetConsoleLogging(false);
    FLogger::SetFileLogging(true);
    FLogger::SetTimestampUsage(true);
    FLogger::SetPaletteUsage(false);
}
```

## Architecture Diagrams

### Logging Pipeline

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│ Application │───▶│  FLogger    │───▶│  Format &   │
│ Code        │    │  (Static)   │    │  Filter     │
└─────────────┘    └─────────────┘    └─────────────┘
                                                        │
                                               ┌────────▼────────┐
                                               │ Level Check     │
                                               │ & Early Exit    │
                                               └─────────────────┘
                                                        │
                                               ┌────────▼────────┐
                                               │ Format Message  │
                                               │ (std::format)   │
                                               └─────────────────┘
                                                        │
                                    ┌───────────────────▼───────────────────┐
                                    │                                       │
                           ┌────────▼────────┐                   ┌────────▼────────┐
                           │ Console Output  │                   │ File Output     │
                           │ (Color-coded)   │                   │ (Rotation)      │
                           └─────────────────┘                   └─────────────────┘
```

### Configuration Flow

```
┌─────────────────┐
│ Initialize()    │
│ Called Once     │
└─────────────────┘
         │
         ▼
┌─────────────────┐    ┌─────────────────┐
│ Check Config    │───▶│ File Logging?   │
│ Settings        │    │                 │
└─────────────────┘    └─────────────────┘
         │                       │
         │ No             ┌──────▼──────┐
         │                │ Create Dir  │
         │                │ & Open File │
         │                └─────────────┘
         │                       │
         ▼                       ▼
┌─────────────────┐    ┌─────────────────┐
│ Ready for       │    │ Ready for       │
│ Logging         │    │ Logging         │
└─────────────────┘    └─────────────────┘
```

### Thread Safety Model

```
┌─────────────────┐
│ Log Call        │
└─────────────────┘
         │
         ▼
┌─────────────────┐    ┌─────────────────┐
│ Acquire Mutex   │───▶│ Check Level     │
│ (Thread-safe)   │    │ Filter           │
└─────────────────┘    └─────────────────┘
         │
         ▼
┌─────────────────┐    ┌─────────────────┐
│ Format Message  │───▶│ Output to       │
│                 │    │ Console/File    │
└─────────────────┘    └─────────────────┘
         │
         ▼
┌─────────────────┐
│ Release Mutex   │
└─────────────────┘
```

## Error Handling

The logger system includes robust error handling:

- **File System Errors**: Graceful handling of directory creation failures
- **File I/O Errors**: Continues operation if log file cannot be written
- **Configuration Errors**: Validates parameters and provides fallbacks
- **Threading Errors**: Mutex operations are exception-safe

## Platform-Specific Notes

### Windows
- Uses Windows console color codes
- Supports UTF-8 encoding for international characters
- Integrates with Windows Event Log (future enhancement)

### Linux/Unix
- Uses ANSI escape sequences for colors
- Supports terminal color detection
- Compatible with systemd journal (future enhancement)

### macOS
- Uses ANSI escape sequences
- Compatible with Terminal.app and iTerm2
- Supports macOS unified logging (future enhancement)

## Future Extensions

The logger architecture supports future enhancements:

- **Structured Logging**: JSON output for log analysis tools
- **Remote Logging**: Network-based log aggregation
- **Log Compression**: Automatic compression of old log files
- **Performance Profiling**: Built-in timing and profiling integration
- **Conditional Logging**: Runtime condition-based filtering

---

*This documentation covers the complete TKD Engine FLogger system as of the current version. For the latest updates and additional examples, refer to the engine's source code and test suites.*
