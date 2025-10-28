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
// Default Beat Saber Engine Settings
///////////////////////////////////////////////////////////////////////////////
static const FEngineSettings BEAT_SABER_ENGINE_SETTINGS = {
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
            .capability = ENetworkCapability::Optional,
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
            .title = "Beat Saber",
            .description = "A rhythm-based virtual reality game.",
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
            .inputActions = {},
            .inputAxes = {},
        },
        .accessibility = {
            .highContrastMode = false,
            .colorblindMode = 0,
            .uiScale = 1.0f,
            .subtitles = true,
            .subtitleSize = 24,
        },
        .vr = {
            .capability = EVRCapability::Optional,
            .device = EVRDevice::OpenVR,
            .movementMode = EVRMovementMode::SmoothLocomotion,
            .snapTurnAngle = EVRSnapTurnAngle::None,
        }
    };

}   // namespace tkd
