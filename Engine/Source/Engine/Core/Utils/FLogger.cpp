///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Utils/FLogger.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
Bool FLogger::s_consoleLogging = true;
Bool FLogger::s_fileLogging = false;
ELogLevel FLogger::s_logLevel = ELogLevel::Info;
FilePath FLogger::s_logDirectoryPath =
    FileSystem::GetExecutableDirectory() / "Logs";
FilePath FLogger::s_logFilePath;
Bool FLogger::s_usePalette = true;
FLoggerPalette FLogger::s_palette;
std::mutex FLogger::s_mutex;
Bool FLogger::s_useTimestamp = true;
std::ofstream FLogger::s_logFileStream;
UInt64 FLogger::s_maxLogFileSizeBytes = 5 * 1024 * 1024;   // 5 MB default
Bool FLogger::s_useNamespace = false;
FString FLogger::s_currentNamespace = "";

///////////////////////////////////////////////////////////////////////////////
std::ostream& FLogger::GetOutputStream(ELogLevel level)
{
    return static_cast<UInt8>(level) < static_cast<UInt8>(ELogLevel::Info)
               ? std::cerr
               : std::cout;
}

///////////////////////////////////////////////////////////////////////////////
FString FLogger::GetColor(ELogLevel level)
{
    if (!s_usePalette) { return ""; }
    switch (level)
    {
    case ELogLevel::Fatal: return s_palette.fatal;
    case ELogLevel::Error: return s_palette.error;
    case ELogLevel::Warn : return s_palette.warn;
    case ELogLevel::Info : return s_palette.info;
    case ELogLevel::Debug: return s_palette.debug;
    case ELogLevel::Trace: return s_palette.trace;
    default              : return s_palette.all;
    }
}

///////////////////////////////////////////////////////////////////////////////
FString FLogger::GetTimestamp(void)
{
    if (!s_useTimestamp) { return ""; }

    const auto now = std::chrono::system_clock::now();
    const auto nowTime = std::chrono::system_clock::to_time_t(now);
    const auto localTm = *std::localtime(&nowTime);

    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S] ", &localTm);

    if (s_usePalette) { return FString::Format("\033[90m{0}\033[0m", buffer); }
    else { return FString(buffer); }
}

///////////////////////////////////////////////////////////////////////////////
FString FLogger::GetLevelName(ELogLevel level)
{
    switch (level)
    {
    case ELogLevel::Fatal: return "[FATAL]";
    case ELogLevel::Error: return "[ERROR]";
    case ELogLevel::Warn : return "[WARN ]";
    case ELogLevel::Info : return "[INFO ]";
    case ELogLevel::Debug: return "[DEBUG]";
    case ELogLevel::Trace: return "[TRACE]";
    default              : return "[     ]";
    }
}

///////////////////////////////////////////////////////////////////////////////
FString FLogger::GetNamespace(void)
{
    if (!s_useNamespace || s_currentNamespace.IsEmpty()) { return ""; }

    if (s_usePalette)
    {
        return FString::Format(
            "\033[95m[{0}]\033[0m ", s_currentNamespace.ToStdString()
        );
    }
    else
    {
        return FString::Format("[{0}] ", s_currentNamespace.ToStdString());
    }
}

///////////////////////////////////////////////////////////////////////////////
Bool FLogger::InitializeLogFile(void)
{
    // Create log directory if it doesn't exist
    if (!FileSystem::IsDirectory(s_logDirectoryPath))
    {
        if (!FileSystem::CreateDirectories(s_logDirectoryPath))
        {
            std::cerr << "Failed to create log directory: "
                      << s_logDirectoryPath.string() << std::endl;
            return false;
        }
    }

    // Generate log file name with timestamp
    const auto now = std::chrono::system_clock::now();
    const auto nowTime = std::chrono::system_clock::to_time_t(now);
    const auto localTm = *std::localtime(&nowTime);

    char timeBuffer[64];
    std::strftime(
        timeBuffer, sizeof(timeBuffer), "%Y-%m-%d_%H-%M-%S", &localTm
    );

    s_logFilePath =
        s_logDirectoryPath / FilePath(std::format("log_{0}.txt", timeBuffer));

    // Open the log file
    s_logFileStream.open(s_logFilePath, std::ios::out | std::ios::app);

    if (!s_logFileStream.is_open())
    {
        std::cerr << "Failed to open log file: " << s_logFilePath.string()
                  << std::endl;
        return false;
    }

    // Write header
    s_logFileStream << "========================================\n";
    s_logFileStream << "TKD Engine Log File\n";
    s_logFileStream << "Created: " << timeBuffer << "\n";
    s_logFileStream << "========================================\n";
    s_logFileStream.flush();

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void FLogger::CloseLogFile(void)
{
    if (s_logFileStream.is_open())
    {
        s_logFileStream << "========================================\n";
        s_logFileStream << "Log file closed\n";
        s_logFileStream << "========================================\n";
        s_logFileStream.close();
    }
}

///////////////////////////////////////////////////////////////////////////////
void FLogger::RotateLogFileIfNeeded(void)
{
    if (!s_logFileStream.is_open() || s_logFilePath.empty()) { return; }

    try
    {
        // Check file size
        UInt64 fileSize = FileSystem::GetFileSize(s_logFilePath);

        if (fileSize >= s_maxLogFileSizeBytes)
        {
            CloseLogFile();
            InitializeLogFile();
        }
    }
    catch (...)
    {
        // If we can't check file size, continue logging
    }
}

///////////////////////////////////////////////////////////////////////////////
void FLogger::WriteToFile(ELogLevel level, const FString& message)
{
    if (!s_logFileStream.is_open())
    {
        if (!InitializeLogFile()) { return; }
    }

    RotateLogFileIfNeeded();

    // Build the log entry without color codes
    const auto now = std::chrono::system_clock::now();
    const auto nowTime = std::chrono::system_clock::to_time_t(now);
    const auto localTm = *std::localtime(&nowTime);

    char timeBuffer[32];
    std::strftime(
        timeBuffer, sizeof(timeBuffer), "[%Y-%m-%d %H:%M:%S]", &localTm
    );

    FString namespaceStr = "";
    if (s_useNamespace && !s_currentNamespace.IsEmpty())
    {
        namespaceStr =
            FString::Format("[{0}] ", s_currentNamespace.ToStdString());
    }

    s_logFileStream << timeBuffer << " " << namespaceStr.ToStdString()
                    << GetLevelName(level) << " " << message << "\n";
    s_logFileStream.flush();
}

///////////////////////////////////////////////////////////////////////////////
void FLogger::SetLogLevel(ELogLevel level)
{
    std::lock_guard lock(s_mutex);
    s_logLevel = level;
}

///////////////////////////////////////////////////////////////////////////////
void FLogger::SetConsoleLogging(Bool enabled)
{
    std::lock_guard lock(s_mutex);
    s_consoleLogging = enabled;
}

///////////////////////////////////////////////////////////////////////////////
void FLogger::SetFileLogging(Bool enabled)
{
    std::lock_guard lock(s_mutex);

    if (enabled && !s_fileLogging)
    {
        // Enabling file logging
        s_fileLogging = enabled;
        InitializeLogFile();
    }
    else if (!enabled && s_fileLogging)
    {
        // Disabling file logging
        s_fileLogging = enabled;
        CloseLogFile();
    }
    else { s_fileLogging = enabled; }
}

///////////////////////////////////////////////////////////////////////////////
void FLogger::SetLogDirectoryPath(const FilePath& filePath)
{
    std::lock_guard lock(s_mutex);
    s_logDirectoryPath = filePath;
}

///////////////////////////////////////////////////////////////////////////////
void FLogger::SetColorPalette(FLoggerPalette palette)
{
    std::lock_guard lock(s_mutex);
    s_palette = palette;
}

///////////////////////////////////////////////////////////////////////////////
void FLogger::SetPaletteUsage(Bool enabled)
{
    std::lock_guard lock(s_mutex);
    s_usePalette = enabled;
}

///////////////////////////////////////////////////////////////////////////////
void FLogger::SetTimestampUsage(bool enabled)
{
    std::lock_guard lock(s_mutex);
    s_useTimestamp = enabled;
}

///////////////////////////////////////////////////////////////////////////////
void FLogger::SetMaxLogFileSize(UInt32 sizeMB)
{
    std::lock_guard lock(s_mutex);
    s_maxLogFileSizeBytes = static_cast<UInt64>(sizeMB) * 1024 * 1024;
}

///////////////////////////////////////////////////////////////////////////////
void FLogger::SetNamespaceUsage(Bool enabled)
{
    std::lock_guard lock(s_mutex);
    s_useNamespace = enabled;
}

///////////////////////////////////////////////////////////////////////////////
void FLogger::SetNamespace(const FString& namespaceName)
{
    std::lock_guard lock(s_mutex);
    s_currentNamespace = namespaceName;
    s_useNamespace = true;
}

///////////////////////////////////////////////////////////////////////////////
void FLogger::ClearNamespace(void)
{
    std::lock_guard lock(s_mutex);
    s_currentNamespace = "";
    s_useNamespace = false;
}

///////////////////////////////////////////////////////////////////////////////
Bool FLogger::Initialize(void)
{
    std::lock_guard lock(s_mutex);

    if (s_fileLogging) { return InitializeLogFile(); }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void FLogger::Shutdown(void)
{
    std::lock_guard lock(s_mutex);
    CloseLogFile();
}

}   // namespace tkd
