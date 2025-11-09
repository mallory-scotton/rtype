# UWidgetTextComponent

## Overview

`UWidgetTextComponent` displays text in the UI with automatic scaling and alignment. It uses bitmap fonts and supports customizable character size, spacing, and color. Text maintains consistent proportions across different screen resolutions using uniform scaling.

## Inheritance

```
UActorComponent
    └── UWidgetComponent
        └── UWidgetTextComponent
```

## Features

- **Bitmap Font Rendering**: Displays text using bitmap font textures
- **Uniform Scaling**: Text maintains aspect ratio and readability at all resolutions
- **Flexible Alignment**: Position text using the 9-point alignment system
- **Customizable Appearance**: Control character size, spacing, and color
- **Automatic Wrapping**: (If supported by renderer) Text can wrap to multiple lines

## Properties

### Inherited from UWidgetComponent
- `FVector2 m_position` - Text position (1920x1080 reference)
- `EWidgetAlignment m_alignment` - Anchor point for text positioning
- `int m_z` - Rendering order
- `bool m_visible` - Visibility state

### Text-Specific
- `FString m_text` - The text content to display
- `FVector2 m_charSize` - Size of each character in reference coordinates (default: varies by HUD)
- `Float32 m_spacing` - Spacing between characters (negative for tighter spacing)
- `FColor m_color` - Text color (default: white)
- `FilePath m_fontPath` - Path to the bitmap font file

## Methods

### Constructor

```cpp
UWidgetTextComponent();
```
Creates a new text widget with default settings.

### Setters

```cpp
void SetText(const FString& text);
```
Sets the text content to display.

```cpp
void SetCharacterSize(const FVector2& size);
```
Sets the character dimensions in reference resolution units.

```cpp
void SetSpacing(Float32 spacing);
```
Sets the spacing between characters. Use negative values for tighter spacing.

```cpp
void SetColor(const FColor& color);
```
Sets the text color.

```cpp
void SetFontPath(const FilePath& path);
```
Sets the path to the bitmap font file to use for rendering.

### Rendering

```cpp
void Render(IRenderer& renderer) const override;
```
Renders the text with uniform scaling. Called automatically by the engine.

## Usage Examples

### Basic Text

```cpp
auto* text = CreateComponent<UWidgetTextComponent>();
text->SetPosition(FVector2(960.0f, 100.0f));
text->SetAlignment(EWidgetAlignment::TopCenter);
text->SetText("Hello, World!");
text->SetCharacterSize(FVector2(40.0f, 20.0f));
text->SetSpacing(-8.0f);
text->SetColor(FColor(255, 255, 255, 255)); // White
text->SetZIndex(10);
```

### Score Display

```cpp
class ScoreDisplay
{
private:
    UWidgetTextComponent* scoreText;
    int currentScore = 0;
    
public:
    void BeginPlay()
    {
        scoreText = CreateComponent<UWidgetTextComponent>();
        scoreText->SetPosition(FVector2(100.0f, 50.0f));
        scoreText->SetAlignment(EWidgetAlignment::TopLeft);
        scoreText->SetCharacterSize(FVector2(50.0f, 25.0f));
        scoreText->SetSpacing(-10.0f);
        scoreText->SetColor(FColor(255, 255, 0, 255)); // Yellow
        scoreText->SetZIndex(20);
        UpdateScore(0);
    }
    
    void UpdateScore(int score)
    {
        currentScore = score;
        scoreText->SetText("SCORE: " + std::to_string(currentScore));
    }
    
    void AddScore(int points)
    {
        UpdateScore(currentScore + points);
    }
};
```

### Title Text

```cpp
auto* title = CreateComponent<UWidgetTextComponent>();
title->SetPosition(FVector2(960.0f, 200.0f));
title->SetAlignment(EWidgetAlignment::TopCenter);
title->SetText("GAME TITLE");
title->SetCharacterSize(FVector2(80.0f, 40.0f)); // Large text
title->SetSpacing(-15.0f);
title->SetColor(FColor(255, 128, 0, 255)); // Orange
title->SetZIndex(15);
```

### Subtitle Text

```cpp
auto* subtitle = CreateComponent<UWidgetTextComponent>();
subtitle->SetPosition(FVector2(960.0f, 300.0f));
subtitle->SetAlignment(EWidgetAlignment::TopCenter);
subtitle->SetText("Press any key to start");
subtitle->SetCharacterSize(FVector2(30.0f, 15.0f)); // Smaller text
subtitle->SetSpacing(-5.0f);
subtitle->SetColor(FColor(204, 204, 204, 255)); // Light gray
subtitle->SetZIndex(15);
```

### Colored Labels

```cpp
// Create a text component for Player 1
auto* p1Label = CreateComponent<UWidgetTextComponent>();

// Player 1 label - Red
p1Label->SetPosition(FVector2(200.0f, 100.0f));
p1Label->SetAlignment(EAlignment::TopLeft);
p1Label->SetText("P1");
p1Label->SetCharacterSize(FVector2(40.0f, 20.0f));
p1Label->SetColor(FColor(255, 0, 0, 255)); // Red

// Create a text component for Player 2
auto* p2Label = CreateComponent<UWidgetTextComponent>();

// Player 2 label - Blue
p2Label->SetPosition(FVector2(1720.0f, 100.0f));
p2Label->SetAlignment(EAlignment::TopRight);
p2Label->SetText("P2");
p2Label->SetCharacterSize(FVector2(40.0f, 20.0f));
p2Label->SetColor(FColor(0, 128, 255, 255)); // Blue
```

### Dynamic Text

```cpp
class Timer
{
private:
    UWidgetTextComponent* timerText;
    float elapsedTime = 0.0f;
    
public:
    void BeginPlay()
    {
        timerText = CreateComponent<UWidgetTextComponent>();
        timerText->SetPosition(FVector2(960.0f, 50.0f));
        timerText->SetAlignment(EWidgetAlignment::TopCenter);
        timerText->SetCharacterSize(FVector2(45.0f, 22.0f));
        timerText->SetSpacing(-8.0f);
        timerText->SetColor(FColor(255, 255, 255, 255));
        timerText->SetZIndex(20);
    }
    
    void Tick(float deltaTime) override
    {
        elapsedTime += deltaTime;
        int minutes = static_cast<int>(elapsedTime) / 60;
        int seconds = static_cast<int>(elapsedTime) % 60;
        
        // Format as MM:SS
        String timeStr = (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
                        (seconds < 10 ? "0" : "") + std::to_string(seconds);
        timerText->SetText(timeStr);
    }
};
```

### Multi-Line Text (Manual)

```cpp
// Create multiple text components for multi-line display
void CreateMultiLineText(const FVector2& startPos, 
                        const std::vector<String>& lines,
                        float lineHeight)
{
    for (size_t i = 0; i < lines.size(); ++i)
    {
        auto* line = CreateComponent<UWidgetTextComponent>();
        line->SetPosition(FVector2(startPos.x, startPos.y + (i * lineHeight)));
        line->SetAlignment(EWidgetAlignment::TopLeft);
        line->SetText(lines[i]);
        line->SetCharacterSize(FVector2(30.0f, 15.0f));
        line->SetSpacing(-5.0f);
        line->SetZIndex(10);
    }
}

// Usage
CreateMultiLineText(
    FVector2(100.0f, 200.0f),
    {
        "Line 1: First line of text",
        "Line 2: Second line of text",
        "Line 3: Third line of text"
    },
    40.0f // Line spacing
);
```

### Animated Text

```cpp
class PulsingText
{
private:
    UWidgetTextComponent* text;
    float pulseTime = 0.0f;
    
public:
    void BeginPlay()
    {
        text = CreateComponent<UWidgetTextComponent>();
        text->SetPosition(FVector2(960.0f, 540.0f));
        text->SetAlignment(EWidgetAlignment::Center);
        text->SetText("GAME OVER");
        text->SetCharacterSize(FVector2(60.0f, 30.0f));
        text->SetZIndex(50);
    }
    
    void Tick(float deltaTime) override
    {
        pulseTime += deltaTime * 2.0f;
        
        // Pulsing alpha
        float alpha = 0.5f + 0.5f * std::sin(pulseTime);
        text->SetColor(FColor(255, 0, 0, alpha));
    }
};
```

### Combo Counter

```cpp
class ComboDisplay
{
private:
    UWidgetTextComponent* comboText;
    int comboCount = 0;
    float comboTimer = 0.0f;
    const float comboTimeout = 3.0f;
    
public:
    void BeginPlay()
    {
        comboText = CreateComponent<UWidgetTextComponent>();
        comboText->SetPosition(FVector2(960.0f, 400.0f));
        comboText->SetAlignment(EWidgetAlignment::Center);
        comboText->SetCharacterSize(FVector2(50.0f, 25.0f));
        comboText->SetSpacing(-10.0f);
        comboText->SetZIndex(30);
        comboText->SetVisible(false);
    }
    
    void AddCombo()
    {
        comboCount++;
        comboTimer = comboTimeout;
        
        comboText->SetText(std::to_string(comboCount) + "x COMBO!");
        
        // Color based on combo level
        if (comboCount < 5)
            comboText->SetColor(FColor(255, 255, 255, 255)); // White
        else if (comboCount < 10)
            comboText->SetColor(FColor(255, 255, 0, 255)); // Yellow
        else
            comboText->SetColor(FColor(255, 0, 0, 255)); // Red
        
        comboText->SetVisible(true);
    }
    
    void Tick(float deltaTime) override
    {
        if (comboTimer > 0.0f)
        {
            comboTimer -= deltaTime;
            if (comboTimer <= 0.0f)
            {
                comboCount = 0;
                comboText->SetVisible(false);
            }
        }
    }
};
```

## Character Size Guidelines

Common character sizes for 1920x1080 reference resolution:

- **Large titles**: 80x40 to 100x50
  - Use for: Game titles, level names, big announcements
  - Readability: Visible from across the room
  
- **Medium titles**: 60x30 to 70x35
  - Use for: Menu headers, section titles, "GAME OVER"
  - Readability: Clear at normal viewing distance
  
- **Regular text**: 40x20 to 50x25
  - Use for: HUD elements, score displays, button labels
  - Readability: Standard UI text, readable without strain
  
- **Small text**: 30x15 to 35x17
  - Use for: Subtitles, tooltips, minor information
  - Readability: Requires attention but still clear
  
- **Tiny text**: 20x10 to 25x12
  - Use for: Copyright notices, version numbers, debug info
  - Readability: Minimum legible size, use sparingly

**Important Notes:**

1. **Aspect Ratios**: Notice all these are roughly 2:1 aspect ratio (width ≈ 2x height). This matches typical bitmap font proportions. If your font has different proportions, adjust accordingly.

2. **Pixel Perfect at Reference**: At 1920x1080, these are exact pixel sizes. On a 1080p monitor, "40x20" means literally 40 pixels wide by 20 pixels tall per character.

3. **Testing on Target Hardware**: Always test on your minimum supported resolution. Text that looks fine at 1080p might be too small at 720p. If supporting 720p, consider using slightly larger base sizes (e.g., 45x22 instead of 40x20).

4. **Accessibility**: For accessibility compliance, body text should typically be at least 40x20. Smaller sizes may be difficult for users with visual impairments.

## Spacing Guidelines

- **Tight spacing**: -15.0 to -10.0 (characters closer together)
  - Use for: Stylized titles, logo text, when you want a compact look
  - Example: "GAME TITLE" with heavy overlap for dramatic effect
  
- **Normal spacing**: -8.0 to -5.0 (recommended for most text)
  - Use for: HUD elements, scores, general UI text
  - This is the sweet spot - readable but not wasteful of space
  
- **Loose spacing**: -3.0 to 0.0 (more space between characters)
  - Use for: When you want emphasis, or text feels cramped
  - Can make text feel more relaxed and easier to scan
  
- **Wide spacing**: 0.0 to 5.0 (significant gaps)
  - Use for: Special effects, artistic choices
  - Example: "S P A C E D   O U T" for dramatic emphasis

**Why Negative Spacing?**

You might wonder why all these values are negative. Bitmap fonts often have built-in padding around each character. Without negative spacing, text can look too spread out:

```
With 0 spacing:     "H  E  L  L  O"
With -8 spacing:    "HELLO"
```

The negative value overlaps characters slightly, removing the padding and creating normal-looking text. The exact value depends on your font file's padding.

**Finding the Right Spacing:**

1. Start with -8.0 as a baseline
2. If text feels too cramped, reduce to -5.0 or -3.0
3. If text feels too spaced out, increase to -10.0 or -12.0
4. Different fonts need different spacing - experiment!

**Pro Tip:** Use the same spacing value for all text of the same size throughout your UI. This creates visual consistency. For example:
```cpp
// Constants for your game
const float TITLE_SPACING = -12.0f;
const float UI_SPACING = -8.0f;
const float SUBTITLE_SPACING = -5.0f;
```

## Scaling Behavior

Text uses **uniform scaling** to maintain readability and aspect ratio:

```cpp
float uniformScale = GetUniformScale();
FVector2 scaledCharSize = m_charSize * uniformScale;
float scaledSpacing = m_spacing * uniformScale;
```

**Why Uniform Scaling is Critical for Text:**

Text must use uniform scaling, not independent X/Y scaling. If you scaled text width and height independently:
- **At 21:9 ultrawide**: Text would be horizontally stretched and look weird
- **At 4:3 monitor**: Text would be vertically stretched and become unreadable
- **Character spacing would be wrong**: Letters might overlap or have incorrect gaps

With uniform scaling, text looks identical at all resolutions - just larger or smaller. A character that's 40x20 at 1920x1080 becomes 26.7x13.3 at 1280x720, maintaining the exact same 2:1 aspect ratio.

**Spacing Behavior:**

Character spacing is also uniformly scaled. If you set spacing to -8.0, this means:
- At 1920x1080: Characters overlap by 8 pixels (negative spacing tightens text)
- At 1280x720: Characters overlap by ~5.3 pixels (scaled proportionally)
- At 3840x2160: Characters overlap by 16 pixels (scaled up 2x)

The visual tightness remains consistent across resolutions.

**Example at different resolutions:**
```
Reference (1920x1080):
- Character size: 40x20 pixels
- Spacing: -8.0 pixels
- Text "HELLO" width: (40-8)*5 = 160 pixels

At 1280x720 (~0.67x scale):
- Character size: ~27x13 pixels  
- Spacing: ~-5.3 pixels
- Text "HELLO" width: (27-5.3)*5 = ~107 pixels (0.67x of 160 ✓)

At 3840x2160 (2.0x scale):
- Character size: 80x40 pixels
- Spacing: -16 pixels
- Text "HELLO" width: (80-16)*5 = 320 pixels (2x of 160 ✓)
```

## Color Examples

```cpp
// Basic colors
text->SetColor(FColor(255, 255, 255, 255)); // White
text->SetColor(FColor(0, 0, 0, 255));       // Black
text->SetColor(FColor(255, 0, 0, 255));     // Red
text->SetColor(FColor(0, 255, 0, 255));     // Green
text->SetColor(FColor(0, 0, 255, 255));     // Blue

// UI colors
text->SetColor(FColor(255, 255, 0, 255));   // Yellow (scores, warnings)
text->SetColor(FColor(255, 128, 0, 255));   // Orange (titles, alerts)
text->SetColor(FColor(128, 128, 128, 255)); // Gray (disabled/secondary)
text->SetColor(FColor(0, 255, 255, 255));   // Cyan (info, hints)

// Semi-transparent
text->SetColor(FColor(255, 255, 255, 178)); // 70% opacity
```

## Best Practices

1. **Readability First**: Choose character sizes large enough to read comfortably
2. **Contrast**: Ensure text color contrasts with background (white on dark, dark on light)
3. **Consistent Sizing**: Use consistent character sizes for similar text types
4. **Negative Spacing**: Most text looks better with negative spacing (-5 to -10)
5. **Alignment**: Use alignment to position text relative to other UI elements
6. **Z-Index**: Text should typically have higher z-index than backgrounds
7. **Dynamic Updates**: Update text in Tick() or event handlers, not in Render()
8. **String Formatting**: Use std::to_string() for numbers, string concatenation for labels

## Common Patterns

### Health Bar with Text

```cpp
// Background panel
auto* healthBg = CreateComponent<UWidgetPanelComponent>();
healthBg->SetPosition(FVector2(100.0f, 50.0f));
healthBg->SetSize(FVector2(200.0f, 30.0f));
healthBg->SetColor(FLinearColor(0.2f, 0.2f, 0.2f, 0.8f));
healthBg->SetZIndex(10);

// Health fill
auto* healthFill = CreateComponent<UWidgetPanelComponent>();
healthFill->SetPosition(FVector2(100.0f, 50.0f));
healthFill->SetSize(FVector2(200.0f, 30.0f)); // Will be adjusted
healthFill->SetColor(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f));
healthFill->SetZIndex(11);

// Health text
auto* healthText = CreateComponent<UWidgetTextComponent>();
healthText->SetPosition(FVector2(200.0f, 65.0f)); // Centered on bar
healthText->SetAlignment(EWidgetAlignment::Center);
healthText->SetCharacterSize(FVector2(25.0f, 12.0f));
healthText->SetSpacing(-5.0f);
healthText->SetColor(FColor(255, 255, 255, 255));
healthText->SetZIndex(12);

// Update health
void SetHealth(int current, int max)
{
    float percentage = static_cast<float>(current) / max;
    healthFill->SetSize(FVector2(200.0f * percentage, 30.0f));
    healthText->SetText(std::to_string(current) + "/" + std::to_string(max));
}
```

## See Also

- [Widget Component](Widget%20Component.md) - Base widget class with scaling documentation
- [Widget Textbox Component](Widget%20Textbox%20Component.md) - For text input
- [Widget Panel Component](Widget%20Panel%20Component.md) - For text backgrounds
- [UFont](../Assets/URessource.md) - Font asset documentation
