///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Core/Containers.hpp>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Utility class for file system operations and path manipulation
///
/// The FileSystem class provides a comprehensive set of static methods for
/// performing common file system operations such as creating, deleting,
/// copying, and moving files and directories. It also includes utilities
/// for path manipulation, file existence checks, and directory traversal.
///////////////////////////////////////////////////////////////////////////////
class FileSystem
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Alias Member
    ///////////////////////////////////////////////////////////////////////////
    using FilePath = std::filesystem::path;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Creates a single directory
    ///
    /// Creates a directory at the specified path. This method will only
    /// create the final directory in the path and will fail if parent
    /// directories do not exist.
    ///
    /// \param path The path where the directory should be created
    ///
    /// \return True if the directory was created successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool Mkdir(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Creates directories recursively
    ///
    /// Creates all directories in the path hierarchy. This method will
    /// create any missing parent directories needed to create the
    /// specified directory.
    ///
    /// \param path The path where the directory structure should be created
    ///
    /// \return True if all directories were created successfully, false
    /// otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool Mkdirs(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if a file exists
    ///
    /// Determines whether the specified path exists in the file system.
    /// This method returns true for both files and directories.
    ///
    /// \param path The path to check for existence
    ///
    /// \return True if the path exists, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool FileExists(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if a directory exists
    ///
    /// Determines whether the specified path exists in the file system.
    /// This method returns true for both files and directories.
    ///
    /// \param path The path to check for existence
    ///
    /// \return True if the path exists, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool DirectoryExists(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the path is a directory
    ///
    /// Determines whether the specified path exists and is a directory.
    /// Returns false if the path does not exist or is a regular file.
    ///
    /// \param path The path to check
    ///
    /// \return True if the path is a directory, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool IsDirectory(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the path is a regular file
    ///
    /// Determines whether the specified path exists and is a regular file.
    /// Returns false if the path does not exist or is a directory.
    ///
    /// \param path The path to check
    ///
    /// \return True if the path is a regular file, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool IsFile(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the path has read permissions
    ///
    /// Determines whether the current process has read access to the
    /// specified file or directory.
    ///
    /// \param path The path to check for read permissions
    ///
    /// \return True if the path is readable, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool IsReadable(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the path has write permissions
    ///
    /// Determines whether the current process has write access to the
    /// specified file or directory.
    ///
    /// \param path The path to check for write permissions
    ///
    /// \return True if the path is writable, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool IsWritable(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Checks if the path has execute permissions
    ///
    /// Determines whether the current process has execute access to the
    /// specified file or directory.
    ///
    /// \param path The path to check for execute permissions
    ///
    /// \return True if the path is executable, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool IsExecutable(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Creates an empty file
    ///
    /// Creates a new empty file at the specified path. If the file already
    /// exists, this operation may fail depending on the implementation.
    ///
    /// \param path The path where the file should be created
    ///
    /// \return True if the file was created successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool CreateFile(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Creates a single directory
    ///
    /// Creates a directory at the specified path. This is an alias for
    /// the Mkdir method, providing consistent naming with other create
    /// operations.
    ///
    /// \param path The path where the directory should be created
    ///
    /// \return True if the directory was created successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool CreateDirectory(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Creates directories recursively
    ///
    /// Creates all directories in the path hierarchy. This is an alias for
    /// the Mkdirs method, providing consistent naming with other create
    /// operations.
    ///
    /// \param path The path where the directory structure should be created
    ///
    /// \return True if all directories were created successfully, false
    /// otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool CreateDirectories(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes a file from the file system
    ///
    /// Deletes the specified file. This operation will fail if the path
    /// points to a directory or if the file cannot be deleted due to
    /// permission issues.
    ///
    /// \param path The path to the file to be removed
    ///
    /// \return True if the file was removed successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool RemoveFile(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Removes a directory from the file system
    ///
    /// Deletes the specified directory. The directory must be empty for
    /// this operation to succeed.
    ///
    /// \param path The path to the directory to be removed
    ///
    /// \return True if the directory was removed successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool RemoveDirectory(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copies a file from source to destination
    ///
    /// Creates a copy of the source file at the destination path. If the
    /// destination file already exists, it will be overwritten.
    ///
    /// \param source The path to the source file to copy
    /// \param destination The path where the file should be copied
    ///
    /// \return True if the file was copied successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool CopyFile(const FilePath& source, const FilePath& destination);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Copies a directory from source to destination
    ///
    /// Creates a copy of the source directory at the destination path.
    /// Can optionally copy all subdirectories and files recursively.
    ///
    /// \param source The path to the source directory to copy
    /// \param destination The path where the directory should be copied
    /// \param recursive Whether to copy subdirectories recursively (default:
    /// true)
    ///
    /// \return True if the directory was copied successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool CopyDirectory(
        const FilePath& source,
        const FilePath& destination,
        bool recursive = true
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Moves a file from source to destination
    ///
    /// Moves the source file to the destination path. This is equivalent
    /// to a copy followed by a delete of the source file.
    ///
    /// \param source The path to the source file to move
    /// \param destination The path where the file should be moved
    ///
    /// \return True if the file was moved successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool MoveFile(const FilePath& source, const FilePath& destination);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Moves a directory from source to destination
    ///
    /// Moves the source directory to the destination path. This operation
    /// moves the entire directory structure including all subdirectories and
    /// files.
    ///
    /// \param source The path to the source directory to move
    /// \param destination The path where the directory should be moved
    ///
    /// \return True if the directory was moved successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool
        MoveDirectory(const FilePath& source, const FilePath& destination);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Renames a file
    ///
    /// Changes the name of a file from the old name to the new name.
    /// Both paths should refer to the same directory.
    ///
    /// \param oldName The current path/name of the file
    /// \param newName The new path/name for the file
    ///
    /// \return True if the file was renamed successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool RenameFile(const FilePath& oldName, const FilePath& newName);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Renames a directory
    ///
    /// Changes the name of a directory from the old name to the new name.
    /// Both paths should refer to the same parent directory.
    ///
    /// \param oldName The current path/name of the directory
    /// \param newName The new path/name for the directory
    ///
    /// \return True if the directory was renamed successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool
        RenameDirectory(const FilePath& oldName, const FilePath& newName);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the current working directory
    ///
    /// Returns the absolute path of the current working directory from
    /// which the application is running.
    ///
    /// \return The current working directory path
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FilePath GetCurrentWorkingDirectory(void);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the path to the current executable
    ///
    /// Returns the absolute path to the executable file of the current
    /// running application.
    ///
    /// \return The executable file path
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FilePath GetExecutablePath(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the directory containing the current executable
    ///
    /// Returns the absolute path to the directory that contains the
    /// executable file of the current running application.
    ///
    /// \return The executable directory path
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FilePath GetExecutableDirectory(void);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the user's home directory
    ///
    /// Returns the absolute path to the current user's home directory.
    /// On Windows this is typically C:\Users\username, on Unix systems
    /// this is typically /home/username.
    ///
    /// \return The user's home directory path
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FilePath GetHomeDirectory(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the system's temporary directory
    ///
    /// Returns the absolute path to the system's temporary directory
    /// where temporary files can be created.
    ///
    /// \return The temporary directory path
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FilePath GetTempDirectory(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the local application data directory
    ///
    /// Returns the absolute path to the local application data directory.
    /// On Windows this is typically C:\Users\username\AppData\Local.
    /// On Unix systems this is typically ~/.local/share.
    ///
    /// \return The local application data directory path
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FilePath GetLocalAppDataDirectory(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the roaming application data directory
    ///
    /// Returns the absolute path to the roaming application data directory.
    /// On Windows this is typically C:\Users\username\AppData\Roaming.
    /// On Unix systems this is typically ~/.config.
    ///
    /// \return The roaming application data directory path
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FilePath GetRoamingAppDataDirectory(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the last application data directory
    ///
    /// Returns the absolute path to the last application data directory.
    /// On Windows this is typically C:\ProgramData.
    /// On Unix systems this is typically /usr/local/share or /usr/share.
    ///
    /// \return The last application data directory path
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FilePath GetLastAppDataDirectory(void);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Creates a unique temporary file path
    ///
    /// Generates a unique file path in the system's temporary directory
    /// with the specified prefix and suffix. The file is not created,
    /// only the path is generated.
    ///
    /// \param prefix The prefix to use for the temporary file name
    /// \param suffix The suffix to use for the temporary file name
    ///
    /// \return A unique temporary file path
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FilePath
        GetUniqueTempFile(const FilePath& prefix, const FilePath& suffix);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Creates a unique temporary directory path
    ///
    /// Generates a unique directory path in the system's temporary directory
    /// with the specified prefix. The directory is not created, only the
    /// path is generated.
    ///
    /// \param prefix The prefix to use for the temporary directory name
    ///
    /// \return A unique temporary directory path
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FilePath GetUniqueTempDirectory(const FilePath& prefix);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Converts a path to its absolute form
    ///
    /// Resolves any relative path components and returns the absolute
    /// path. Relative paths are resolved against the current working
    /// directory.
    ///
    /// \param path The path to convert to absolute form
    ///
    /// \return The absolute path
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FilePath GetAbsolutePath(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the relative path from one location to another
    ///
    /// Calculates the relative path needed to navigate from the 'from'
    /// path to the 'to' path.
    ///
    /// \param from The starting path
    /// \param to The destination path
    ///
    /// \return The relative path from 'from' to 'to'
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FilePath GetRelativePath(const FilePath& from, const FilePath& to);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the parent directory of a path
    ///
    /// Returns the parent directory path of the specified path.
    /// For example, "/home/user/file.txt" returns "/home/user".
    ///
    /// \param path The path to get the parent directory from
    ///
    /// \return The parent directory path
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FilePath GetParentPath(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the filename from a path
    ///
    /// Extracts the filename (including extension) from the specified path.
    /// For example, "/home/user/file.txt" returns "file.txt".
    ///
    /// \param path The path to extract the filename from
    ///
    /// \return The filename with extension
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FilePath GetFileName(const FilePath& path);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the file extension from a path
    ///
    /// Extracts the file extension from the specified path.
    /// For example, "/home/user/file.txt" returns ".txt".
    ///
    /// \param path The path to extract the extension from
    ///
    /// \return The file extension including the dot
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FilePath GetFileExtension(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Gets the size of a file in bytes
    ///
    /// Returns the size of the specified file in bytes. The file must
    /// exist and be accessible for this operation to succeed.
    ///
    /// \param path The path to the file to measure
    ///
    /// \return The file size in bytes
    ///
    ///////////////////////////////////////////////////////////////////////////
    static SizeT GetFileSize(const FilePath& path);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Lists all files in a directory
    ///
    /// Returns a vector containing the paths of all regular files in the
    /// specified directory. Does not include subdirectories or hidden files.
    ///
    /// \param path The directory path to list files from
    ///
    /// \return A vector of file paths
    ///
    ///////////////////////////////////////////////////////////////////////////
    static std::vector<FilePath> ListFiles(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Lists all directories in a directory
    ///
    /// Returns a vector containing the paths of all subdirectories in the
    /// specified directory. Does not include regular files.
    ///
    /// \param path The directory path to list subdirectories from
    ///
    /// \return A vector of directory paths
    ///
    ///////////////////////////////////////////////////////////////////////////
    static std::vector<FilePath> ListDirectories(const FilePath& path);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Lists all entries in a directory
    ///
    /// Returns a vector containing the paths of all entries (both files
    /// and directories) in the specified directory.
    ///
    /// \param path The directory path to list entries from
    ///
    /// \return A vector of all entry paths (files and directories)
    ///
    ///////////////////////////////////////////////////////////////////////////
    static std::vector<FilePath> ListEntries(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Lists files with a specific extension in a directory
    ///
    /// Returns a vector containing the paths of all files in the specified
    /// directory that have the given file extension.
    ///
    /// \param path The directory path to search in
    /// \param extension The file extension to filter by (e.g., ".txt")
    ///
    /// \return A vector of file paths with the specified extension
    ///
    ///////////////////////////////////////////////////////////////////////////
    static std::vector<FilePath>
        ListFilesByExtension(const FilePath& path, const FilePath& extension);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Recursively lists all files in a directory tree
    ///
    /// Returns a vector containing the paths of all regular files in the
    /// specified directory and all its subdirectories recursively.
    ///
    /// \param path The root directory path to search from
    ///
    /// \return A vector of all file paths found recursively
    ///
    ///////////////////////////////////////////////////////////////////////////
    static std::vector<FilePath> RecursiveListFiles(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Recursively lists all directories in a directory tree
    ///
    /// Returns a vector containing the paths of all subdirectories in the
    /// specified directory and all its subdirectories recursively.
    ///
    /// \param path The root directory path to search from
    ///
    /// \return A vector of all directory paths found recursively
    ///
    ///////////////////////////////////////////////////////////////////////////
    static std::vector<FilePath>
        RecursiveListDirectories(const FilePath& path);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Recursively lists all entries in a directory tree
    ///
    /// Returns a vector containing the paths of all entries (both files
    /// and directories) in the specified directory and all its subdirectories
    /// recursively.
    ///
    /// \param path The root directory path to search from
    ///
    /// \return A vector of all entry paths found recursively
    ///
    ///////////////////////////////////////////////////////////////////////////
    static std::vector<FilePath> RecursiveListEntries(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Recursively lists files with a specific extension in a directory
    /// tree
    ///
    /// Returns a vector containing the paths of all files with the specified
    /// extension in the directory and all its subdirectories recursively.
    ///
    /// \param path The root directory path to search from
    /// \param extension The file extension to filter by (e.g., ".txt")
    ///
    /// \return A vector of file paths with the specified extension found
    /// recursively
    ///
    ///////////////////////////////////////////////////////////////////////////
    static std::vector<FilePath> RecursiveListFilesByExtension(
        const FilePath& path, const FilePath& extension
    );
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reads a file's contents as raw bytes
    ///
    /// Reads the entire content of the specified file and returns it as
    /// a vector of bytes. This is useful for binary files.
    ///
    /// \param path The path to the file to read
    ///
    /// \return A vector containing the file's raw bytes
    ///
    ///////////////////////////////////////////////////////////////////////////
    static std::vector<UInt8> ReadFileToBytes(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reads a file's contents as a string
    ///
    /// Reads the entire content of the specified file and returns it as
    /// a string. This is suitable for text files.
    ///
    /// \param path The path to the file to read
    ///
    /// \return A string containing the file's content
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FString ReadFileToString(const FilePath& path);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Reads a file's contents as a string (alias)
    ///
    /// Alias for ReadFileToString. Reads the entire content of the
    /// specified file and returns it as a string.
    ///
    /// \param path The path to the file to read
    ///
    /// \return A string containing the file's content
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FString ReadFile(const FilePath& path);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Writes raw bytes to a file
    ///
    /// Writes the specified byte data to a file, creating the file if it
    /// doesn't exist or overwriting it if it does.
    ///
    /// \param path The path to the file to write
    /// \param data The byte data to write to the file
    ///
    /// \return True if the data was written successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool
        WriteBytesToFile(const FilePath& path, const std::vector<UInt8>& data);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Writes a string to a file
    ///
    /// Writes the specified string data to a file, creating the file if it
    /// doesn't exist or overwriting it if it does.
    ///
    /// \param path The path to the file to write
    /// \param data The string data to write to the file
    ///
    /// \return True if the data was written successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool WriteStringToFile(const FilePath& path, const FString& data);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Writes a string to a file (alias)
    ///
    /// Alias for WriteStringToFile. Writes the specified string data to
    /// a file, creating or overwriting as needed.
    ///
    /// \param path The path to the file to write
    /// \param data The string data to write to the file
    ///
    /// \return True if the data was written successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool WriteFile(const FilePath& path, const FString& data);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Appends raw bytes to a file
    ///
    /// Appends the specified byte data to the end of an existing file.
    /// If the file doesn't exist, it will be created.
    ///
    /// \param path The path to the file to append to
    /// \param data The byte data to append to the file
    ///
    /// \return True if the data was appended successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool AppendBytesToFile(
        const FilePath& path, const std::vector<UInt8>& data
    );

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Appends a string to a file
    ///
    /// Appends the specified string data to the end of an existing file.
    /// If the file doesn't exist, it will be created.
    ///
    /// \param path The path to the file to append to
    /// \param data The string data to append to the file
    ///
    /// \return True if the data was appended successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool AppendStringToFile(const FilePath& path, const FString& data);
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Appends a string to a file (alias)
    ///
    /// Alias for AppendStringToFile. Appends the specified string data
    /// to the end of an existing file, creating it if needed.
    ///
    /// \param path The path to the file to append to
    /// \param data The string data to append to the file
    ///
    /// \return True if the data was appended successfully, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static bool AppendToFile(const FilePath& path, const FString& data);
};

}   // namespace tkd
