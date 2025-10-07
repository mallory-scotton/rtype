///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <variant>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Keyboard keys enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class EKeyboardKeys
{
    Unknown = -1,
    A = 0,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    Num0,
    Num1,
    Num2,
    Num3,
    Num4,
    Num5,
    Num6,
    Num7,
    Num8,
    Num9,
    Escape,
    LControl,
    LShift,
    LAlt,
    LSystem,
    RControl,
    RShift,
    RAlt,
    RSystem,
    Menu,
    LBracket,
    RBracket,
    Semicolon,
    Comma,
    Period,
    Quote,
    Slash,
    Backslash,
    Tilde,
    Equal,
    Hyphen,
    Space,
    Enter,
    Backspace,
    Tab,
    PageUp,
    PageDown,
    End,
    Home,
    Insert,
    Delete,
    Add,
    Subtract,
    Multiply,
    Divide,
    Left,
    Right,
    Up,
    Down,
    Numpad0,
    Numpad1,
    Numpad2,
    Numpad3,
    Numpad4,
    Numpad5,
    Numpad6,
    Numpad7,
    Numpad8,
    Numpad9,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    Pause
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Mouse buttons enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class EMouseButtons
{
    Unknown = -1,
    Left = 101,
    Right,
    Middle,
    XButton1,
    XButton2
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Gamepad buttons enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class EGamepadButtons
{
    Unknown = -1,
    A = 106,
    B,
    X,
    Y,
    Back,
    Guide,
    Start,
    LeftStick,
    RightStick,
    LeftShoulder,
    RightShoulder,
    DPadUp,
    DPadDown,
    DPadLeft,
    DPadRight
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Gamepad axes enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class EGamepadAxes
{
    Unknown = -1,
    LeftX = 121,
    LeftY,
    RightX,
    RightY,
    LeftTrigger,
    RightTrigger
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Input devices enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class EInputDevice
{
    Keyboard,
    Mouse,
    Gamepad,
    Touch,
    Count
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Input state enumeration
///
///////////////////////////////////////////////////////////////////////////////
enum class EInputState
{
    Pressed,
    Released,
    Held
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Input key and axis variants
///
///////////////////////////////////////////////////////////////////////////////
enum class EInput
{
    Unknown = -1,
    Keyboard_A,
    Keyboard_B,
    Keyboard_C,
    Keyboard_D,
    Keyboard_E,
    Keyboard_F,
    Keyboard_G,
    Keyboard_H,
    Keyboard_I,
    Keyboard_J,
    Keyboard_K,
    Keyboard_L,
    Keyboard_M,
    Keyboard_N,
    Keyboard_O,
    Keyboard_P,
    Keyboard_Q,
    Keyboard_R,
    Keyboard_S,
    Keyboard_T,
    Keyboard_U,
    Keyboard_V,
    Keyboard_W,
    Keyboard_X,
    Keyboard_Y,
    Keyboard_Z,
    Keyboard_Num0,
    Keyboard_Num1,
    Keyboard_Num2,
    Keyboard_Num3,
    Keyboard_Num4,
    Keyboard_Num5,
    Keyboard_Num6,
    Keyboard_Num7,
    Keyboard_Num8,
    Keyboard_Num9,
    Keyboard_Escape,
    Keyboard_LControl,
    Keyboard_LShift,
    Keyboard_LAlt,
    Keyboard_LSystem,
    Keyboard_RControl,
    Keyboard_RShift,
    Keyboard_RAlt,
    Keyboard_RSystem,
    Keyboard_Menu,
    Keyboard_LBracket,
    Keyboard_RBracket,
    Keyboard_Semicolon,
    Keyboard_Comma,
    Keyboard_Period,
    Keyboard_Quote,
    Keyboard_Slash,
    Keyboard_Backslash,
    Keyboard_Tilde,
    Keyboard_Equal,
    Keyboard_Hyphen,
    Keyboard_Space,
    Keyboard_Enter,
    Keyboard_Backspace,
    Keyboard_Tab,
    Keyboard_PageUp,
    Keyboard_PageDown,
    Keyboard_End,
    Keyboard_Home,
    Keyboard_Insert,
    Keyboard_Delete,
    Keyboard_Add,
    Keyboard_Subtract,
    Keyboard_Multiply,
    Keyboard_Divide,
    Keyboard_Left,
    Keyboard_Right,
    Keyboard_Up,
    Keyboard_Down,
    Keyboard_Numpad0,
    Keyboard_Numpad1,
    Keyboard_Numpad2,
    Keyboard_Numpad3,
    Keyboard_Numpad4,
    Keyboard_Numpad5,
    Keyboard_Numpad6,
    Keyboard_Numpad7,
    Keyboard_Numpad8,
    Keyboard_Numpad9,
    Keyboard_F1,
    Keyboard_F2,
    Keyboard_F3,
    Keyboard_F4,
    Keyboard_F5,
    Keyboard_F6,
    Keyboard_F7,
    Keyboard_F8,
    Keyboard_F9,
    Keyboard_F10,
    Keyboard_F11,
    Keyboard_F12,
    Keyboard_F13,
    Keyboard_F14,
    Keyboard_F15,
    Keyboard_Pause,
    Mouse_Left,
    Mouse_Right,
    Mouse_Middle,
    Mouse_XButton1,
    Mouse_XButton2,
    GamePad_A,
    GamePad_B,
    GamePad_X,
    GamePad_Y,
    GamePad_Back,
    GamePad_Guide,
    GamePad_Start,
    GamePad_LeftStick,
    GamePad_RightStick,
    GamePad_LeftShoulder,
    GamePad_RightShoulder,
    GamePad_DPadUp,
    GamePad_DPadDown,
    GamePad_DPadLeft,
    GamePad_DPadRight,
    GamePadAxis_LeftX,
    GamePadAxis_LeftY,
    GamePadAxis_RightX,
    GamePadAxis_RightY,
    GamePadAxis_LeftTrigger,
    GamePadAxis_RightTrigger
};

}   // namespace tkd
