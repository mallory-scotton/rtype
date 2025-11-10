# UWidgetTextboxComponent

## Overview

`UWidgetTextboxComponent` is an interactive text input widget that allows users to type text. It combines a rectangular background with editable text, featuring automatic sizing based on content, character limits, and focus management.

**Key Features:**
- **Automatic Focus Management**: Click to focus, click elsewhere to unfocus
- **Adaptive Sizing**: Box automatically grows/shrinks as you type (optional)
- **Character Limits**: Prevent users from entering too much text
- **Visual Feedback**: Different background colors for normal vs focused states
- **Keyboard Shortcuts**: Ctrl+Backspace for word deletion
- **Min/Max Box Sizes**: Control how small or large the box can grow

**How Text Input Works:**

When the textbox is focused (user clicked on it), the engine captures keyboard input:
1. Every keypress is detected by the input system
2. Printable characters (a-z, 0-9, space, etc.) are added to the text
3. Backspace removes the last character
4. Ctrl+Backspace removes the last word
5. The box resizes if adaptive sizing is enabled
6. Background color changes to focused color
7. Text is rendered with any overflow handled gracefully

**Adaptive Sizing Explained:**

When enabled, the textbox width adjusts based on text length:
```cpp
// With SetAdaptSizeToText(true):
// Empty: Shows minBoxChars width (e.g., 8 characters wide)
// "Hi": Still shows minBoxChars (8 chars) - won't shrink smaller
// "Hello World!": Box grows to fit 12 characters
// "Very long text...": Box grows until maxBoxChars limit (e.g., 20 chars)
// Then text scrolls within the box
```

This creates a responsive input field that doesn't waste space when empty but expands to show longer input.

## Inheritance

```
UActorComponent
    └── UWidgetComponent
        └── UWidgetTextboxComponent
```

## Features

- **Text Input**: Capture keyboard input when focused
- **Adaptive Sizing**: Automatically resizes to fit content
- **Character Limits**: Set maximum text length
- **Focus Management**: Click to focus, automatic focus indicators
- **Validation**: Support for input validation and filtering
- **Uniform Scaling**: Maintains consistent appearance across resolutions
- **Visual Feedback**: Different colors for normal, focused, and disabled states

## Properties

### Inherited from UWidgetComponent
- `FVector2 m_position` - Textbox position (1920x1080 reference)
- `FVector2 m_size` - Textbox dimensions (auto-calculated or manual)
- `EWidgetAlignment m_alignment` - Anchor point
- `int m_z` - Rendering order
- `bool m_visible` - Visibility state
- `bool m_enabled` - Whether textbox accepts input
- `bool m_focused` - Whether textbox has keyboard focus

### Textbox-Specific

#### Text Properties
- `FString m_text` - Current text content
- `FVector2 m_charSize` - Character dimensions (default: varies by HUD)
- `Float32 m_spacing` - Character spacing (negative for tighter spacing)
- `FilePath m_fontPath` - Path to the bitmap font file

#### Colors
- `FColor m_backgroundColor` - Background color (default: dark gray)
- `FColor m_focusedBackgroundColor` - Background color when focused (default: darker)
- `FColor m_textColor` - Color of text (default: white)

#### Constraints
- `UInt32 m_maxCharacters` - Maximum number of characters (0 = unlimited)
- `UInt32 m_minBoxChars` - Minimum visible characters (box won't shrink below this)
- `UInt32 m_maxBoxChars` - Maximum visible characters (box won't grow beyond this)
- `bool m_adaptSizeToText` - Whether box adapts to text size (default: true)

## Methods

### Constructor

```cpp
UWidgetTextboxComponent();
```
Creates a new textbox with default settings and adaptive sizing enabled.

### Text Methods

```cpp
void SetText(const FString& text);
FString GetText() const;
```
Sets or gets the current text content.

```cpp
void SetFontPath(const FilePath& path);
```
Sets the path to the bitmap font file.

### Appearance Methods

```cpp
void SetCharacterSize(const FVector2& size);
void SetSpacing(Float32 spacing);
void SetTextColor(const FColor& color);
void SetBackgroundColor(const FColor& color);
void SetFocusedBackgroundColor(const FColor& color);
```
Customize the textbox appearance.

### Sizing Methods

```cpp
void SetAdaptSizeToText(bool adapt);
void SetMinBoxChars(UInt32 minChars);
void SetMaxBoxChars(UInt32 maxChars);
```
Control how the textbox resizes based on content.

### Constraint Methods

```cpp
void SetMaxCharacters(UInt32 maxChars);
```
Set the maximum number of characters allowed (0 = unlimited).

### Focus Methods

```cpp
void SetFocused(bool focused);
bool IsFocused() const;
```
Manually control or check focus state.

### Rendering

```cpp
void Render(IRenderer& renderer) const override;
```
Renders the textbox background and text. Called automatically by the engine.

## Usage Examples

### Basic Textbox

```cpp
// Create a textbox component
auto* textbox = CreateComponent<UWidgetTextboxComponent>();

// Set up basic properties
textbox->SetPosition(FVector2(960.0f, 400.0f));
textbox->SetAlignment(EAlignment::Center);
textbox->SetPlaceholder("Enter your name...");
textbox->SetCharacterSize(FVector2(30.0f, 15.0f));
textbox->SetSpacing(-5.0f);
textbox->SetZIndex(10);
```

### Username Input

```cpp
// Create username textbox
auto* username = CreateComponent<UWidgetTextboxComponent>();

// Set up properties
username->SetPosition(FVector2(960.0f, 400.0f));
username->SetAlignment(EAlignment::Center);
username->SetPlaceholder("Username");
username->SetMaxLength(20);
username->SetCharacterSize(FVector2(35.0f, 17.0f));
username->SetSpacing(-6.0f);

// Allow only alphanumeric characters
username->SetCharFilter([](char c) {
    return std::isalnum(c) || c == '_';
});

// Validate minimum length
username->SetValidator([](const String& text) {
    return text.length() >= 3;
});

username->SetZIndex(15);
```

### Password Input

```cpp
// Create password textbox
auto* password = CreateComponent<UWidgetTextboxComponent>();

// Set up properties
password->SetPosition(FVector2(960.0f, 500.0f));
password->SetAlignment(EAlignment::Center);
password->SetPlaceholder("Password");
password->SetMaxLength(32);
password->SetCharacterSize(FVector2(35.0f, 17.0f));
password->SetSpacing(-6.0f);

// Display asterisks instead of actual characters
// Note: This would require custom rendering support
password->SetCharFilter([](char c) {
    return std::isprint(c) && c != ' ';
});

password->SetZIndex(15);
```

### Number Input

```cpp
auto* numberBox = CreateComponent<UWidgetTextboxComponent>();
numberBox->SetPosition(FVector2(960.0f, 450.0f));
numberBox->SetAlignment(EWidgetAlignment::Center);
numberBox->SetPlaceholder("0");
numberBox->SetMaxLength(10);
numberBox->SetAdaptiveSize(false);
numberBox->SetSize(FVector2(200.0f, 50.0f));

// Allow only digits
numberBox->SetCharFilter([](char c) {
    return std::isdigit(c);
});

// Validate as integer
numberBox->SetValidator([](const String& text) {
    if (text.empty()) return true;
    try {
        int value = std::stoi(text);
        return value >= 0 && value <= 99999;
    } catch (...) {
        return false;
    }
});

numberBox->SetZIndex(15);
```

### Search Box

```cpp
auto* searchBox = CreateComponent<UWidgetTextboxComponent>();
searchBox->SetPosition(FVector2(960.0f, 100.0f));
searchBox->SetSize(FVector2(600.0f, 50.0f));
searchBox->SetAlignment(EWidgetAlignment::TopCenter);
searchBox->SetPlaceholder("Search...");
searchBox->SetAdaptiveSize(false); // Fixed size
searchBox->SetCharacterSize(FVector2(30.0f, 15.0f));
searchBox->SetSpacing(-5.0f);
searchBox->SetBgColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.9f));
searchBox->SetFocusColor(FLinearColor(0.15f, 0.15f, 0.15f, 1.0f));
searchBox->SetZIndex(10);

// Add search icon (separate image component)
auto* searchIcon = CreateComponent<UWidgetImageComponent>();
searchIcon->SetPosition(FVector2(680.0f, 100.0f));
searchIcon->SetSize(FVector2(32.0f, 32.0f));
searchIcon->SetAlignment(EWidgetAlignment::TopLeft);
searchIcon->SetTexture("Assets/Icons/search.png");
searchIcon->SetZIndex(11);
```

### Form with Multiple Fields

```cpp
class LoginForm
{
private:
    UWidgetTextboxComponent* usernameBox;
    UWidgetTextboxComponent* passwordBox;
    UWidgetButtonComponent* submitButton;
    
public:
    void BeginPlay()
    {
        // Background panel
        auto* panel = CreateComponent<UWidgetPanelComponent>();
        panel->SetPosition(FVector2(960.0f, 540.0f));
        panel->SetSize(FVector2(500.0f, 300.0f));
        panel->SetAlignment(EWidgetAlignment::Center);
        panel->SetColor(FLinearColor(0.15f, 0.15f, 0.2f, 0.95f));
        panel->SetZIndex(10);
        
        // Title
        auto* title = CreateComponent<UWidgetTextComponent>();
        title->SetPosition(FVector2(960.0f, 420.0f));
        title->SetAlignment(EWidgetAlignment::Center);
        title->SetText("LOGIN");
        title->SetCharacterSize(FVector2(50.0f, 25.0f));
        title->SetZIndex(11);
        
        // Username field
        usernameBox = CreateComponent<UWidgetTextboxComponent>();
        usernameBox->SetPosition(FVector2(960.0f, 500.0f));
        usernameBox->SetAlignment(EWidgetAlignment::Center);
        usernameBox->SetPlaceholder("Username");
        usernameBox->SetMaxLength(20);
        usernameBox->SetZIndex(11);
        
        // Password field
        passwordBox = CreateComponent<UWidgetTextboxComponent>();
        passwordBox->SetPosition(FVector2(960.0f, 580.0f));
        passwordBox->SetAlignment(EWidgetAlignment::Center);
        passwordBox->SetPlaceholder("Password");
        passwordBox->SetMaxLength(32);
        passwordBox->SetZIndex(11);
        
        // Submit button
        submitButton = CreateComponent<UWidgetButtonComponent>();
        submitButton->SetPosition(FVector2(960.0f, 660.0f));
        submitButton->SetSize(FVector2(200.0f, 50.0f));
        submitButton->SetAlignment(EWidgetAlignment::Center);
        submitButton->SetColor(FLinearColor(0.2f, 0.6f, 1.0f, 1.0f));
        submitButton->SetZIndex(11);
        
        submitButton->SetOnClick([this]() {
            OnSubmit();
        });
        
        // Add submit button label
        auto* buttonText = CreateComponent<UWidgetTextComponent>();
        buttonText->SetPosition(FVector2(960.0f, 660.0f));
        buttonText->SetAlignment(EWidgetAlignment::Center);
        buttonText->SetText("LOGIN");
        buttonText->SetZIndex(12);
    }
    
    void OnSubmit()
    {
        String username = usernameBox->GetText();
        String password = passwordBox->GetText();
        
        if (username.empty() || password.empty())
        {
            ShowError("Please fill in all fields");
            return;
        }
        
        // Process login...
        AttemptLogin(username, password);
    }
};
```

### Chat Input

```cpp
class ChatInput
{
private:
    UWidgetTextboxComponent* chatBox;
    std::vector<String> chatHistory;
    
public:
    void BeginPlay()
    {
        chatBox = CreateComponent<UWidgetTextboxComponent>();
        chatBox->SetPosition(FVector2(960.0f, 1000.0f));
        chatBox->SetSize(FVector2(1000.0f, 50.0f));
        chatBox->SetAlignment(EWidgetAlignment::BottomCenter);
        chatBox->SetPlaceholder("Type a message...");
        chatBox->SetMaxLength(200);
        chatBox->SetAdaptiveSize(false);
        chatBox->SetZIndex(20);
    }
    
    void Tick(float deltaTime) override
    {
        // Check for Enter key to send message
        if (chatBox->IsFocused() && Input::IsKeyPressed(Key::Enter))
        {
            SendMessage();
        }
    }
    
    void SendMessage()
    {
        String message = chatBox->GetText();
        if (!message.empty())
        {
            chatHistory.push_back(message);
            chatBox->Clear();
            
            // Send to server/other players...
            BroadcastMessage(message);
        }
    }
};
```

### Settings Input

```cpp
// Port number input
auto* portBox = CreateComponent<UWidgetTextboxComponent>();
portBox->SetPosition(FVector2(960.0f, 400.0f));
portBox->SetAlignment(EWidgetAlignment::Center);
portBox->SetText("8080"); // Default value
portBox->SetMaxLength(5);
portBox->SetAdaptiveSize(false);
portBox->SetSize(FVector2(150.0f, 50.0f));

// Only allow digits
portBox->SetCharFilter([](char c) {
    return std::isdigit(c);
});

// Validate port range (1-65535)
portBox->SetValidator([](const String& text) {
    if (text.empty()) return false;
    try {
        int port = std::stoi(text);
        return port >= 1 && port <= 65535;
    } catch (...) {
        return false;
    }
});
```

### Disabled Textbox

```cpp
auto* readOnlyBox = CreateComponent<UWidgetTextboxComponent>();
readOnlyBox->SetPosition(FVector2(960.0f, 300.0f));
readOnlyBox->SetAlignment(EWidgetAlignment::Center);
readOnlyBox->SetText("Read Only Value");
readOnlyBox->SetEnabled(false); // Cannot edit
readOnlyBox->SetBgColor(FLinearColor(0.3f, 0.3f, 0.3f, 0.5f)); // Gray out
readOnlyBox->SetTextColor(FLinearColor(0.6f, 0.6f, 0.6f, 1.0f)); // Gray text
```

## Best Practices

1. **Clear Labeling**: Use text components to label what the textbox is for
2. **Character Limits**: Set reasonable max characters to prevent overflow
3. **Visual Feedback**: Use different background colors for normal and focused states
4. **Box Sizing**: Use adaptive sizing for dynamic content, fixed for forms (SetAdaptSizeToText)
5. **Min/Max Box**: Set minBoxChars and maxBoxChars to control visible area
6. **Focus Management**: Click to focus, click elsewhere to unfocus
7. **Accessibility**: Ensure adequate contrast and readable character sizes
8. **Font Consistency**: Use the same font as other text components in your UI

## Common Issues

**Problem**: Text doesn't appear when typing
- **Solution**: Check that textbox has focus (click it first) and is enabled

**Problem**: Can't type certain characters
- **Solution**: Check char filter - it may be blocking those characters

**Problem**: Text gets cut off
- **Solution**: Either increase textbox size or enable adaptive sizing with SetAdaptSizeToText(true)

**Problem**: Box doesn't resize with text
- **Solution**: Ensure SetAdaptSizeToText(true) is called, and check minBoxChars/maxBoxChars settings

**Problem**: Textbox doesn't scale correctly
- **Solution**: Verify character size and spacing use the reference resolution (1920x1080)

## See Also

- [Widget Component](Widget%20Component.md) - Base widget class with input documentation
- [Widget Text Component](Widget%20Text%20Component.md) - For display-only text
- [Widget Button Component](Widget%20Button%20Component.md) - For submit buttons
- [Widget Panel Component](Widget%20Panel%20Component.md) - For textbox backgrounds
