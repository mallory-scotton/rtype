///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Config/Config.hpp>
#include <Engine/Config/EInputs.hpp>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Engine configuration settings
///
///////////////////////////////////////////////////////////////////////////////
struct FEngineSettings
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Settings Members
    ///////////////////////////////////////////////////////////////////////////
    std::string version = TKD_VERSION_STRING;   //<! Engine version
    bool debug = false;                         //<! Enable debug mode

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Window settings
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct Window
    {
        bool enableVSync = true;     //<! Enable or disable V-Sync
        int targetFPS = -1;          //<! Target frames per second (-1 for inf)
        int windowWidth = 1280;      //<! Default window width
        int windowHeight = 720;      //<! Default window height
        bool isFullscreen = false;   //<! Start in fullscreen mode
        float brightness = 1.0f;     //<! Default brightness level
        bool showCursor = true;      //<! Show or hide the cursor
        bool resizable = true;       //<! Allow window resizing
        bool borderless = false;     //<! Enable or disable window borders
    } window;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Network settings
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct Network
    {
        bool enabled = true;        //<! Enable or disable networking
        int maxClients = -1;        //<! Maximum number of clients
        int port = 8080;            //<! Default network port
        int timeout = 5000;         //<! Network timeout in milliseconds
        int maxPacketSize = 1452;   //<! Maximum UDP packet size
        int protocolVersion = 1;    //<! Network protocol version
    } network;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Logging settings
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct Logging
    {
        bool enabled = true;                //<! Enable or disable logging
        bool enableConsoleLogging = true;   //<! Enable or disable console log
        bool enableFileLogging = false;     //<! Enable or disable file log
        int maxLogFileSizeMB = 5;           //<! Maximum log file size in MB
        int logRetentionDays = 7;           //<! Days to retain log files
    } logging;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Game Settings and Metadata
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct Game
    {
        std::string title;         //<! Title of the game
        std::string description;   //<! Description of the game
        std::string version;       //<! Version of the game
        std::string author;        //<! Author of the game
        std::string license;       //<! License information
    } game;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Audio settings
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct Audio
    {
        bool enabled = true;         //<! Enable or disable audio
        int masterVolume = 100;      //<! Master volume level (0-100)
        int musicVolume = 80;        //<! Music volume level (0-100)
        int sfxVolume = 80;          //<! Sound effects volume level (0-100)
        bool enable3DAudio = true;   //<! Enable or disable 3D audio
    } audio;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Inputs settings
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct Inputs
    {
        bool enableGamepad = true;       //<! Enable or disable gamepad support
        float mouseSensitivity = 1.0f;   //<! Mouse sensitivity multiplier
        bool invertYAxis = false;        //<! Invert Y-axis for mouse/gamepad
        std::unordered_map<std::string, std::vector<EInput>>
            inputActions = {};           //<! Action mappings
        std::unordered_map<std::string, std::vector<std::pair<EInput, float>>>
            inputAxes = {};              //<! Axis mappings
    } inputs;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Accessibility settings
    ///
    ///////////////////////////////////////////////////////////////////////////
    struct Accessibility
    {
        bool highContrastMode = false;   //<! Enable or disable high contrast
        int colorblindMode = 0;          //<! Enable or disable colorblind mode
        float uiScale = 1.0f;            //<! UI scale factor
        bool subtitles = true;           //<! Enable or disable subtitles
        int subtitleSize = 24;           //<! Subtitle font size
    } accessibility;
};

}   // namespace tkd
