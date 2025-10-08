///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
// Default R-Type Engine Settings
///////////////////////////////////////////////////////////////////////////////
static const FEngineSettings RTYPE_ENGINE_SETTINGS = {
        .version = TKD_VERSION_STRING,
        .debug = false,
        .window = {
            .enableVSync = true,
            .targetFPS = -1,
            .windowWidth = 1280,
            .windowHeight = 720,
            .isFullscreen = false,
            .brightness = 1.0f,
            .showCursor = true,
            .resizable = true,
            .borderless = false,
        },
        .network = {
            .enabled = true,
            .maxClients = -1,
            .port = 8080,
            .timeout = 5000,
            .maxPacketSize = 1452,
            .protocolVersion = 1,
        },
        .logging = {
            .enabled = true,
            .enableConsoleLogging = true,
            .enableFileLogging = false,
            .maxLogFileSizeMB = 5,
            .logRetentionDays = 7,
        },
        .game = {
            .title = "R-Type",
            .description = "A classic side-scrolling shooter game.",
            .version = "1.0.0",
            .author = "Tekyo Drift",
            .license = "MIT",
        },
        .audio = {
            .enabled = true,
            .masterVolume = 100,
            .musicVolume = 80,
            .sfxVolume = 80,
            .enable3DAudio = true,
        },
        .inputs = {
            .enableGamepad = true,
            .mouseSensitivity = 1.0f,
            .invertYAxis = false,
            .inputActions = {
                { "Fire", {
                    tkd::EInput::Mouse_Left,
                    tkd::EInput::GamePadAxis_RightTrigger,
                    tkd::EInput::Keyboard_Space
                } }
            },
            .inputAxes = {
                { "HorizontalMoves", {
                    { tkd::EInput::Keyboard_A, -1.0f },
                    { tkd::EInput::Keyboard_D, 1.0f },
                    { tkd::EInput::Keyboard_Left, -1.0f },
                    { tkd::EInput::Keyboard_Right, 1.0f },
                    { tkd::EInput::GamePadAxis_LeftX, -1.0f },
                    { tkd::EInput::GamePadAxis_RightX, 1.0f },
                } },
                { "VerticalMoves", {
                    { tkd::EInput::Keyboard_W, -1.0f },
                    { tkd::EInput::Keyboard_S, 1.0f },
                    { tkd::EInput::Keyboard_Up, -1.0f },
                    { tkd::EInput::Keyboard_Down, 1.0f },
                    { tkd::EInput::GamePadAxis_LeftY, -1.0f },
                    { tkd::EInput::GamePadAxis_RightY, 1.0f },
                } }
            },
        },
        .accessibility = {
            .highContrastMode = false,
            .colorblindMode = 0,
            .uiScale = 1.0f,
            .subtitles = true,
            .subtitleSize = 24,
        },
    };

}   // namespace tkd
