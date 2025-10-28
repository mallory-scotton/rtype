///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config.hpp>
#include <Engine/Core/Containers.hpp>
#include <Engine/Core/Utils/FileSystem.hpp>
#include <format>
#include <fstream>
#include <iostream>
#include <mutex>
#include <optional>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Log level enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class ELogLevel : UInt8
{
    Off,
    Fatal,
    Error,
    Warn,
    Info,
    Debug,
    Trace,
    All
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Palette for the different logging level
///
///////////////////////////////////////////////////////////////////////////////
struct FLoggerPalette
{
    FString fatal = "\033[1;41;97m";   //<! Fatal log colors
    FString error = "\033[1;31m";      //<! Error log colors
    FString warn = "\033[1;33m";       //<! Warn log colors
    FString info = "\033[1;32m";       //<! Info log colors
    FString debug = "\033[1;36m";      //<! Debug log colors
    FString trace = "\033[0;37m";      //<! Trace log colors
    FString all = "\033[0m";           //<! Fallback log colors
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Utility class to handle logs in the TKD engine
///
///////////////////////////////////////////////////////////////////////////////
class FLogger
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Class Static Members
    ///////////////////////////////////////////////////////////////////////////
    static Bool s_consoleLogging;           //<! Console logging enabled?
    static Bool s_fileLogging;              //<! File logging enabled?
    static ELogLevel s_logLevel;            //<! Current log level
    static FilePath s_logDirectoryPath;     //<! Directory of the logs
    static FilePath s_logFilePath;          //<! Current logging file
    static Bool s_usePalette;               //<! Uses color palette
    static FLoggerPalette s_palette;        //<! The palette to use
    static std::mutex s_mutex;              //<! Mutex for thread safety
    static Bool s_useTimestamp;             //<! Timestamp enabled?
    static std::ofstream s_logFileStream;   //<! File stream for logging
    static UInt64 s_maxLogFileSizeBytes;    //<! Max log file size in bytes
    static Bool s_useNamespace;             //<! Enable namespace support
    static FString s_currentNamespace;      //<! Current logging namespace

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the appropriate output stream based on log level
    ///
    /// \param level The log level
    ///
    /// \return Reference to std::cerr or std::cout
    ///
    ///////////////////////////////////////////////////////////////////////////
    static std::ostream& GetOutputStream(ELogLevel level);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the color code for a given log level
    ///
    /// \param level The log level
    ///
    /// \return Color code string
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FString GetColor(ELogLevel level);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the formatted timestamp string
    ///
    /// \return Formatted timestamp
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FString GetTimestamp(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the log level name as a string
    ///
    /// \param level The log level
    ///
    /// \return Log level name
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FString GetLevelName(ELogLevel level);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get the formatted namespace string
    ///
    /// \return Formatted namespace or empty string
    ///
    ///////////////////////////////////////////////////////////////////////////
    static FString GetNamespace(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Initialize the log file for writing
    ///
    /// \return True if successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Bool InitializeLogFile(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Close the current log file
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void CloseLogFile(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Check if log file needs rotation and rotate if necessary
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void RotateLogFileIfNeeded(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Write a message to the log file
    ///
    /// \param level The log level
    /// \param message The message to write
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void WriteToFile(ELogLevel level, const FString& message);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the current log level
    ///
    /// \param level The log level to set
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void SetLogLevel(ELogLevel level);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Enable or disable console logging
    ///
    /// \param enabled True to enable, false to disable
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void SetConsoleLogging(Bool enabled);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Enable or disable file logging
    ///
    /// \param enabled True to enable, false to disable
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void SetFileLogging(Bool enabled);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the directory path where log files will be stored
    ///
    /// \param filePath The directory path
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void SetLogDirectoryPath(const FilePath& filePath);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the color palette for console output
    ///
    /// \param palette The palette to use
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void SetColorPalette(FLoggerPalette palette);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Enable or disable color palette usage
    ///
    /// \param enabled True to enable, false to disable
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void SetPaletteUsage(Bool enabled);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Enable or disable timestamp in logs
    ///
    /// \param enabled True to enable, false to disable
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void SetTimestampUsage(Bool enabled);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the maximum log file size before rotation
    ///
    /// \param sizeMB Maximum file size in megabytes
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void SetMaxLogFileSize(UInt32 sizeMB);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Enable or disable namespace usage in logs
    ///
    /// \param enabled True to enable, false to disable
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void SetNamespaceUsage(Bool enabled);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Set the current logging namespace
    ///
    /// \param namespaceName The namespace to use (e.g., "Network",
    /// "Rendering")
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void SetNamespace(const FString& namespaceName);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Clear the current namespace
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void ClearNamespace(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Initialize the logger system
    ///
    /// \return True if successful, false otherwise
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Bool Initialize(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Shutdown the logger system and close all resources
    ///
    ///////////////////////////////////////////////////////////////////////////
    static void Shutdown(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Log a message at a specific level
    ///
    /// \param level The log level
    /// \param format Format string
    /// \param args Variadic arguments for formatting
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    static void Log(ELogLevel level, std::string_view format, Args&&... args)
    {
        std::lock_guard lock(s_mutex);

        if (level == ELogLevel::Off) { return; }
        if (static_cast<UInt8>(level) > static_cast<UInt8>(s_logLevel))
        {
            return;
        }

        std::string message =
            std::vformat(format, std::make_format_args(args...));

        if (s_consoleLogging)
        {
            GetOutputStream(level)
                << GetTimestamp() << GetNamespace() << GetColor(level)
                << message << "\033[0m" << std::endl;
        }

        if (s_fileLogging) { WriteToFile(level, message); }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Log a message at a specific level with a temporary namespace
    ///
    /// \param level The log level
    /// \param namespaceName The namespace to use for this log entry
    /// \param format Format string
    /// \param args Variadic arguments for formatting
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    static void LogWithNamespace(
        ELogLevel level,
        const FString& namespaceName,
        std::string_view format,
        Args&&... args
    )
    {
        std::lock_guard lock(s_mutex);

        if (level == ELogLevel::Off) { return; }
        if (static_cast<UInt8>(level) > static_cast<UInt8>(s_logLevel))
        {
            return;
        }

        // Temporarily save the current namespace
        FString previousNamespace = s_currentNamespace;
        Bool previousUseNamespace = s_useNamespace;

        // Set the temporary namespace
        s_currentNamespace = namespaceName;
        s_useNamespace = true;

        std::string message =
            std::vformat(format, std::make_format_args(args...));

        if (s_consoleLogging)
        {
            GetOutputStream(level)
                << GetTimestamp() << GetNamespace() << GetColor(level)
                << message << "\033[0m" << std::endl;
        }

        if (s_fileLogging) { WriteToFile(level, message); }

        // Restore the previous namespace
        s_currentNamespace = previousNamespace;
        s_useNamespace = previousUseNamespace;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Log a fatal error message
    ///
    /// \param format Format string
    /// \param args Variadic arguments for formatting
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    static void Fatal(std::string_view format, Args&&... args)
    {
        FLogger::Log(ELogLevel::Fatal, format, std::forward<Args>(args)...);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Log an error message
    ///
    /// \param format Format string
    /// \param args Variadic arguments for formatting
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    static void Error(std::string_view format, Args&&... args)
    {
        FLogger::Log(ELogLevel::Error, format, std::forward<Args>(args)...);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Log a warning message
    ///
    /// \param format Format string
    /// \param args Variadic arguments for formatting
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    static void Warn(std::string_view format, Args&&... args)
    {
        FLogger::Log(ELogLevel::Warn, format, std::forward<Args>(args)...);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Log an info message
    ///
    /// \param format Format string
    /// \param args Variadic arguments for formatting
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    static void Info(std::string_view format, Args&&... args)
    {
        FLogger::Log(ELogLevel::Info, format, std::forward<Args>(args)...);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Log a debug message
    ///
    /// \param format Format string
    /// \param args Variadic arguments for formatting
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    static void Debug(std::string_view format, Args&&... args)
    {
        FLogger::Log(ELogLevel::Debug, format, std::forward<Args>(args)...);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Log a trace message
    ///
    /// \param format Format string
    /// \param args Variadic arguments for formatting
    ///
    ///////////////////////////////////////////////////////////////////////////
    template <typename... Args>
    static void Trace(std::string_view format, Args&&... args)
    {
        FLogger::Log(ELogLevel::Trace, format, std::forward<Args>(args)...);
    }
};

}   // namespace tkd
