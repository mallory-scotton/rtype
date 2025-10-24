# HUD Documentation

## Overview

The `AHUD` class is a specialized actor that serves as the foundation for Heads-Up Display (HUD) systems in the TKD Game Engine. As a base class that inherits from `AActor`, it provides the infrastructure for creating user interface elements that overlay the game world, including health bars, mini-maps, crosshairs, menus, and other 2D interface components.

### Key Features

- **Actor Integration**: Inherits from `AActor` for full actor system integration
- **Component-Based UI**: Modular UI construction using actor components
- **Screen Space Rendering**: 2D overlay rendering independent of 3D world
- **Input Handling**: UI interaction and input processing capabilities
- **Layer Management**: Z-ordering and layering of UI elements
- **Resolution Independence**: Automatic scaling and positioning
- **Event System**: UI event handling and user interaction
- **Styling System**: Consistent visual appearance and theming

### Architecture

```cpp
class AHUD : public AActor
{
public:
    AHUD(void);
};
```

The `AHUD` provides a minimal base class that leverages the full power of the `AActor` system:

- **Component System**: Attach UI components for different interface elements
- **Lifecycle Management**: `BeginPlay()`, `Tick()`, `EndPlay()` for UI initialization and updates
- **Networking**: HUD state synchronization across clients
- **Property System**: Replicated UI state and configuration
- **Event System**: UI interaction events and callbacks

## Core Components

### UI Component System

HUD functionality is implemented through specialized components attached to the HUD actor:

```cpp
// Example HUD components
class UTextComponent : public UActorComponent {
    UProperty<FString> text;
    UProperty<FFont> font;
    UProperty<FColor> color;
    UProperty<FVector2> position;
};

class UImageComponent : public UActorComponent {
    UProperty<UTexture> texture;
    UProperty<FVector2> position;
    UProperty<FVector2> size;
    UProperty<FColor> tint;
};

class UProgressBarComponent : public UActorComponent {
    UProperty<Float32> progress;  // 0.0 to 1.0
    UProperty<FVector2> position;
    UProperty<FVector2> size;
    UProperty<FColor> fillColor;
    UProperty<FColor> backgroundColor;
};
```

### Coordinate System

HUD uses screen-space coordinates:

- **Origin**: Top-left corner (0,0)
- **X-Axis**: Increases rightward
- **Y-Axis**: Increases downward
- **Units**: Pixels or normalized coordinates (0.0-1.0)
- **Resolution Independence**: Automatic scaling based on viewport

### Layer System

UI elements are organized in layers for proper rendering order:

```cpp
enum class EHUDLayer {
    Background = 0,    // Background elements
    Main = 100,        // Primary UI elements
    Overlay = 200,     // Modal dialogs, tooltips
    Cursor = 300       // Mouse cursor, drag elements
};
```

## API Reference

### Constructor & Destructor

#### `AHUD(void)`

Creates a new HUD actor instance.

**Behavior:**
- Initializes the HUD with default name "AHUD"
- Sets up basic HUD infrastructure
- Registers with the UI rendering system

### Inherited Methods

Since `AHUD` inherits from `AActor`, it has access to all actor functionality:

- **Lifecycle**: `BeginPlay()`, `Tick(Float32 deltaTime)`, `EndPlay()`
- **Component Management**: `AddComponent<T>()`, `GetComponent<T>()`, `RemoveComponent()`
- **Transform**: `GetTransform()`, `SetTransform()` (typically unused for HUD)
- **State Management**: `IsActive()`, `SetActive()`, `MarkForDeletion()`
- **Networking**: Property replication and RPC support

### HUD-Specific Methods

HUD functionality is typically implemented through components and custom methods:

```cpp
class AGameHUD : public AHUD {
public:
    virtual void BeginPlay() override {
        AHUD::BeginPlay();
        InitializeUI();
    }

    virtual void Tick(Float32 deltaTime) override {
        AHUD::Tick(deltaTime);
        UpdateUI();
    }

    // HUD-specific methods
    void ShowDamageIndicator(const FVector3& worldPosition, Float32 damage);
    void UpdateHealthBar(Float32 currentHealth, Float32 maxHealth);
    void ShowMessage(const FString& message, Float32 duration);
    void ToggleMinimap(bool visible);
};
```

## Usage Examples

### Basic HUD Setup

```cpp
class AGameHUD : public AHUD
{
public:
    virtual void BeginPlay() override
    {
        AHUD::BeginPlay();

        // Create health bar
        healthBar = AddComponent<UProgressBarComponent>("HealthBar");
        healthBar->position = FVector2(20, 20);
        healthBar->size = FVector2(200, 30);
        healthBar->fillColor = FColor::Green;
        healthBar->backgroundColor = FColor::Red;

        // Create crosshair
        crosshair = AddComponent<UImageComponent>("Crosshair");
        crosshair->texture = GetTexture("crosshair.png");
        crosshair->position = FVector2(0.5f, 0.5f); // Center of screen
        crosshair->size = FVector2(32, 32);
        crosshair->anchor = EAnchor::Center;

        // Create ammo counter
        ammoText = AddComponent<UTextComponent>("AmmoText");
        ammoText->text = "30/30";
        ammoText->font = GetFont("hud_font.ttf", 24);
        ammoText->color = FColor::White;
        ammoText->position = FVector2(20, 60);
    }

    void UpdateHealth(Float32 current, Float32 max)
    {
        if (healthBar) {
            healthBar->progress = current / max;
            healthBar->fillColor = (current > max * 0.3f) ? FColor::Green : FColor::Red;
        }
    }

    void UpdateAmmo(Int32 current, Int32 max)
    {
        if (ammoText) {
            ammoText->text = FString::Format("{}/{}", current, max);
        }
    }

private:
    UProgressBarComponent* healthBar;
    UImageComponent* crosshair;
    UTextComponent* ammoText;
};
```

### Advanced HUD with Minimap

```cpp
class AAdvancedHUD : public AHUD
{
public:
    virtual void BeginPlay() override
    {
        AHUD::BeginPlay();

        // Initialize all UI components
        InitializeHealthSystem();
        InitializeWeaponSystem();
        InitializeMinimap();
        InitializeObjectives();
        InitializeDamageIndicators();
    }

    virtual void Tick(Float32 deltaTime) override
    {
        AHUD::Tick(deltaTime);

        UpdateMinimap();
        UpdateDamageIndicators(deltaTime);
        UpdateObjectives();
    }

private:
    void InitializeHealthSystem()
    {
        // Main health bar
        healthBar = AddComponent<UProgressBarComponent>("HealthBar");
        healthBar->position = FVector2(50, GetViewportHeight() - 100);
        healthBar->size = FVector2(300, 40);
        healthBar->progress = 1.0f;

        // Health text
        healthText = AddComponent<UTextComponent>("HealthText");
        healthText->position = FVector2(50, GetViewportHeight() - 140);
        healthText->text = "100/100 HP";
        healthText->font = GetFont("hud_font.ttf", 20);
    }

    void InitializeWeaponSystem()
    {
        // Weapon info panel
        weaponPanel = AddComponent<UImageComponent>("WeaponPanel");
        weaponPanel->texture = GetTexture("weapon_panel.png");
        weaponPanel->position = FVector2(GetViewportWidth() - 250, GetViewportHeight() - 150);
        weaponPanel->size = FVector2(200, 100);

        // Ammo counter
        ammoCounter = AddComponent<UTextComponent>("AmmoCounter");
        ammoCounter->position = FVector2(GetViewportWidth() - 200, GetViewportHeight() - 120);
        ammoCounter->text = "30/30";
        ammoCounter->font = GetFont("hud_font.ttf", 24);
        ammoCounter->color = FColor::Yellow;
    }

    void InitializeMinimap()
    {
        minimap = AddComponent<UMinimapComponent>("Minimap");
        minimap->position = FVector2(GetViewportWidth() - 200, 50);
        minimap->size = FVector2(150, 150);
        minimap->worldBounds = FRect(-1000, -1000, 2000, 2000);
        minimap->zoomLevel = 1.0f;

        // Minimap toggle button
        minimapToggle = AddComponent<UButtonComponent>("MinimapToggle");
        minimapToggle->position = FVector2(GetViewportWidth() - 200, 210);
        minimapToggle->size = FVector2(150, 30);
        minimapToggle->text = "Toggle Minimap";
        minimapToggle->OnClicked.Add([this]() {
            minimap->SetVisible(!minimap->IsVisible());
        });
    }

    void InitializeObjectives()
    {
        objectivesPanel = AddComponent<UTextComponent>("Objectives");
        objectivesPanel->position = FVector2(50, 50);
        objectivesPanel->text = "OBJECTIVES:\n- Find the treasure\n- Avoid enemies\n- Reach the exit";
        objectivesPanel->font = GetFont("hud_font.ttf", 16);
        objectivesPanel->color = FColor::Cyan;
    }

    void InitializeDamageIndicators()
    {
        // Damage indicators pool
        for (int i = 0; i < 10; ++i) {
            UDamageIndicatorComponent* indicator = AddComponent<UDamageIndicatorComponent>(
                FString::Format("DamageIndicator{}", i));
            indicator->SetVisible(false);
            damageIndicators.push_back(indicator);
        }
    }

    void UpdateMinimap()
    {
        if (!minimap || !minimap->IsVisible()) return;

        // Update player position on minimap
        APlayerController* player = GetPlayerController();
        if (player) {
            FVector3 playerPos = player->GetPawn()->GetTransform().GetPosition();
            minimap->SetPlayerPosition(FVector2(playerPos.x, playerPos.z));
        }

        // Update enemy positions
        minimap->ClearMarkers();
        for (AEnemy* enemy : GetEnemies()) {
            FVector3 enemyPos = enemy->GetTransform().GetPosition();
            minimap->AddMarker(FVector2(enemyPos.x, enemyPos.z), FColor::Red);
        }
    }

    void UpdateDamageIndicators(Float32 deltaTime)
    {
        for (UDamageIndicatorComponent* indicator : damageIndicators) {
            if (indicator->IsVisible()) {
                indicator->Update(deltaTime);
            }
        }
    }

    void UpdateObjectives()
    {
        // Update objective completion status
        FString objectivesText = "OBJECTIVES:\n";

        if (hasTreasure) objectivesText += "- [X] Find the treasure\n";
        else objectivesText += "- [ ] Find the treasure\n";

        if (avoidedEnemies) objectivesText += "- [X] Avoid enemies\n";
        else objectivesText += "- [ ] Avoid enemies\n";

        if (reachedExit) objectivesText += "- [X] Reach the exit\n";
        else objectivesText += "- [ ] Reach the exit\n";

        objectivesPanel->text = objectivesText;
    }

    void ShowDamage(Float32 damage, const FVector3& worldPosition)
    {
        // Find available damage indicator
        for (UDamageIndicatorComponent* indicator : damageIndicators) {
            if (!indicator->IsVisible()) {
                indicator->ShowDamage(damage, worldPosition);
                break;
            }
        }
    }

public:
    void OnPlayerDamaged(Float32 damage, const FVector3& damageSource)
    {
        ShowDamage(damage, damageSource);
        UpdateHealth(currentHealth, maxHealth);
    }

    void OnAmmoChanged(Int32 current, Int32 max)
    {
        UpdateAmmo(current, max);
    }

    void OnObjectiveCompleted(EObjectiveType objective)
    {
        switch (objective) {
            case EObjectiveType::FindTreasure: hasTreasure = true; break;
            case EObjectiveType::AvoidEnemies: avoidedEnemies = true; break;
            case EObjectiveType::ReachExit: reachedExit = true; break;
        }
        UpdateObjectives();
    }

private:
    // UI Components
    UProgressBarComponent* healthBar;
    UTextComponent* healthText;
    UImageComponent* weaponPanel;
    UTextComponent* ammoCounter;
    UMinimapComponent* minimap;
    UButtonComponent* minimapToggle;
    UTextComponent* objectivesPanel;
    TVector<UDamageIndicatorComponent*> damageIndicators;

    // Game state
    Float32 currentHealth = 100.0f;
    Float32 maxHealth = 100.0f;
    bool hasTreasure = false;
    bool avoidedEnemies = true;
    bool reachedExit = false;
};
```

### Dynamic HUD Scaling

```cpp
class AResponsiveHUD : public AHUD
{
public:
    virtual void BeginPlay() override
    {
        AHUD::BeginPlay();
        UpdateUIScaling();
    }

    void OnViewportResized(UInt32 newWidth, UInt32 newHeight)
    {
        viewportWidth = newWidth;
        viewportHeight = newHeight;
        UpdateUIScaling();
    }

private:
    void UpdateUIScaling()
    {
        // Calculate scaling factors
        Float32 scaleX = viewportWidth / referenceWidth;
        Float32 scaleY = viewportHeight / referenceHeight;
        Float32 uniformScale = FMath::Min(scaleX, scaleY);

        // Scale UI elements
        for (UActorComponent* component : GetComponents<UActorComponent>()) {
            if (UUIComponent* uiComponent = dynamic_cast<UUIComponent*>(component)) {
                uiComponent->SetScale(uniformScale);
                uiComponent->UpdateAnchoredPosition(viewportWidth, viewportHeight);
            }
        }
    }

    // Reference resolution (designed for 1920x1080)
    static constexpr Float32 referenceWidth = 1920.0f;
    static constexpr Float32 referenceHeight = 1080.0f;

    UInt32 viewportWidth = 1920;
    UInt32 viewportHeight = 1080;
};
```

### Interactive HUD with Menus

```cpp
class AMenuHUD : public AHUD
{
public:
    virtual void BeginPlay() override
    {
        AHUD::BeginPlay();
        CreateMainMenu();
    }

    void ShowPauseMenu()
    {
        if (pauseMenu) {
            pauseMenu->SetVisible(true);
            GetPlayerController()->SetInputMode(EInputMode::UI);
        }
    }

    void HidePauseMenu()
    {
        if (pauseMenu) {
            pauseMenu->SetVisible(false);
            GetPlayerController()->SetInputMode(EInputMode::Game);
        }
    }

private:
    void CreateMainMenu()
    {
        // Background panel
        menuBackground = AddComponent<UImageComponent>("MenuBackground");
        menuBackground->texture = GetTexture("menu_background.png");
        menuBackground->position = FVector2(0.5f, 0.5f);
        menuBackground->size = FVector2(400, 600);
        menuBackground->anchor = EAnchor::Center;
        menuBackground->color = FColor(0, 0, 0, 0.8f);

        // Title
        menuTitle = AddComponent<UTextComponent>("MenuTitle");
        menuTitle->text = "GAME PAUSED";
        menuTitle->font = GetFont("title_font.ttf", 36);
        menuTitle->color = FColor::White;
        menuTitle->position = FVector2(0.5f, 0.3f);
        menuTitle->anchor = EAnchor::Center;

        // Buttons
        resumeButton = CreateMenuButton("Resume", FVector2(0.5f, 0.4f), [this]() {
            HidePauseMenu();
        });

        settingsButton = CreateMenuButton("Settings", FVector2(0.5f, 0.5f), [this]() {
            ShowSettingsMenu();
        });

        quitButton = CreateMenuButton("Quit", FVector2(0.5f, 0.6f), [this]() {
            GetGameInstance()->QuitGame();
        });

        // Initially hidden
        menuBackground->SetVisible(false);
        menuTitle->SetVisible(false);
        resumeButton->SetVisible(false);
        settingsButton->SetVisible(false);
        quitButton->SetVisible(false);
    }

    UButtonComponent* CreateMenuButton(const FString& text, const FVector2& position,
                                     std::function<void()> callback)
    {
        UButtonComponent* button = AddComponent<UButtonComponent>(
            FString::Format("Button_{}", text));

        button->text = text;
        button->position = position;
        button->size = FVector2(200, 50);
        button->anchor = EAnchor::Center;
        button->normalColor = FColor(0.2f, 0.2f, 0.2f, 1.0f);
        button->hoverColor = FColor(0.4f, 0.4f, 0.4f, 1.0f);
        button->pressedColor = FColor(0.6f, 0.6f, 0.6f, 1.0f);

        button->OnClicked.Add(callback);

        return button;
    }

    void ShowSettingsMenu()
    {
        // Hide main menu
        menuBackground->SetVisible(false);
        menuTitle->SetVisible(false);
        resumeButton->SetVisible(false);
        settingsButton->SetVisible(false);
        quitButton->SetVisible(false);

        // Show settings menu
        // (Implementation for settings menu)
    }

private:
    UImageComponent* menuBackground;
    UTextComponent* menuTitle;
    UButtonComponent* resumeButton;
    UButtonComponent* settingsButton;
    UButtonComponent* quitButton;
};
```

## UI System Integration

### Component Architecture

HUD components provide specialized UI functionality:

```cpp
class UUIComponent : public UActorComponent {
public:
    UProperty<FVector2> position;        // Position (absolute or normalized)
    UProperty<FVector2> size;           // Size in pixels
    UProperty<EAnchor> anchor;          // Anchoring point
    UProperty<Int32> layer;             // Rendering layer
    UProperty<bool> visible;            // Visibility flag

    virtual void Render(UCanvas* canvas) = 0;
    virtual bool HandleInput(const FInputEvent& event) { return false; }

    FRect GetBounds() const;
    bool ContainsPoint(const FVector2& point) const;
};

class UCanvas {
public:
    void DrawText(const FString& text, const FVector2& position, const FFont& font, const FColor& color);
    void DrawImage(UTexture* texture, const FRect& rect, const FColor& tint = FColor::White);
    void DrawRect(const FRect& rect, const FColor& color);
    void DrawLine(const FVector2& start, const FVector2& end, const FColor& color, Float32 thickness = 1.0f);

    FVector2 GetViewportSize() const;
    Float32 GetDPIScale() const;
};
```

### Input Handling

HUD components can handle user input:

```cpp
class UButtonComponent : public UUIComponent {
public:
    UFunction<> OnClicked;
    UFunction<> OnHovered;
    UFunction<> OnPressed;

    virtual bool HandleInput(const FInputEvent& event) override {
        FVector2 mousePos = event.GetMousePosition();

        if (ContainsPoint(mousePos)) {
            if (event.GetType() == EInputEventType::MouseButtonPressed &&
                event.GetMouseButton() == EMouseButton::Left) {
                OnPressed();
                return true;
            }
            else if (event.GetType() == EInputEventType::MouseMoved) {
                if (!isHovered) {
                    isHovered = true;
                    OnHovered();
                }
                return true;
            }
        } else {
            isHovered = false;
        }

        return false;
    }

private:
    bool isHovered = false;
};
```

### Styling and Theming

HUD supports consistent styling across all UI elements:

```cpp
class UHUDTheme {
public:
    FFont defaultFont;
    FColor primaryColor;
    FColor secondaryColor;
    FColor accentColor;
    FColor backgroundColor;
    Float32 borderRadius;
    Float32 shadowOffset;

    static UHUDTheme* GetDefaultTheme();
    static UHUDTheme* CreateCustomTheme(/* parameters */);
};

class UUIComponent {
public:
    void ApplyTheme(UHUDTheme* theme);
    void SetCustomStyle(const FUIStyle& style);
};
```

## Performance Considerations

### Memory Usage

- **Component Overhead**: Each UI component adds memory overhead
- **Texture Management**: UI textures should be managed efficiently
- **Font Caching**: Font glyphs should be cached for performance
- **Object Pooling**: Reuse UI components when possible

### CPU Performance

- **Rendering Cost**: UI rendering is typically inexpensive compared to 3D
- **Input Processing**: Only active UI elements need input processing
- **Update Frequency**: UI updates can be less frequent than game logic
- **Batching**: Group similar UI elements for efficient rendering

### Optimization Strategies

1. **Component Pooling**: Reuse inactive UI components
2. **Lazy Updates**: Only update UI when values change
3. **Viewport Culling**: Don't render off-screen UI elements
4. **Texture Atlasing**: Combine small textures into atlases

### Best Practices

- **Minimal Updates**: Update UI at 30-60 FPS maximum
- **Efficient Layout**: Use anchor-based positioning for responsiveness
- **Texture Compression**: Use appropriate texture formats for UI
- **Font Subsetting**: Only load needed font characters
- **Input Prioritization**: Process UI input before game input

## Architecture Diagrams

### HUD Class Hierarchy

```
UObject (Base Object)
├── UUID: Unique identifier
├── Name: Object name
├── Properties: Reflection system
├── Events: Object lifecycle
└── Serialization: Data persistence

ITickable (Update Interface)
└── Tick(deltaTime): Frame update method

AActor (Game Actor)
├── Transform: Position/Rotation/Scale
├── Active State: Update enable/disable
├── Components: Modular functionality
├── Network State: Prediction/Reconciliation
├── Interpolation: Smooth synchronization
├── Movement RPCs: Network movement sync
└── Lifecycle: BeginPlay/Tick/EndPlay

AHUD (HUD Actor)
├── UI Components: Text, Images, Progress bars
├── Screen Space: 2D overlay rendering
├── Input Handling: UI interaction processing
├── Layer System: Z-order management
├── Resolution Scaling: Automatic UI scaling
├── Event System: UI callbacks and events
└── Styling: Consistent visual appearance
```

### UI Component System

```
AHUD
├── UUIComponent (Base UI Component)
│   ├── Position/Size: Layout properties
│   ├── Anchor: Positioning mode
│   ├── Layer: Rendering order
│   ├── Visibility: Show/hide state
│   ├── Render(): Drawing method
│   └── HandleInput(): Input processing
│
├── UTextComponent
│   ├── Text content
│   ├── Font properties
│   └── Text rendering
│
├── UImageComponent
│   ├── Texture reference
│   ├── Tint color
│   └── Image rendering
│
├── UButtonComponent
│   ├── Button states
│   ├── Click events
│   └── Interactive behavior
│
└── UProgressBarComponent
    ├── Progress value
    ├── Fill colors
    └── Progress visualization
```

### HUD Rendering Pipeline

```
Game Frame
├── 3D World Rendering ──► Framebuffer
├── HUD Actor Tick ─────► UI State Update
├── UI Component Render ─► UI Rendering
├── Layer Compositing ──► Final Composition
└── Display Output ─────► Screen
```

## Troubleshooting

### Common Issues

#### UI Elements Not Visible

**Symptoms:** HUD elements don't appear on screen

**Possible Causes:**
- HUD actor not spawned or active
- UI components not properly initialized
- Rendering system not configured for HUD
- Components positioned outside viewport

**Solutions:**
```cpp
// Check HUD actor state
void DebugHUDVisibility(AHUD* hud) {
    if (!hud) {
        FLogger::Log(ELogLevel::Error, "HUD actor is null");
        return;
    }

    if (!hud->IsActive()) {
        FLogger::Log(ELogLevel::Warning, "HUD actor is not active");
        hud->SetActive(true);
    }

    // Check components
    auto components = hud->GetComponents<UUIComponent>();
    FLogger::Log(ELogLevel::Debug,
        FString::Format("HUD has {} UI components", components.size()));

    for (size_t i = 0; i < components.size(); ++i) {
        UUIComponent* component = components[i];
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Component {}: {} (Visible: {}, Layer: {})",
                i, component->GetName(), component->visible.Get(), component->layer.Get()));

        FRect bounds = component->GetBounds();
        FLogger::Log(ELogLevel::Debug,
            FString::Format("  Bounds: ({}, {}) - ({}, {})",
                bounds.x, bounds.y, bounds.x + bounds.width, bounds.y + bounds.height));
    }
}

// Ensure HUD is registered with rendering system
void RegisterHUDWithRenderer(AHUD* hud, URenderer* renderer) {
    if (!renderer->HasHUD(hud)) {
        renderer->RegisterHUD(hud);
        FLogger::Log(ELogLevel::Debug, "HUD registered with renderer");
    }
}
```

#### UI Elements Incorrectly Positioned

**Symptoms:** HUD elements appear in wrong locations

**Possible Causes:**
- Incorrect coordinate system usage
- Wrong anchor points
- Viewport size changes not handled
- Scaling issues

**Solutions:**
```cpp
// Debug UI positioning
void DebugUIPositioning(UUIComponent* component) {
    FVector2 pos = component->position.Get();
    FVector2 size = component->size.Get();
    EAnchor anchor = component->anchor.Get();

    FLogger::Log(ELogLevel::Debug,
        FString::Format("Component: {}", component->GetName()));
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Position: ({}, {})", pos.x, pos.y));
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Size: ({}, {})", size.x, size.y));
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Anchor: {}", static_cast<int>(anchor)));

    // Calculate actual screen position
    FVector2 screenPos = CalculateScreenPosition(pos, size, anchor);
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Screen position: ({}, {})", screenPos.x, screenPos.y));
}

// Fix positioning issues
void FixUIPositioning(UUIComponent* component, const FVector2& desiredScreenPos) {
    // Convert screen position back to component coordinates
    FVector2 componentPos = ScreenToComponentPosition(desiredScreenPos,
                                                    component->size.Get(),
                                                    component->anchor.Get());
    component->position = componentPos;
}

// Handle viewport resize
void OnViewportResize(AHUD* hud, UInt32 newWidth, UInt32 newHeight) {
    for (UUIComponent* component : hud->GetComponents<UUIComponent>()) {
        // Update normalized positions if needed
        if (component->anchor.Get() != EAnchor::Absolute) {
            component->UpdateAnchoredPosition(newWidth, newHeight);
        }
    }
}
```

#### Input Not Working on UI Elements

**Symptoms:** UI elements don't respond to mouse/keyboard input

**Possible Causes:**
- Input events not reaching HUD
- UI components not handling input correctly
- Z-order issues with overlapping elements
- Input mode not set correctly

**Solutions:**
```cpp
// Debug input handling
void DebugUIInput(AHUD* hud, const FInputEvent& event) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Input event: Type={}, MousePos=({}, {})",
            static_cast<int>(event.GetType()),
            event.GetMousePosition().x, event.GetMousePosition().y));

    // Test which components would handle this input
    auto components = hud->GetComponents<UUIComponent>();
    for (UUIComponent* component : components) {
        if (component->ContainsPoint(event.GetMousePosition())) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Component {} contains point", component->GetName()));

            if (component->HandleInput(event)) {
                FLogger::Log(ELogLevel::Debug,
                    FString::Format("Component {} handled input", component->GetName()));
                break;
            }
        }
    }
}

// Fix input mode
void SetUIInputMode(APlayerController* controller) {
    controller->SetInputMode(EInputMode::UI);
    FLogger::Log(ELogLevel::Debug, "Input mode set to UI");
}

// Check input processing order
void ProcessUIInput(AHUD* hud, const FInputEvent& event) {
    // Process in reverse layer order (top to bottom)
    auto components = hud->GetComponents<UUIComponent>();
    std::sort(components.begin(), components.end(),
        [](UUIComponent* a, UUIComponent* b) {
            return a->layer.Get() > b->layer.Get();
        });

    for (UUIComponent* component : components) {
        if (component->visible.Get() && component->HandleInput(event)) {
            // Input was handled, stop processing
            break;
        }
    }
}
```

#### Performance Issues with Many UI Elements

**Symptoms:** Low frame rate with complex HUD

**Possible Causes:**
- Too many UI components updating every frame
- Inefficient rendering of UI elements
- Memory leaks in UI component management
- Unnecessary texture swaps

**Solutions:**
```cpp
// Profile UI performance
class HUDProfiler {
public:
    void ProfileHUD(AHUD* hud) {
        auto start = std::chrono::high_resolution_clock::now();

        // Measure update time
        hud->Tick(1.0f / 60.0f);

        auto updateEnd = std::chrono::high_resolution_clock::now();

        // Measure render time
        UCanvas* canvas = GetCanvas();
        for (UUIComponent* component : hud->GetComponents<UUIComponent>()) {
            if (component->visible.Get()) {
                component->Render(canvas);
            }
        }

        auto renderEnd = std::chrono::high_resolution_clock::now();

        auto updateDuration = std::chrono::duration_cast<std::chrono::microseconds>(updateEnd - start);
        auto renderDuration = std::chrono::duration_cast<std::chrono::microseconds>(renderEnd - updateEnd);

        FLogger::Log(ELogLevel::Debug,
            FString::Format("HUD Update: {}us, Render: {}us",
                updateDuration.count(), renderDuration.count()));

        if (updateDuration.count() > 5000 || renderDuration.count() > 5000) {
            FLogger::Log(ELogLevel::Warning, "HUD performance is poor");
            OptimizeHUD(hud);
        }
    }

private:
    void OptimizeHUD(AHUD* hud) {
        // Disable unnecessary updates
        for (UUIComponent* component : hud->GetComponents<UUIComponent>()) {
            if (!component->NeedsFrequentUpdates()) {
                component->SetUpdateFrequency(30.0f); // 30 FPS instead of 60
            }
        }

        // Implement component pooling
        // Combine small UI elements into atlases
        // Use simpler rendering for distant elements
    }
};

// Implement UI culling
void CullUIOffscreen(AHUD* hud) {
    FVector2 viewportSize = GetViewportSize();

    for (UUIComponent* component : hud->GetComponents<UUIComponent>()) {
        FRect bounds = component->GetBounds();

        // Check if component is visible on screen
        bool isVisible = bounds.x < viewportSize.x && bounds.y < viewportSize.y &&
                        (bounds.x + bounds.width) > 0 && (bounds.y + bounds.height) > 0;

        if (component->visible.Get() != isVisible) {
            component->visible = isVisible;
        }
    }
}
```

### Debug Tools

Enable comprehensive HUD debugging:

```cpp
class HUDDebugger {
public:
    static void LogHUDState(AHUD* hud) {
        FLogger::Log(ELogLevel::Debug, "=== HUD Debug Info ===");
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Name: {}", hud->GetName()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Active: {}", hud->IsActive()));

        auto components = hud->GetComponents<UUIComponent>();
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Total components: {}", components.size()));

        std::map<Int32, Int32> layerCounts;
        Int32 visibleCount = 0;

        for (UUIComponent* component : components) {
            if (component->visible.Get()) visibleCount++;
            layerCounts[component->layer.Get()]++;
        }

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Visible components: {}", visibleCount));

        FLogger::Log(ELogLevel::Debug, "Components by layer:");
        for (const auto& pair : layerCounts) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("  Layer {}: {} components", pair.first, pair.second));
        }
    }

    static void VisualizeUIBounds(AHUD* hud, UCanvas* canvas) {
        for (UUIComponent* component : hud->GetComponents<UUIComponent>()) {
            if (component->visible.Get()) {
                FRect bounds = component->GetBounds();
                canvas->DrawRect(bounds, FColor(1.0f, 0.0f, 0.0f, 0.5f)); // Red outline
            }
        }
    }

    static void TestUIInteractions(AHUD* hud) {
        // Simulate mouse clicks on all interactive components
        auto components = hud->GetComponents<UUIComponent>();

        for (UUIComponent* component : components) {
            if (UButtonComponent* button = dynamic_cast<UButtonComponent*>(component)) {
                FVector2 center = component->GetBounds().GetCenter();
                FInputEvent clickEvent(EInputEventType::MouseButtonPressed,
                                     center, EMouseButton::Left);

                FLogger::Log(ELogLevel::Debug,
                    FString::Format("Testing button: {}", button->GetName()));

                if (button->HandleInput(clickEvent)) {
                    FLogger::Log(ELogLevel::Debug, "Button handled click event");
                }
            }
        }
    }

    static void ValidateUIHierarchy(AHUD* hud) {
        auto components = hud->GetComponents<UUIComponent>();

        // Check for naming conflicts
        std::set<FString> names;
        for (UUIComponent* component : components) {
            if (names.count(component->GetName()) > 0) {
                FLogger::Log(ELogLevel::Warning,
                    FString::Format("Duplicate component name: {}", component->GetName()));
            }
            names.insert(component->GetName());
        }

        // Check layer ordering
        Int32 prevLayer = INT_MIN;
        for (UUIComponent* component : components) {
            if (component->layer.Get() < prevLayer) {
                FLogger::Log(ELogLevel::Warning, "Components not in layer order");
                break;
            }
            prevLayer = component->layer.Get();
        }

        // Validate positioning
        FVector2 viewportSize = GetViewportSize();
        for (UUIComponent* component : components) {
            FRect bounds = component->GetBounds();
            if (bounds.x + bounds.width < 0 || bounds.y + bounds.height < 0 ||
                bounds.x > viewportSize.x || bounds.y > viewportSize.y) {
                FLogger::Log(ELogLevel::Warning,
                    FString::Format("Component {} is outside viewport", component->GetName()));
            }
        }
    }
};
```

## Future Enhancements

### Planned Features

1. **Advanced Layout System**: Constraint-based layouts and responsive design
2. **Animation System**: UI transitions and state animations
3. **Accessibility**: Screen reader support and keyboard navigation
4. **Localization**: Multi-language text and cultural adaptation
5. **Touch Support**: Mobile and tablet UI interactions
6. **3D UI Elements**: World-space UI and volumetric displays
7. **Data Binding**: Automatic UI updates from game state
8. **Skinning System**: Runtime UI appearance changes

### Extension Points

The HUD system allows for easy extension:

```cpp
class AdvancedHUD : public AHUD {
public:
    // Add custom UI components
    virtual void RegisterCustomComponents() {
        // Register new component types
    }

    // Add advanced input handling
    virtual bool HandleAdvancedInput(const FInputEvent& event) {
        // Custom input processing
        return AHUD::HandleAdvancedInput(event);
    }

    // Add UI animation system
    virtual void UpdateAnimations(Float32 deltaTime) {
        // Animate UI elements
    }

    // Add accessibility features
    virtual void UpdateAccessibility() {
        // Screen reader support, etc.
    }
};

// Custom UI components
class UCustomProgressBar : public UProgressBarComponent {
public:
    // Add gradient fills
    UProperty<FColor> gradientStartColor;
    UProperty<FColor> gradientEndColor;

    virtual void Render(UCanvas* canvas) override {
        // Custom gradient rendering
    }
};

class UAnimatedText : public UTextComponent {
public:
    // Add typewriter effect
    UProperty<Float32> typeSpeed;
    UProperty<bool> isTyping;

    virtual void Tick(Float32 deltaTime) override {
        if (isTyping) {
            // Animate text appearance
        }
    }
};
```

### Advanced Features

Future versions may include:
- **UI Editor**: Visual HUD design and layout tools
- **Performance Monitoring**: Real-time UI performance metrics
- **Cross-Platform Scaling**: Automatic adaptation for different devices
- **Dynamic Resolution**: UI scaling based on performance requirements
- **Procedural UI**: Data-driven UI generation
- **Multi-Modal Input**: Voice, gesture, and eye-tracking input
- **AR/VR Integration**: Spatial UI for augmented reality

---

*This documentation covers the complete AHUD implementation as of TKD Engine v1.0.0. For the latest updates and additional examples, refer to the engine's source code and test suites.*
