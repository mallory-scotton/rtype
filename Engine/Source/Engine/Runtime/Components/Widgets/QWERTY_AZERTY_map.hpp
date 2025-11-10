///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Runtime/Input/FInputManager.hpp>
#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{
///////////////////////////////////////////////////////////////////////////////
// Namespace KeyboardMapping
///////////////////////////////////////////////////////////////////////////////
namespace KeyboardMapping
{
// Normal (no Shift)
const std::unordered_map<EInput, char> QWERTY_TO_AZERTY = {
    // Letters
    {         EInput::Keyboard_A,  'q' },
    {         EInput::Keyboard_Z,  'w' },
    {         EInput::Keyboard_Q,  'a' },
    {         EInput::Keyboard_W,  'z' },
    {         EInput::Keyboard_M,  ',' },
    {     EInput::Keyboard_Comma,  ';' },
    { EInput::Keyboard_Semicolon,  'm' },
    {         EInput::Keyboard_E,  'e' },
    {         EInput::Keyboard_R,  'r' },
    {         EInput::Keyboard_T,  't' },
    {         EInput::Keyboard_Y,  'y' },
    {         EInput::Keyboard_U,  'u' },
    {         EInput::Keyboard_I,  'i' },
    {         EInput::Keyboard_O,  'o' },
    {         EInput::Keyboard_P,  'p' },
    {         EInput::Keyboard_S,  's' },
    {         EInput::Keyboard_D,  'd' },
    {         EInput::Keyboard_F,  'f' },
    {         EInput::Keyboard_G,  'g' },
    {         EInput::Keyboard_H,  'h' },
    {         EInput::Keyboard_J,  'j' },
    {         EInput::Keyboard_K,  'k' },
    {         EInput::Keyboard_L,  'l' },
    {         EInput::Keyboard_X,  'x' },
    {         EInput::Keyboard_C,  'c' },
    {         EInput::Keyboard_V,  'v' },
    {         EInput::Keyboard_B,  'b' },
    {         EInput::Keyboard_N,  'n' },

    // Numbers (AZERTY unshifted)
    {      EInput::Keyboard_Num1,  '&' },
    {      EInput::Keyboard_Num2,  'e' },
    {      EInput::Keyboard_Num3,  '"' },
    {      EInput::Keyboard_Num4, '\'' },
    {      EInput::Keyboard_Num5,  '(' },
    {      EInput::Keyboard_Num6,  '-' },
    {      EInput::Keyboard_Num7,  'e' },
    {      EInput::Keyboard_Num8,  '_' },
    {      EInput::Keyboard_Num9,  'c' },
    {      EInput::Keyboard_Num0,  'a' },

    // Numbers numpad
    {   EInput::Keyboard_Numpad1,  '1' },
    {   EInput::Keyboard_Numpad2,  '2' },
    {   EInput::Keyboard_Numpad3,  '3' },
    {   EInput::Keyboard_Numpad4,  '4' },
    {   EInput::Keyboard_Numpad5,  '5' },
    {   EInput::Keyboard_Numpad6,  '6' },
    {   EInput::Keyboard_Numpad7,  '7' },
    {   EInput::Keyboard_Numpad8,  '8' },
    {   EInput::Keyboard_Numpad9,  '9' },
    {   EInput::Keyboard_Numpad0,  '0' },

    // Punctuation
    {    EInput::Keyboard_Hyphen,  ')' },
    {     EInput::Keyboard_Equal,  '=' },
    {  EInput::Keyboard_LBracket,  '^' },
    {  EInput::Keyboard_RBracket,  '$' },
    {     EInput::Keyboard_Tilde,  'u' },
    { EInput::Keyboard_Backslash,  '*' },
    {     EInput::Keyboard_Comma,  ';' },
    {    EInput::Keyboard_Period,  ':' },
    {     EInput::Keyboard_Slash,  '!' },
    {     EInput::Keyboard_Space,  ' ' }
};

// Shifted (with Shift key)
const std::unordered_map<EInput, char> QWERTY_TO_AZERTY_SHIFT = {
    // Letters
    {         EInput::Keyboard_A, 'Q' },
    {         EInput::Keyboard_Z, 'W' },
    {         EInput::Keyboard_Q, 'A' },
    {         EInput::Keyboard_W, 'Z' },
    {         EInput::Keyboard_M, '?' },
    {     EInput::Keyboard_Comma, '.' },
    { EInput::Keyboard_Semicolon, 'M' },
    {         EInput::Keyboard_E, 'E' },
    {         EInput::Keyboard_R, 'R' },
    {         EInput::Keyboard_T, 'T' },
    {         EInput::Keyboard_Y, 'Y' },
    {         EInput::Keyboard_U, 'U' },
    {         EInput::Keyboard_I, 'I' },
    {         EInput::Keyboard_O, 'O' },
    {         EInput::Keyboard_P, 'P' },
    {         EInput::Keyboard_S, 'S' },
    {         EInput::Keyboard_D, 'D' },
    {         EInput::Keyboard_F, 'F' },
    {         EInput::Keyboard_G, 'G' },
    {         EInput::Keyboard_H, 'H' },
    {         EInput::Keyboard_J, 'J' },
    {         EInput::Keyboard_K, 'K' },
    {         EInput::Keyboard_L, 'L' },
    {         EInput::Keyboard_X, 'X' },
    {         EInput::Keyboard_C, 'C' },
    {         EInput::Keyboard_V, 'V' },
    {         EInput::Keyboard_B, 'B' },
    {         EInput::Keyboard_N, 'N' },

    // Numbers (AZERTY shifted)
    {      EInput::Keyboard_Num1, '1' },
    {      EInput::Keyboard_Num2, '2' },
    {      EInput::Keyboard_Num3, '3' },
    {      EInput::Keyboard_Num4, '4' },
    {      EInput::Keyboard_Num5, '5' },
    {      EInput::Keyboard_Num6, '6' },
    {      EInput::Keyboard_Num7, '7' },
    {      EInput::Keyboard_Num8, '8' },
    {      EInput::Keyboard_Num9, '9' },
    {      EInput::Keyboard_Num0, '0' },

    // Numbers numpad
    {   EInput::Keyboard_Numpad1, '1' },
    {   EInput::Keyboard_Numpad2, '2' },
    {   EInput::Keyboard_Numpad3, '3' },
    {   EInput::Keyboard_Numpad4, '4' },
    {   EInput::Keyboard_Numpad5, '5' },
    {   EInput::Keyboard_Numpad6, '6' },
    {   EInput::Keyboard_Numpad7, '7' },
    {   EInput::Keyboard_Numpad8, '8' },
    {   EInput::Keyboard_Numpad9, '9' },
    {   EInput::Keyboard_Numpad0, '0' },

    // Punctuation (AZERTY shifted)
    {    EInput::Keyboard_Hyphen, ' ' },
    {     EInput::Keyboard_Equal, '+' },
    {  EInput::Keyboard_LBracket, ' ' },
    {  EInput::Keyboard_RBracket, '$' },
    {     EInput::Keyboard_Tilde, '%' },
    { EInput::Keyboard_Backslash, 'm' },
    {     EInput::Keyboard_Comma, '.' },
    {    EInput::Keyboard_Period, '/' },
    {     EInput::Keyboard_Slash, ' ' },
    {     EInput::Keyboard_Space, ' ' }
};
}   // namespace KeyboardMapping
}   // namespace tkd
