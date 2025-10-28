# FlipBook Animation Example

This document demonstrates how to use FlipBook animations with Sprite and Billboard components.

## Overview

The FlipBook system allows you to create animated sprites from either:
- **Individual frame files** (FromFiles mode)
- **A sprite sheet** (FromSprite mode)

Both `USpriteComponent` and `UBillboardComponent` support FlipBook animations.

## Basic Usage

### 1. Create a FlipBook from Individual Files

```cpp
// Create a FlipBook
UFlipBook* walkAnimation = new UFlipBook("WalkAnimation", 
    UFlipBook::ESourceMode::FromFiles,
    UFlipBook::EPlayMode::Loop);

// Add frame paths
walkAnimation->AddFrame("Assets/Character/walk_01.png");
walkAnimation->AddFrame("Assets/Character/walk_02.png");
walkAnimation->AddFrame("Assets/Character/walk_03.png");
walkAnimation->AddFrame("Assets/Character/walk_04.png");

// Set animation duration (in seconds)
walkAnimation->SetDuration(0.8f);

// Load the frames
walkAnimation->LoadFrames();
```

### 2. Create a FlipBook from Sprite Sheet

```cpp
// Create a FlipBook
UFlipBook* runAnimation = new UFlipBook("RunAnimation",
    UFlipBook::ESourceMode::FromSprite,
    UFlipBook::EPlayMode::Loop);

// Set the sprite sheet path
runAnimation->SetSpriteSheetPath("Assets/Character/run_spritesheet.png");

// Define frame rectangles (x, y, width, height)
runAnimation->AddFrame(FRectangle(0, 0, 32, 32));
runAnimation->AddFrame(FRectangle(32, 0, 32, 32));
runAnimation->AddFrame(FRectangle(64, 0, 32, 32));
runAnimation->AddFrame(FRectangle(96, 0, 32, 32));

// Set animation duration
runAnimation->SetDuration(0.6f);

// Load the sprite sheet
runAnimation->LoadFrames();
```

## Using FlipBooks with USpriteComponent

### Setup

```cpp
// Get or create a sprite component
USpriteComponent* spriteComp = actor->GetComponent<USpriteComponent>();

// Set the FlipBook (automatically switches to FlipBook mode)
spriteComp->SetFlipBook(walkAnimation);

// The component will now display the animated FlipBook
```

### Switching Between Static Texture and FlipBook

```cpp
// Switch to static texture mode
spriteComp->SetDisplayMode(USpriteComponent::EDisplayMode::StaticTexture);
spriteComp->SetTexturePath("Assets/Character/idle.png");

// Switch to FlipBook mode
spriteComp->SetDisplayMode(USpriteComponent::EDisplayMode::FlipBook);
spriteComp->SetFlipBook(walkAnimation);
```

### Changing Animations

```cpp
// Create multiple animations
UFlipBook* idleAnimation = /* ... */;
UFlipBook* walkAnimation = /* ... */;
UFlipBook* attackAnimation = /* ... */;

// Switch between animations
void ChangeAnimation(UFlipBook* newAnimation)
{
    spriteComp->SetFlipBook(newAnimation);
    // Animation automatically starts playing
}

// Example: Change based on state
if (playerState == State::Idle)
    ChangeAnimation(idleAnimation);
else if (playerState == State::Walking)
    ChangeAnimation(walkAnimation);
else if (playerState == State::Attacking)
    ChangeAnimation(attackAnimation);
```

### Update Loop

```cpp
// In your game loop or actor's Update method
void Update(float deltaTime)
{
    // Update the sprite component to advance FlipBook animation
    spriteComp->Update(deltaTime);
}
```

## Using FlipBooks with UBillboardComponent

Billboard components work the same way as sprite components:

```cpp
// Get or create a billboard component
UBillboardComponent* billboardComp = actor->GetComponent<UBillboardComponent>();

// Set the FlipBook
billboardComp->SetFlipBook(explosionAnimation);

// Update in game loop
void Update(float deltaTime)
{
    billboardComp->Update(deltaTime);
}
```

## Play Modes

### Loop Mode
```cpp
flipBook->SetPlayMode(UFlipBook::EPlayMode::Loop);
// Animation loops indefinitely
```

### Once Mode
```cpp
flipBook->SetPlayMode(UFlipBook::EPlayMode::Once);
// Plays once and stops at the last frame

// Check if animation finished
if (flipBook->HasFinished())
{
    // Handle animation completion
    // E.g., destroy explosion effect, switch to idle animation, etc.
}
```

### PingPong Mode
```cpp
flipBook->SetPlayMode(UFlipBook::EPlayMode::PingPong);
// Plays forward, then backward, then forward, etc.
```

## Advanced Features

### Playback Control

```cpp
// Pause the animation
flipBook->Pause();

// Resume the animation
flipBook->Resume();

// Restart from beginning
flipBook->Restart();

// Check if playing
if (flipBook->IsPlaying())
{
    // Animation is running
}
```

### Playback Speed

```cpp
// Normal speed
flipBook->SetPlaybackSpeed(1.0f);

// Double speed
flipBook->SetPlaybackSpeed(2.0f);

// Half speed (slow motion)
flipBook->SetPlaybackSpeed(0.5f);

// Reverse playback (play animation backwards)
flipBook->SetPlaybackSpeed(-1.0f);

// Fast reverse
flipBook->SetPlaybackSpeed(-2.0f);

// Get current speed
float speed = flipBook->GetPlaybackSpeed();
```

**Note on Negative Playback Speed:**
- **Negative values** reverse the animation direction
- `-1.0f` plays the animation backwards at normal speed
- Works with all play modes:
  - **Loop**: Continuously loops backward
  - **Once**: Plays from end to start, then stops
  - **PingPong**: Reverses the ping-pong direction
- Useful for rewinding effects, returning to idle state, or creative animations

### Progress Tracking

```cpp
// Get animation progress (0.0 to 1.0)
float progress = flipBook->GetProgress();

// Example: trigger event at 50% through animation
if (progress >= 0.5f && !eventTriggered)
{
    TriggerHalfwayEvent();
    eventTriggered = true;
}
```

## Complete Example: Character with Multiple Animations

```cpp
class ACharacter : public AActor
{
private:
    USpriteComponent* m_spriteComponent;
    UFlipBook* m_idleAnimation;
    UFlipBook* m_walkAnimation;
    UFlipBook* m_jumpAnimation;
    
public:
    void Initialize()
    {
        // Create sprite component
        m_spriteComponent = CreateComponent<USpriteComponent>("Sprite");
        
        // Create idle animation
        m_idleAnimation = new UFlipBook("Idle", 
            UFlipBook::ESourceMode::FromSprite,
            UFlipBook::EPlayMode::Loop);
        m_idleAnimation->SetSpriteSheetPath("Assets/Character/spritesheet.png");
        m_idleAnimation->AddFrame(FRectangle(0, 0, 32, 32));
        m_idleAnimation->AddFrame(FRectangle(32, 0, 32, 32));
        m_idleAnimation->SetDuration(1.0f);
        m_idleAnimation->LoadFrames();
        
        // Create walk animation
        m_walkAnimation = new UFlipBook("Walk",
            UFlipBook::ESourceMode::FromSprite,
            UFlipBook::EPlayMode::Loop);
        m_walkAnimation->SetSpriteSheetPath("Assets/Character/spritesheet.png");
        m_walkAnimation->AddFrame(FRectangle(0, 32, 32, 32));
        m_walkAnimation->AddFrame(FRectangle(32, 32, 32, 32));
        m_walkAnimation->AddFrame(FRectangle(64, 32, 32, 32));
        m_walkAnimation->AddFrame(FRectangle(96, 32, 32, 32));
        m_walkAnimation->SetDuration(0.6f);
        m_walkAnimation->LoadFrames();
        
        // Create jump animation (plays once)
        m_jumpAnimation = new UFlipBook("Jump",
            UFlipBook::ESourceMode::FromSprite,
            UFlipBook::EPlayMode::Once);
        m_jumpAnimation->SetSpriteSheetPath("Assets/Character/spritesheet.png");
        m_jumpAnimation->AddFrame(FRectangle(0, 64, 32, 32));
        m_jumpAnimation->AddFrame(FRectangle(32, 64, 32, 32));
        m_jumpAnimation->AddFrame(FRectangle(64, 64, 32, 32));
        m_jumpAnimation->SetDuration(0.3f);
        m_jumpAnimation->LoadFrames();
        
        // Start with idle animation
        m_spriteComponent->SetFlipBook(m_idleAnimation);
    }
    
    void Update(float deltaTime) override
    {
        // Update sprite animation
        m_spriteComponent->Update(deltaTime);
        
        // Switch animations based on state
        if (IsJumping())
        {
            if (m_spriteComponent->GetFlipBook() != m_jumpAnimation)
            {
                m_spriteComponent->SetFlipBook(m_jumpAnimation);
            }
        }
        else if (IsMoving())
        {
            if (m_spriteComponent->GetFlipBook() != m_walkAnimation)
            {
                m_spriteComponent->SetFlipBook(m_walkAnimation);
            }
        }
        else
        {
            if (m_spriteComponent->GetFlipBook() != m_idleAnimation)
            {
                m_spriteComponent->SetFlipBook(m_idleAnimation);
            }
        }
    }
};
```

## Tips and Best Practices

1. **Memory Management**: Load FlipBooks once and reuse them across multiple actors
2. **Frame Duration**: Adjust duration based on desired animation speed (0.5-1.0s for most animations)
3. **Once Mode**: Use for one-shot effects like explosions, attacks, etc.
4. **Loop Mode**: Use for continuous animations like idle, walk, run
5. **PingPong Mode**: Use for breathing effects, UI pulsing, etc.
6. **Update Calls**: Always call `Update(deltaTime)` on components using FlipBooks
7. **Animation Switching**: Check current FlipBook before switching to avoid restarting the same animation
8. **Reverse Playback**: Use negative playback speed (`-1.0f`) to smoothly rewind animations

## Advanced Example: Door with Opening/Closing Animation

```cpp
class ADoor : public AActor
{
private:
    USpriteComponent* m_spriteComponent;
    UFlipBook* m_doorAnimation;
    bool m_isOpen;
    
public:
    void Initialize()
    {
        m_spriteComponent = CreateComponent<USpriteComponent>("DoorSprite");
        m_isOpen = false;
        
        // Create door animation (plays once)
        m_doorAnimation = new UFlipBook("DoorAnim",
            UFlipBook::ESourceMode::FromSprite,
            UFlipBook::EPlayMode::Once);
        m_doorAnimation->SetSpriteSheetPath("Assets/door.png");
        
        // Add frames from closed (0) to open (4)
        for (int i = 0; i < 5; i++)
        {
            m_doorAnimation->AddFrame(FRectangle(i * 64, 0, 64, 64));
        }
        
        m_doorAnimation->SetDuration(0.5f);
        m_doorAnimation->LoadFrames();
        
        // Start with door closed
        m_spriteComponent->SetFlipBook(m_doorAnimation);
        m_doorAnimation->Pause();  // Start paused at closed position
    }
    
    void Open()
    {
        if (!m_isOpen)
        {
            m_isOpen = true;
            m_doorAnimation->SetPlaybackSpeed(1.0f);   // Forward playback
            m_doorAnimation->Restart();
            m_doorAnimation->Resume();
        }
    }
    
    void Close()
    {
        if (m_isOpen)
        {
            m_isOpen = false;
            m_doorAnimation->SetPlaybackSpeed(-1.0f);  // Reverse playback
            m_doorAnimation->SetCurrentTime(m_doorAnimation->GetDuration());  // Start from end
            m_doorAnimation->Resume();
        }
    }
    
    void Update(float deltaTime) override
    {
        m_spriteComponent->Update(deltaTime);
        
        // Pause when animation finishes
        if (m_doorAnimation->HasFinished())
        {
            m_doorAnimation->Pause();
        }
    }
};
```
