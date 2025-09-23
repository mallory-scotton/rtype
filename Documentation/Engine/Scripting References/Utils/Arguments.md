# FArgs

## Overview

The `FArgs` class is a command-line argument parsing utility that provides a simple and type-safe way to handle program arguments. As a singleton class, it offers automatic help generation, validation, and conversion for various data types. The class supports both individual values and vectors, with built-in error handling and usage display.

## Basic Usage

### Setting Up Argument Parsing

```cpp
#include <Engine/Core/Utils/FArgs.hpp>

int main(int argc, char* argv[]) {
    // Get the singleton instance
    auto& args = tkd::FArgs::GetInstance();

    // Define variables to hold parsed values
    std::string configFile;
    int port = 8080;
    bool verbose = false;
    std::vector<std::string> modules;

    // Add command-line flags
    args.AddFlags("config", "Configuration file path", configFile, true);
    args.AddFlags("port", "Server port number", port, false);
    args.AddFlags("verbose", "Enable verbose output", verbose, false);
    args.AddVectorFlags("modules", "List of modules to load", modules, false);

    // Process command-line arguments
    if (!args.Process(argc, argv)) {
        return args.GetExitCode();  // Error occurred, exit with appropriate code
    }

    // Use the parsed values
    std::cout << "Config: " << configFile << std::endl;
    std::cout << "Port: " << port << std::endl;
    std::cout << "Verbose: " << (verbose ? "yes" : "no") << std::endl;

    return 0;
}
```

### Command-Line Usage

```bash
# Basic usage with required flag
./myprogram --config config.json

# With optional flags
./myprogram --config config.json --port 9000 --verbose

# With vector flag
./myprogram --config config.json --modules network,graphics,audio

# Get help
./myprogram --help
```

## Adding Flags

### Single Value Flags

#### `AddFlags(const std::string& flags, const std::string& description, T& reference, bool mandatory = true)`

Adds a command-line flag that accepts a single value.

**Parameters:**
- `flags`: Base name for the flag (generates `--flags` and `-f`)
- `description`: Help text for the flag
- `reference`: Variable to store the parsed value
- `mandatory`: Whether the flag is required (default: true)

**Supported Types:**
- `std::string`
- `int`
- `float`
- `double`
- `bool`
- Any arithmetic type

```cpp
std::string outputFile;
int maxConnections = 100;
float timeout = 30.0f;
bool debug = false;

// Required string flag
args.AddFlags("output", "Output file path", outputFile, true);

// Optional integer flag with default
args.AddFlags("connections", "Maximum connections", maxConnections, false);

// Optional float flag
args.AddFlags("timeout", "Connection timeout in seconds", timeout, false);

// Optional boolean flag
args.AddFlags("debug", "Enable debug mode", debug, false);
```

### Vector Flags

#### `AddVectorFlags(const std::string& flags, const std::string& description, std::vector<T>& reference, bool mandatory = false, const std::string& delimiter = ",")`

Adds a command-line flag that accepts multiple values as a delimited list.

**Parameters:**
- `flags`: Base name for the flag
- `description`: Help text for the flag
- `reference`: Vector to store the parsed values
- `mandatory`: Whether the flag is required (default: false)
- `delimiter`: Character to split values (default: ",")

```cpp
std::vector<std::string> includePaths;
std::vector<int> ports;
std::vector<float> weights;

// Comma-separated strings
args.AddVectorFlags("include", "Include paths", includePaths, false);

// Space-separated integers
args.AddVectorFlags("ports", "Server ports", ports, false, " ");

// Custom delimiter
args.AddVectorFlags("weights", "Layer weights", weights, false, ";");
```

## Processing Arguments

### `Process(int argc, char* argv[])`

Parses the command-line arguments according to the defined flags.

**Returns:** `true` if parsing succeeded, `false` if errors occurred

**Behavior:**
- Validates all arguments against defined flags
- Converts string values to appropriate types
- Checks for mandatory flags
- Displays help on `--help` or `-help`
- Shows usage and exits on errors

```cpp
if (!args.Process(argc, argv)) {
    // Parsing failed - error messages already displayed
    return args.GetExitCode();
}

// Parsing successful - proceed with program logic
```

### Error Handling

The class provides comprehensive error handling:

```cpp
// Unknown argument
./program --unknown-flag value
# Error: Unknown argument '--unknown-flag'

// Missing value
./program --config
# Error: Flag '--config' requires a value

// Invalid type conversion
./program --port not-a-number
# Error: Invalid value 'not-a-number' for flag '--port'

// Missing mandatory flag
./program --output result.txt  # (if port is mandatory)
# Error: Required flag '--port' not provided
```

## Help and Usage

### Automatic Help Generation

The class automatically generates help text when `--help` or `-help` is provided:

```
Usage: ./myprogram <config> [port] [verbose] [modules]

Options:
  -c, --config      Configuration file path (required)
  -p, --port        Server port number (default: 8080)
  -v, --verbose     Enable verbose output (default: false)
  -m, --modules     List of modules to load (default: [])

Example:
  ./myprogram --config config.json --port 9000 --verbose --modules network,graphics
```

### Custom Help Integration

```cpp
// Check for help before processing
if (argc > 1 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-help")) {
    args.PrintUsage(argv[0]);  // Manual help display
    return 0;
}
```

## Advanced Usage

### Type-Safe Parsing

The class uses template metaprogramming for type-safe conversions:

```cpp
// Automatic type deduction
int count;
float ratio;
std::string name;

args.AddFlags("count", "Item count", count);
args.AddFlags("ratio", "Aspect ratio", ratio);
args.AddFlags("name", "User name", name);

// Supports custom types via template specialization
struct Resolution {
    int width, height;
};

// Would require custom setter/getter functions for complex types
```

### Vector Parsing Details

```cpp
std::vector<std::string> files;

// Input: --files "file1.txt,file2.txt,file3.txt"
args.AddVectorFlags("files", "Input files", files);

// Result: files = ["file1.txt", "file2.txt", "file3.txt"]

// With spaces: --files "file1.txt file2.txt file3.txt" (delimiter=" ")
// With semicolons: --files "file1.txt;file2.txt;file3.txt" (delimiter=";")
```

### Integration with Engine

```cpp
// In Engine initialization
bool Engine::Initialize(int argc, char* argv[]) {
    auto& args = FArgs::GetInstance();

    // Engine configuration flags
    args.AddFlags("width", "Window width", m_windowWidth, false);
    args.AddFlags("height", "Window height", m_windowHeight, false);
    args.AddFlags("fullscreen", "Enable fullscreen", m_fullscreen, false);

    if (!args.Process(argc, argv)) {
        return false;
    }

    // Initialize with parsed values
    return InitializeGraphics(m_windowWidth, m_windowHeight, m_fullscreen);
}
```

## Best Practices

### Flag Naming

```cpp
// Good: Descriptive names
args.AddFlags("config", "Configuration file path", configFile);
args.AddFlags("timeout", "Connection timeout in seconds", timeout);

// Avoid: Single characters or abbreviations
args.AddFlags("c", "Configuration file", configFile);  // Unclear
args.AddFlags("t", "Timeout", timeout);                // Ambiguous
```

### Default Values

```cpp
// Good: Sensible defaults for optional flags
int port = 8080;
bool verbose = false;
std::string logLevel = "info";

args.AddFlags("port", "Server port", port, false);
args.AddFlags("verbose", "Verbose output", verbose, false);
args.AddFlags("log-level", "Logging level", logLevel, false);
```

### Error Handling

```cpp
// Good: Check return values and handle errors gracefully
auto& args = FArgs::GetInstance();

if (!args.Process(argc, argv)) {
    std::cerr << "Failed to parse command-line arguments" << std::endl;
    return args.GetExitCode();
}
```

### Documentation

```cpp
// Good: Clear descriptions
args.AddFlags("database", "Database connection string (format: user:pass@host:port/db)", dbString);
args.AddFlags("workers", "Number of worker threads (recommended: CPU cores)", workerCount, false);
```

## Limitations

### Type Restrictions

- Complex types require custom setter/getter functions
- No support for nested vectors or complex data structures
- Boolean flags are parsed as "true"/"false" or "1"/"0"

### Parsing Behavior

- All flags require values (no standalone flags like `--verbose`)
- Vector parsing uses simple string splitting
- No support for quoted strings with spaces in vector values

## Future Enhancements

### Planned Features

- **Standalone Flags**: Support for `--verbose` without values
- **Short Options**: Combined short options like `-xvzf`
- **Configuration Files**: Load arguments from config files
- **Environment Variables**: Fallback to environment variables
- **Subcommands**: Support for git-style subcommands
- **Validation**: Custom validation functions for parsed values
