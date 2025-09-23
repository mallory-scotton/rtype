# FileSystem

## Overview

The `FileSystem` class is a comprehensive static utility class that provides cross-platform file system operations and path manipulation. It serves as the TKD Engine's primary interface for interacting with the file system, offering a consistent API across Windows, Linux, and macOS platforms.

Built on top of C++17's `<filesystem>` library, the FileSystem class provides both high-level convenience methods and low-level control over file operations, directory management, and path handling.

## Key Features

- **Cross-platform compatibility** - Works identically on Windows, Linux, and macOS
- **Exception-safe operations** - Uses `std::error_code` for error handling
- **Path manipulation** - Rich set of path utilities and transformations
- **Directory traversal** - Both shallow and recursive directory listing
- **File I/O operations** - Read/write files as strings or binary data
- **Permission checking** - Verify read, write, and execute permissions
- **Special directories** - Access to home, temp, executable, and working directories

## Directory Operations

### Creating Directories

#### `static bool Mkdir(const FilePath& path)`

Creates a single directory. Parent directories must exist.

```cpp
#include <Engine/Core/Utils/FileSystem.hpp>

tkd::FileSystem::FilePath dirPath = "my_directory";
if (tkd::FileSystem::Mkdir(dirPath)) {
    std::cout << "Directory created" << std::endl;
}
```

#### `static bool Mkdirs(const FilePath& path)` / `CreateDirectories(const FilePath& path)`

Creates directory hierarchy recursively. All parent directories are created as needed.

```cpp
tkd::FileSystem::FilePath nestedPath = "parent/child/grandchild";
tkd::FileSystem::CreateDirectories(nestedPath);  // Creates all levels
```

### Directory Removal

#### `static bool RemoveDirectory(const FilePath& path)`

Removes a directory. Directory must be empty.

```cpp
tkd::FileSystem::FilePath dirPath = "empty_directory";
tkd::FileSystem::RemoveDirectory(dirPath);
```

## File Operations

### File Creation and Removal

#### `static bool CreateFile(const FilePath& path)`

Creates an empty file. Fails if file already exists.

```cpp
tkd::FileSystem::FilePath filePath = "new_file.txt";
tkd::FileSystem::CreateFile(filePath);
```

#### `static bool RemoveFile(const FilePath& path)`

Deletes a file from the filesystem.

```cpp
tkd::FileSystem::FilePath filePath = "file_to_delete.txt";
tkd::FileSystem::RemoveFile(filePath);
```

### File Copying and Moving

#### `static bool CopyFile(const FilePath& source, const FilePath& destination)`

Copies a file from source to destination. Overwrites existing files.

```cpp
tkd::FileSystem::FilePath src = "source.txt";
tkd::FileSystem::FilePath dst = "destination.txt";
tkd::FileSystem::CopyFile(src, dst);
```

#### `static bool MoveFile(const FilePath& source, const FilePath& destination)`

Moves/renames a file. More efficient than copy+delete.

```cpp
tkd::FileSystem::FilePath oldPath = "old_name.txt";
tkd::FileSystem::FilePath newPath = "new_name.txt";
tkd::FileSystem::MoveFile(oldPath, newPath);
```

#### `static bool CopyDirectory(const FilePath& source, const FilePath& destination, bool recursive = true)`

Copies entire directory structure.

```cpp
tkd::FileSystem::FilePath srcDir = "source_folder";
tkd::FileSystem::FilePath dstDir = "destination_folder";
tkd::FileSystem::CopyDirectory(srcDir, dstDir, true);  // Recursive copy
```

## Path Information and Manipulation

### Path Queries

#### `static bool IsDirectory(const FilePath& path)` / `IsFile(const FilePath& path)`

Check if path is directory or regular file.

```cpp
tkd::FileSystem::FilePath path = "some_path";

if (tkd::FileSystem::IsDirectory(path)) {
    std::cout << "It's a directory" << std::endl;
} else if (tkd::FileSystem::IsFile(path)) {
    std::cout << "It's a file" << std::endl;
}
```

#### `static bool IsReadable/IsWritable/IsExecutable(const FilePath& path)`

Check file permissions.

```cpp
tkd::FileSystem::FilePath file = "test.txt";

if (tkd::FileSystem::IsReadable(file) && tkd::FileSystem::IsWritable(file)) {
    std::cout << "File is readable and writable" << std::endl;
}
```

### Special Directory Paths

#### `static FilePath GetCurrentWorkingDirectory(void)`

Get the current working directory.

```cpp
auto cwd = tkd::FileSystem::GetCurrentWorkingDirectory();
std::cout << "Current directory: " << cwd << std::endl;
```

#### `static FilePath GetExecutablePath(void)` / `GetExecutableDirectory(void)`

Get path to current executable and its directory.

```cpp
auto exePath = tkd::FileSystem::GetExecutablePath();
auto exeDir = tkd::FileSystem::GetExecutableDirectory();
```

#### `static FilePath GetHomeDirectory(void)` / `GetTempDirectory(void)`

Get user home and system temp directories.

```cpp
auto home = tkd::FileSystem::GetHomeDirectory();
auto temp = tkd::FileSystem::GetTempDirectory();
```

### Path Manipulation

#### `static FilePath GetAbsolutePath(const FilePath& path)`

Convert relative path to absolute.

```cpp
tkd::FileSystem::FilePath relative = "relative/path";
auto absolute = tkd::FileSystem::GetAbsolutePath(relative);
```

#### `static FilePath GetParentPath(const FilePath& path)` / `GetFileName(const FilePath& path)`

Extract path components.

```cpp
tkd::FileSystem::FilePath fullPath = "/home/user/file.txt";

auto parent = tkd::FileSystem::GetParentPath(fullPath);     // "/home/user"
auto filename = tkd::FileSystem::GetFileName(fullPath);     // "file.txt"
auto extension = tkd::FileSystem::GetFileExtension(fullPath); // ".txt"
```

## Directory Listing

### Basic Listing

#### `static std::vector<FilePath> ListFiles(const FilePath& path)`

List all regular files in a directory.

```cpp
tkd::FileSystem::FilePath dir = "some_directory";
auto files = tkd::FileSystem::ListFiles(dir);

for (const auto& file : files) {
    std::cout << "File: " << file << std::endl;
}
```

#### `static std::vector<FilePath> ListDirectories(const FilePath& path)`

List all subdirectories.

```cpp
auto dirs = tkd::FileSystem::ListDirectories(dir);
for (const auto& subdir : dirs) {
    std::cout << "Directory: " << subdir << std::endl;
}
```

### Recursive Listing

#### `static std::vector<FilePath> RecursiveListFiles(const FilePath& path)`

Recursively list all files in directory tree.

```cpp
auto allFiles = tkd::FileSystem::RecursiveListFiles("project_root");
std::cout << "Found " << allFiles.size() << " files" << std::endl;
```

### Filtered Listing

#### `static std::vector<FilePath> ListFilesByExtension(const FilePath& path, const FilePath& extension)`

List files with specific extension.

```cpp
auto cppFiles = tkd::FileSystem::ListFilesByExtension(".", ".cpp");
auto headerFiles = tkd::FileSystem::RecursiveListFilesByExtension("src", ".hpp");
```

## File I/O Operations

### Reading Files

#### `static FString ReadFileToString(const FilePath& path)` / `ReadFile(const FilePath& path)`

Read entire file as string.

```cpp
tkd::FileSystem::FilePath configFile = "config.txt";
tkd::FString content = tkd::FileSystem::ReadFile(configFile);

if (!content.IsEmpty()) {
    std::cout << "File content: " << content.CStr() << std::endl;
}
```

#### `static std::vector<UInt8> ReadFileToBytes(const FilePath& path)`

Read entire file as binary data.

```cpp
auto binaryData = tkd::FileSystem::ReadFileToBytes("image.png");
// Process binary data...
```

### Writing Files

#### `static bool WriteStringToFile(const FilePath& path, const FString& data)` / `WriteFile(const FilePath& path, const FString& data)`

Write string to file (overwrites existing).

```cpp
tkd::FString data = "Hello, World!";
tkd::FileSystem::WriteFile("output.txt", data);
```

#### `static bool WriteBytesToFile(const FilePath& path, const std::vector<UInt8>& data)`

Write binary data to file.

```cpp
std::vector<tkd::UInt8> binaryData = {0x00, 0x01, 0x02};
tkd::FileSystem::WriteBytesToFile("binary.dat", binaryData);
```

### Appending to Files

#### `static bool AppendStringToFile(const FilePath& path, const FString& data)` / `AppendToFile(const FilePath& path, const FString& data)`

Append string to existing file.

```cpp
tkd::FileSystem::AppendToFile("log.txt", "New log entry\n");
```

#### `static bool AppendBytesToFile(const FilePath& path, const std::vector<UInt8>& data)`

Append binary data to existing file.

```cpp
std::vector<tkd::UInt8> moreData = {0x03, 0x04};
tkd::FileSystem::AppendBytesToFile("binary.dat", moreData);
```

## Temporary Files and Directories

### Creating Unique Paths

#### `static FilePath GetUniqueTempFile(const FilePath& prefix, const FilePath& suffix)`

Generate unique temporary file path.

```cpp
auto tempFile = tkd::FileSystem::GetUniqueTempFile("myapp_", ".tmp");
// Creates path like: /tmp/myapp_12345.tmp
```

#### `static FilePath GetUniqueTempDirectory(const FilePath& prefix)`

Generate unique temporary directory path.

```cpp
auto tempDir = tkd::FileSystem::GetUniqueTempDirectory("session_");
// Creates path like: /tmp/session_67890/
```

## Error Handling

All FileSystem methods return boolean success indicators. For detailed error information:

```cpp
tkd::FileSystem::FilePath path = "nonexistent/directory/file.txt";

if (!tkd::FileSystem::WriteFile(path, "data")) {
    // Operation failed - check permissions, disk space, etc.
    std::cerr << "Failed to write file: " << path << std::endl;
}
```

## Performance Considerations

### Directory Traversal
- Recursive operations can be expensive on deep directory trees
- Use `ListFiles` for shallow traversal when possible
- Consider caching results for frequently accessed directories

### File I/O
- Reading entire files into memory may not be suitable for large files
- Use streaming I/O for large data processing
- Consider memory-mapped files for performance-critical scenarios

### Path Operations
- Path manipulation is generally fast
- Avoid repeated `GetAbsolutePath` calls on the same path
- Cache frequently used directory paths

## Platform-Specific Behavior

### Path Separators
- Windows: `\` (backslash)
- Unix/Linux/macOS: `/` (forward slash)
- FileSystem handles conversion automatically

### Case Sensitivity
- Windows: Case-insensitive
- Unix/Linux/macOS: Case-sensitive
- Be aware when porting applications

### Permissions
- Unix: Standard POSIX permissions (rwx)
- Windows: ACL-based permissions
- FileSystem abstracts these differences

### Special Directories
- Home directory detection varies by platform
- Temp directory uses system defaults
- Executable path detection uses platform-specific APIs

## Best Practices

### Resource Management
```cpp
// Good: Check existence before operations
if (tkd::FileSystem::IsFile("important.txt")) {
    auto content = tkd::FileSystem::ReadFile("important.txt");
    // Process content...
}

// Good: Use RAII for temporary files
class TempFile {
    tkd::FileSystem::FilePath path;
public:
    TempFile() : path(tkd::FileSystem::GetUniqueTempFile("temp", ".dat")) {}
    ~TempFile() { tkd::FileSystem::RemoveFile(path); }
    // ...
};
```

### Error Handling
```cpp
// Good: Check return values
bool success = tkd::FileSystem::CreateDirectories("output/logs");
if (!success) {
    std::cerr << "Failed to create log directory" << std::endl;
    return false;
}
```

### Path Safety
```cpp
// Good: Use absolute paths for critical operations
auto exeDir = tkd::FileSystem::GetExecutableDirectory();
auto configPath = exeDir / "config.json";

// Good: Validate paths before use
if (!tkd::FileSystem::IsDirectory(configPath.parent_path())) {
    tkd::FileSystem::CreateDirectories(configPath.parent_path());
}
```

## Integration with Engine Systems

### Asset Loading
```cpp
// Load game assets relative to executable
auto assetDir = tkd::FileSystem::GetExecutableDirectory() / "assets";
auto textureFiles = tkd::FileSystem::RecursiveListFilesByExtension(assetDir, ".png");
```

### Configuration Management
```cpp
// Save/load configuration files
auto configDir = tkd::FileSystem::GetHomeDirectory() / ".mygame";
tkd::FileSystem::CreateDirectories(configDir);

auto configFile = configDir / "settings.json";
tkd::FileSystem::WriteFile(configFile, configJson);
```

### Logging
```cpp
// Create log directory and files
auto logDir = tkd::FileSystem::GetExecutableDirectory() / "logs";
tkd::FileSystem::CreateDirectories(logDir);

auto logFile = logDir / "game.log";
tkd::FileSystem::AppendToFile(logFile, "Game started\n");
```

## Future Enhancements

### Planned Features
- **File watching**: Monitor file system changes
- **Asynchronous I/O**: Non-blocking file operations
- **Compression**: Built-in file compression/decompression
- **Encryption**: File encryption utilities
- **Cloud storage**: Integration with cloud storage APIs
- **Archive handling**: ZIP, TAR support
