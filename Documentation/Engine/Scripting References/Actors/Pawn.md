# Pawn Documentation

## Overview

The `APawn` class is a specialized actor that represents physical entities in the TKD Game Engine that can be possessed and controlled by players or AI. As a base class that inherits from `AActor`, it provides the foundation for creating controllable game objects such as player characters, vehicles, mounts, and AI-controlled entities.

### Key Features

- **Possession System**: Can be possessed by controllers for input processing
- **Network Replication**: Always replicated for multiplayer synchronization
- **Controller Integration**: Seamless integration with player and AI controllers
- **Input Processing**: Receives and processes input from possessing controllers
- **Movement Authority**: Handles movement logic and physics interaction
- **State Synchronization**: Maintains consistent state across network clients
- **Lifecycle Management**: Complete lifecycle with possession/unpossession events

### Architecture

```cpp
class APawn : public AActor
{
public:
    APawn(void);
};
```

The `APawn` builds upon the `AActor` foundation with specialized features for controllable entities:

- **Possession**: Can be possessed by `AController` instances
- **Input**: Receives input from possessing controller
- **Replication**: Always replicated for network synchronization
- **Authority**: Server has authority over pawn state
- **Ownership**: Maintains relationship with controlling entity

## Core Components

### Possession System

The possession system allows controllers to take control of pawns:

```cpp
class AController {
public:
    virtual void Possess(APawn* pawn);
    virtual void UnPossess();
    APawn* GetPawn() const;
    bool IsPossessing() const;
};

class APawn {
public:
    AController* GetController() const;
    bool IsPossessed() const;
    void OnPossessed(AController* newController);
    void OnUnPossessed();
};
```

### Input Processing

Pawns receive input through the possession system:

```cpp
class APawn {
protected:
    virtual void SetupPlayerInput(AController* controller);
    virtual void ProcessInput(Float32 deltaTime);
    virtual void HandleInput(const FInputAction& action, Float32 value);
};
```

### Movement Authority

Pawns handle movement with network synchronization:

- **Client Prediction**: Local movement simulation for responsiveness
- **Server Authority**: Authoritative movement validation
- **Reconciliation**: Correction of prediction errors
- **Interpolation**: Smooth state synchronization

## API Reference

### Constructor & Destructor

#### `APawn(void)`

Creates a new pawn actor instance.

**Behavior:**
- Initializes the pawn with default name "APawn"
- Enables transform replication (pawns are always networked)
- Sets up pawn-specific infrastructure

### Possession Methods

#### `AController* GetController(void) const`

Retrieves the controller currently possessing this pawn.

**Returns:** Pointer to possessing controller, nullptr if not possessed

#### `bool IsPossessed(void) const`

Checks if the pawn is currently possessed by a controller.

**Returns:** True if possessed, false otherwise

#### `void OnPossessed(AController* newController)`

Called when a controller possesses this pawn.

**Parameters:**
- `newController`: The controller that is possessing this pawn

**Behavior:**
- Sets up input processing for the controller
- Initializes possession-specific state
- Calls possession event handlers

#### `void OnUnPossessed(void)`

Called when the possessing controller unpossesses this pawn.

**Behavior:**
- Cleans up input processing
- Resets possession-specific state
- Calls unpossession event handlers

### Input Methods

#### `virtual void SetupPlayerInput(AController* controller)`

Sets up input bindings for the possessing controller.

**Parameters:**
- `controller`: The controller to set up input for

**Behavior:**
- Binds input actions to pawn methods
- Configures input processing settings
- Override in derived classes for custom input setup

#### `virtual void ProcessInput(Float32 deltaTime)`

Processes input from the possessing controller.

**Parameters:**
- `deltaTime`: Time elapsed since last input processing

**Behavior:**
- Updates input state
- Applies movement and actions
- Called every frame when possessed

#### `virtual void HandleInput(const FInputAction& action, Float32 value)`

Handles a specific input action.

**Parameters:**
- `action`: The input action that occurred
- `value`: The value associated with the action (0.0-1.0 for axes)

**Behavior:**
- Processes individual input events
- Updates pawn state based on input
- Override for custom input handling

### Inherited Methods

Since `APawn` inherits from `AActor`, it has access to all actor functionality:

- **Lifecycle**: `BeginPlay()`, `Tick(Float32 deltaTime)`, `EndPlay()`
- **Component Management**: `AddComponent<T>()`, `GetComponent<T>()`, `RemoveComponent()`
- **Transform**: `GetTransform()`, `SetTransform()`, `Translate()`, `Rotate()`, `Scale()`
- **Movement**: `ApplyMovement()`, `SimulateMovement()`
- **Networking**: Property replication, RPCs, movement synchronization
- **State Management**: `IsActive()`, `SetActive()`, `MarkForDeletion()`

## Usage Examples

### Basic Player Character

```cpp
class APlayerCharacter : public APawn
{
public:
    APlayerCharacter()
        : APawn()
        , moveSpeed(300.0f)
        , jumpForce(500.0f)
        , isGrounded(true)
    {
        // Add character components
        capsuleComponent = AddComponent<UCapsuleComponent>("Capsule");
        meshComponent = AddComponent<USkeletalMeshComponent>("Mesh");
        cameraComponent = AddComponent<UCameraComponent>("Camera");

        // Configure capsule
        capsuleComponent->SetRadius(20.0f);
        capsuleComponent->SetHeight(90.0f);

        // Position camera
        cameraComponent->SetRelativeLocation(FVector3(0, 0, 60));
    }

    virtual void BeginPlay() override
    {
        APawn::BeginPlay();

        // Initialize character state
        currentHealth = maxHealth;
        currentAmmo = maxAmmo;
    }

    virtual void SetupPlayerInput(AController* controller) override
    {
        if (APlayerController* pc = dynamic_cast<APlayerController*>(controller))
        {
            // Movement input
            pc->BindAxis("MoveForward", [this](Float32 value) {
                HandleMoveForward(value);
            });
            pc->BindAxis("MoveRight", [this](Float32 value) {
                HandleMoveRight(value);
            });

            // Action input
            pc->BindAction("Jump", EInputEvent::Pressed, [this]() {
                HandleJump();
            });
            pc->BindAction("Fire", EInputEvent::Pressed, [this]() {
                HandleFire();
            });
            pc->BindAction("Reload", EInputEvent::Pressed, [this]() {
                HandleReload();
            });
        }
    }

    virtual void Tick(Float32 deltaTime) override
    {
        APawn::Tick(deltaTime);

        // Update character logic
        UpdateMovement(deltaTime);
        UpdateAnimation();
        UpdateHealthRegeneration(deltaTime);
    }

private:
    void HandleMoveForward(Float32 value)
    {
        if (abs(value) > 0.1f)
        {
            FVector3 forward = GetController()->GetControlRotation().GetForwardVector();
            FVector3 movement = forward * value * moveSpeed * GetWorld()->GetDeltaTime();
            ApplyMovement(movement, GetWorld()->GetDeltaTime());
        }
    }

    void HandleMoveRight(Float32 value)
    {
        if (abs(value) > 0.1f)
        {
            FVector3 right = GetController()->GetControlRotation().GetRightVector();
            FVector3 movement = right * value * moveSpeed * GetWorld()->GetDeltaTime();
            ApplyMovement(movement, GetWorld()->GetDeltaTime());
        }
    }

    void HandleJump()
    {
        if (isGrounded)
        {
            FVector3 jumpVelocity = FVector3(0, 0, jumpForce);
            capsuleComponent->AddImpulse(jumpVelocity);
            isGrounded = false;
        }
    }

    void HandleFire()
    {
        if (currentAmmo > 0 && CanFire())
        {
            // Spawn projectile
            AProjectile* projectile = GetWorld()->SpawnActor<AProjectile>(
                "Projectile", GetTransform().GetPosition() + GetForwardVector() * 50.0f);

            // Apply velocity
            projectile->SetVelocity(GetForwardVector() * projectileSpeed);

            currentAmmo--;
            lastFireTime = GetWorld()->GetTime();
        }
    }

    void HandleReload()
    {
        if (currentAmmo < maxAmmo && !isReloading)
        {
            isReloading = true;
            // Start reload animation/montage
            meshComponent->PlayAnimation(reloadAnimation);

            // Schedule reload completion
            GetWorld()->SetTimer(reloadTime, [this]() {
                currentAmmo = maxAmmo;
                isReloading = false;
            });
        }
    }

    void UpdateMovement(Float32 deltaTime)
    {
        // Apply gravity
        if (!isGrounded)
        {
            FVector3 gravity = FVector3(0, 0, -gravityStrength * deltaTime);
            capsuleComponent->AddVelocity(gravity);
        }

        // Ground check
        FVector3 groundCheckPos = GetTransform().GetPosition() - FVector3(0, 0, capsuleComponent->GetHeight() / 2 + 5.0f);
        isGrounded = GetWorld()->LineTrace(groundCheckPos, groundCheckPos - FVector3(0, 0, 10.0f));
    }

    void UpdateAnimation()
    {
        FVector3 velocity = capsuleComponent->GetVelocity();

        if (velocity.Length() > 10.0f)
        {
            if (isGrounded)
            {
                meshComponent->PlayAnimation(runAnimation);
            }
        }
        else
        {
            meshComponent->PlayAnimation(idleAnimation);
        }
    }

    void UpdateHealthRegeneration(Float32 deltaTime)
    {
        if (currentHealth < maxHealth && lastDamageTime + healthRegenDelay < GetWorld()->GetTime())
        {
            currentHealth = FMath::Min(maxHealth, currentHealth + healthRegenRate * deltaTime);
        }
    }

    bool CanFire() const
    {
        return GetWorld()->GetTime() - lastFireTime >= fireRate;
    }

private:
    // Components
    UCapsuleComponent* capsuleComponent;
    USkeletalMeshComponent* meshComponent;
    UCameraComponent* cameraComponent;

    // Movement
    Float32 moveSpeed;
    Float32 jumpForce;
    bool isGrounded;

    // Combat
    Int32 currentAmmo;
    Int32 maxAmmo;
    Float32 lastFireTime;
    Float32 fireRate;
    Float32 projectileSpeed;
    bool isReloading;
    Float32 reloadTime;

    // Health
    Float32 currentHealth;
    Float32 maxHealth;
    Float32 lastDamageTime;
    Float32 healthRegenRate;
    Float32 healthRegenDelay;

    // Physics
    Float32 gravityStrength;

    // Animations
    UAnimationAsset* idleAnimation;
    UAnimationAsset* runAnimation;
    UAnimationAsset* reloadAnimation;
};
```

### Vehicle Pawn

```cpp
class AVehiclePawn : public APawn
{
public:
    AVehiclePawn()
        : APawn()
        , enginePower(1000.0f)
        , maxSpeed(50.0f)
        , turnSpeed(2.0f)
        , currentSpeed(0.0f)
    {
        // Add vehicle components
        chassisComponent = AddComponent<UStaticMeshComponent>("Chassis");
        wheelComponents[0] = AddComponent<UStaticMeshComponent>("WheelFL");
        wheelComponents[1] = AddComponent<UStaticMeshComponent>("WheelFR");
        wheelComponents[2] = AddComponent<UStaticMeshComponent>("WheelRL");
        wheelComponents[3] = AddComponent<UStaticMeshComponent>("WheelRR");

        // Set up physics
        chassisComponent->SetSimulatePhysics(true);
        chassisComponent->SetMass(1000.0f);

        // Position wheels
        wheelComponents[0]->SetRelativeLocation(FVector3(40, 30, -20));
        wheelComponents[1]->SetRelativeLocation(FVector3(40, -30, -20));
        wheelComponents[2]->SetRelativeLocation(FVector3(-40, 30, -20));
        wheelComponents[3]->SetRelativeLocation(FVector3(-40, -30, -20));
    }

    virtual void SetupPlayerInput(AController* controller) override
    {
        if (APlayerController* pc = dynamic_cast<APlayerController*>(controller))
        {
            pc->BindAxis("Throttle", [this](Float32 value) {
                HandleThrottle(value);
            });
            pc->BindAxis("Steer", [this](Float32 value) {
                HandleSteering(value);
            });
            pc->BindAction("Handbrake", EInputEvent::Pressed, [this]() {
                HandleHandbrake(true);
            });
            pc->BindAction("Handbrake", EInputEvent::Released, [this]() {
                HandleHandbrake(false);
            });
        }
    }

    virtual void Tick(Float32 deltaTime) override
    {
        APawn::Tick(deltaTime);

        UpdateVehiclePhysics(deltaTime);
        UpdateWheelAnimations();
        UpdateEngineSound();
    }

private:
    void HandleThrottle(Float32 value)
    {
        // Apply engine force
        Float32 force = value * enginePower;
        FVector3 forward = GetTransform().GetForwardVector();
        chassisComponent->AddForce(forward * force);

        // Limit speed
        FVector3 velocity = chassisComponent->GetVelocity();
        if (velocity.Length() > maxSpeed)
        {
            velocity = velocity.Normalized() * maxSpeed;
            chassisComponent->SetVelocity(velocity);
        }
    }

    void HandleSteering(Float32 value)
    {
        // Apply steering torque
        Float32 torque = value * turnSpeed * currentSpeed;
        chassisComponent->AddTorque(FVector3(0, 0, torque));
    }

    void HandleHandbrake(bool activated)
    {
        if (activated)
        {
            // Apply braking force
            FVector3 velocity = chassisComponent->GetVelocity();
            FVector3 brakingForce = -velocity.Normalized() * brakingPower;
            chassisComponent->AddForce(brakingForce);
        }
    }

    void UpdateVehiclePhysics(Float32 deltaTime)
    {
        // Update current speed
        currentSpeed = chassisComponent->GetVelocity().Length();

        // Apply rolling resistance
        FVector3 velocity = chassisComponent->GetVelocity();
        FVector3 resistance = -velocity * rollingResistance;
        chassisComponent->AddForce(resistance);

        // Update wheel rotations based on speed
        Float32 wheelRotation = currentSpeed * deltaTime / wheelRadius;
        for (UStaticMeshComponent* wheel : wheelComponents)
        {
            wheel->AddLocalRotation(FRotator(wheelRotation, 0, 0));
        }
    }

    void UpdateWheelAnimations()
    {
        // Update wheel steering angles
        Float32 steerAngle = steeringInput * maxSteerAngle;
        wheelComponents[0]->SetRelativeRotation(FRotator(0, steerAngle, 0));
        wheelComponents[1]->SetRelativeRotation(FRotator(0, steerAngle, 0));
    }

    void UpdateEngineSound()
    {
        // Update engine audio based on speed
        Float32 pitch = 0.5f + (currentSpeed / maxSpeed) * 1.5f;
        engineAudioComponent->SetPitch(pitch);
    }

public:
    // Vehicle interface
    void EnterVehicle(APawn* passenger)
    {
        if (passengers.size() < maxPassengers)
        {
            passengers.push_back(passenger);
            passenger->AttachToActor(this, "PassengerSeat");
            OnPassengerEntered(passenger);
        }
    }

    void ExitVehicle(APawn* passenger)
    {
        auto it = std::find(passengers.begin(), passengers.end(), passenger);
        if (it != passengers.end())
        {
            passengers.erase(it);
            passenger->DetachFromActor();
            OnPassengerExited(passenger);
        }
    }

private:
    UFunction<APawn*> OnPassengerEntered;
    UFunction<APawn*> OnPassengerExited;

    // Components
    UStaticMeshComponent* chassisComponent;
    UStaticMeshComponent* wheelComponents[4];
    UAudioComponent* engineAudioComponent;

    // Physics
    Float32 enginePower;
    Float32 maxSpeed;
    Float32 turnSpeed;
    Float32 currentSpeed;
    Float32 rollingResistance;
    Float32 brakingPower;
    Float32 wheelRadius;
    Float32 maxSteerAngle;

    // Input
    Float32 steeringInput;

    // Passengers
    TVector<APawn*> passengers;
    Int32 maxPassengers;
};
```

### AI-Controlled Pawn

```cpp
class AAICharacter : public APawn
{
public:
    AAICharacter()
        : APawn()
        , detectionRange(500.0f)
        , attackRange(100.0f)
        , moveSpeed(200.0f)
        , currentState(EAIState::Idle)
    {
        // Add AI components
        aiController = AddComponent<UAIControllerComponent>("AIController");
        perceptionComponent = AddComponent<UPerceptionComponent>("Perception");
        behaviorTreeComponent = AddComponent<UBehaviorTreeComponent>("BehaviorTree");

        // Configure AI
        aiController->SetBehaviorTree(aiBehaviorTree);
        perceptionComponent->SetDetectionRange(detectionRange);
    }

    virtual void BeginPlay() override
    {
        APawn::BeginPlay();

        // Spawn AI controller
        aiController = GetWorld()->SpawnActor<AAIController>("AIController");
        aiController->Possess(this);
    }

    virtual void Tick(Float32 deltaTime) override
    {
        APawn::Tick(deltaTime);

        UpdateAIState(deltaTime);
        UpdateBehavior();
    }

    virtual void OnPossessed(AController* newController) override
    {
        APawn::OnPossessed(newController);

        if (AAIController* aiCtrl = dynamic_cast<AAIController*>(newController))
        {
            // AI-specific possession setup
            aiCtrl->SetDetectionRange(detectionRange);
            aiCtrl->SetAttackRange(attackRange);
        }
    }

private:
    void UpdateAIState(Float32 deltaTime)
    {
        // Update AI perception
        TArray<AActor*> perceivedActors = perceptionComponent->GetPerceivedActors();

        // Find player
        APlayerCharacter* player = nullptr;
        Float32 closestDistance = detectionRange;

        for (AActor* actor : perceivedActors)
        {
            if (APlayerCharacter* pc = dynamic_cast<APlayerCharacter*>(actor))
            {
                Float32 distance = (GetTransform().GetPosition() - pc->GetTransform().GetPosition()).Length();
                if (distance < closestDistance)
                {
                    player = pc;
                    closestDistance = distance;
                }
            }
        }

        // Update state based on perception
        if (player)
        {
            if (closestDistance <= attackRange)
            {
                SetAIState(EAIState::Attacking);
                targetActor = player;
            }
            else if (closestDistance <= detectionRange)
            {
                SetAIState(EAIState::Chasing);
                targetActor = player;
            }
        }
        else
        {
            SetAIState(EAIState::Patrolling);
            targetActor = nullptr;
        }
    }

    void UpdateBehavior()
    {
        switch (currentState)
        {
            case EAIState::Idle:
                // Do nothing
                break;

            case EAIState::Patrolling:
                UpdatePatrolBehavior();
                break;

            case EAIState::Chasing:
                UpdateChaseBehavior();
                break;

            case EAIState::Attacking:
                UpdateAttackBehavior();
                break;

            case EAIState::Fleeing:
                UpdateFleeBehavior();
                break;
        }
    }

    void UpdatePatrolBehavior()
    {
        // Simple patrol between waypoints
        if (patrolPoints.empty()) return;

        FVector3 currentPos = GetTransform().GetPosition();
        FVector3 targetPos = patrolPoints[currentPatrolIndex];

        if ((currentPos - targetPos).Length() < 50.0f)
        {
            // Move to next patrol point
            currentPatrolIndex = (currentPatrolIndex + 1) % patrolPoints.size();
            targetPos = patrolPoints[currentPatrolIndex];
        }

        // Move towards target
        FVector3 direction = (targetPos - currentPos).Normalized();
        FVector3 movement = direction * moveSpeed * GetWorld()->GetDeltaTime();
        ApplyMovement(movement, GetWorld()->GetDeltaTime());
    }

    void UpdateChaseBehavior()
    {
        if (!targetActor) return;

        FVector3 currentPos = GetTransform().GetPosition();
        FVector3 targetPos = targetActor->GetTransform().GetPosition();
        FVector3 direction = (targetPos - currentPos).Normalized();

        FVector3 movement = direction * moveSpeed * GetWorld()->GetDeltaTime();
        ApplyMovement(movement, GetWorld()->GetDeltaTime());
    }

    void UpdateAttackBehavior()
    {
        if (!targetActor) return;

        // Stop moving and attack
        // (Attack logic would go here)
        PerformAttack(targetActor);
    }

    void UpdateFleeBehavior()
    {
        if (!targetActor) return;

        FVector3 currentPos = GetTransform().GetPosition();
        FVector3 targetPos = targetActor->GetTransform().GetPosition();
        FVector3 direction = (currentPos - targetPos).Normalized(); // Away from target

        FVector3 movement = direction * moveSpeed * 1.5f * GetWorld()->GetDeltaTime();
        ApplyMovement(movement, GetWorld()->GetDeltaTime());
    }

    void SetAIState(EAIState newState)
    {
        if (currentState != newState)
        {
            currentState = newState;
            OnAIStateChanged(currentState);
        }
    }

    void PerformAttack(AActor* target)
    {
        // Attack logic
        if (APlayerCharacter* player = dynamic_cast<APlayerCharacter*>(target))
        {
            player->TakeDamage(attackDamage);
        }

        // Set cooldown
        attackCooldown = attackRate;
    }

private:
    UAIControllerComponent* aiController;
    UPerceptionComponent* perceptionComponent;
    UBehaviorTreeComponent* behaviorTreeComponent;

    // AI properties
    Float32 detectionRange;
    Float32 attackRange;
    Float32 moveSpeed;
    Float32 attackDamage;
    Float32 attackRate;
    Float32 attackCooldown;

    // State
    EAIState currentState;
    AActor* targetActor;

    // Patrol
    TVector<FVector3> patrolPoints;
    Int32 currentPatrolIndex;

    // Events
    UFunction<EAIState> OnAIStateChanged;
};
```

### Pawn Spawning and Management

```cpp
class AGameMode : public AActor
{
public:
    virtual void BeginPlay() override
    {
        AActor::BeginPlay();

        // Spawn player pawn
        SpawnPlayerPawn();

        // Spawn AI pawns
        SpawnAIPawns();
    }

    void SpawnPlayerPawn()
    {
        // Create player start location
        FTransform spawnTransform = FindPlayerStart();

        // Spawn player character
        playerPawn = GetWorld()->SpawnActor<APlayerCharacter>("PlayerCharacter", spawnTransform);

        // Spawn player controller and possess
        playerController = GetWorld()->SpawnActor<APlayerController>("PlayerController");
        playerController->Possess(playerPawn);
    }

    void SpawnAIPawns()
    {
        for (int i = 0; i < numAIEnemies; ++i)
        {
            // Find spawn location
            FTransform spawnTransform = FindAISpawnLocation(i);

            // Spawn AI character
            AAICharacter* aiPawn = GetWorld()->SpawnActor<AAICharacter>(
                FString::Format("AICharacter{}", i), spawnTransform);
            aiPawns.push_back(aiPawn);

            // Spawn AI controller and possess
            AAIController* aiController = GetWorld()->SpawnActor<AAIController>(
                FString::Format("AIController{}", i));
            aiController->Possess(aiPawn);
        }
    }

    void RespawnPlayer()
    {
        if (playerPawn && playerPawn->IsMarkedForDeletion())
        {
            // Find respawn location
            FTransform respawnTransform = FindPlayerStart();

            // Spawn new player pawn
            playerPawn = GetWorld()->SpawnActor<APlayerCharacter>("PlayerCharacter", respawnTransform);

            // Re-possess with existing controller
            playerController->Possess(playerPawn);
        }
    }

    void OnPlayerDeath()
    {
        // Handle player death
        playerController->UnPossess();

        // Start respawn timer
        GetWorld()->SetTimer(respawnDelay, [this]() {
            RespawnPlayer();
        });
    }

    void RemoveAIPawn(AAICharacter* aiPawn)
    {
        // Find and remove from list
        auto it = std::find(aiPawns.begin(), aiPawns.end(), aiPawn);
        if (it != aiPawns.end())
        {
            aiPawns.erase(it);
        }

        // Mark for deletion
        aiPawn->MarkForDeletion();
    }

private:
    FTransform FindPlayerStart()
    {
        // Find player start actor in world
        APlayerStart* playerStart = GetWorld()->FindActorOfType<APlayerStart>();
        if (playerStart)
        {
            return playerStart->GetTransform();
        }

        // Fallback to origin
        return FTransform::Identity;
    }

    FTransform FindAISpawnLocation(int index)
    {
        // Find AI spawn points
        TArray<AAISpawnPoint*> spawnPoints = GetWorld()->FindActorsOfType<AAISpawnPoint>();
        if (index < spawnPoints.size())
        {
            return spawnPoints[index]->GetTransform();
        }

        // Fallback to random location
        FVector3 randomPos = FVector3(
            FMath::RandRange(-1000.0f, 1000.0f),
            FMath::RandRange(-1000.0f, 1000.0f),
            0.0f);
        return FTransform(randomPos, FRotator::Zero, FVector3(1, 1, 1));
    }

private:
    APlayerCharacter* playerPawn;
    APlayerController* playerController;
    TVector<AAICharacter*> aiPawns;

    Float32 respawnDelay;
    Int32 numAIEnemies;
};
```

## Possession System

### Controller-Pawn Relationship

The possession system establishes a relationship between controllers and pawns:

```cpp
class AController {
private:
    APawn* possessedPawn;

public:
    void Possess(APawn* pawn) {
        if (possessedPawn) {
            UnPossess();
        }

        possessedPawn = pawn;
        pawn->OnPossessed(this);
        OnPossessionChanged(pawn);
    }

    void UnPossess() {
        if (possessedPawn) {
            APawn* oldPawn = possessedPawn;
            possessedPawn = nullptr;
            oldPawn->OnUnPossessed();
            OnPossessionChanged(nullptr);
        }
    }
};
```

### Input Flow

Input flows from controller to pawn through possession:

```
Input Device ──► Controller ──► Pawn ──► Movement/Action
     │               │            │           │
     ▼               ▼            ▼           ▼
Raw Input     Input Processing  Input Handling  Game Logic
```

### Authority and Ownership

- **Server Authority**: Server has final authority over pawn state
- **Client Prediction**: Clients predict movement for responsiveness
- **Ownership**: Controllers own their possessed pawns
- **Synchronization**: Pawn state synchronized across network

## Networking

### Replication

Pawns are always replicated for network synchronization:

```cpp
APawn::APawn(void)
    : AActor("APawn")
{
    // Pawns are always replicated
    SetTransformReplicated(true);
}
```

### Movement Synchronization

Pawns use the actor's movement prediction system:

- **Client Prediction**: Local movement simulation
- **Server Validation**: Authoritative server movement
- **Reconciliation**: Correction of prediction errors
- **Interpolation**: Smooth remote pawn movement

### Possession Replication

Possession state is synchronized across network:

```cpp
class APawn {
    UProperty<AController*> Controller;  // Replicated possession state

    void OnPossessed(AController* newController) {
        Controller = newController;
        // Replication will synchronize to clients
    }
};
```

## Performance Considerations

### Memory Usage

- **Base Overhead**: Minimal additional memory beyond `AActor`
- **Component Storage**: Pawn-specific components add memory
- **Network State**: Movement prediction buffers consume memory
- **AI State**: AI pawns may have additional state storage

### CPU Performance

- **Input Processing**: Input handling every frame when possessed
- **Movement Simulation**: Physics and movement calculations
- **AI Updates**: AI logic processing for AI-controlled pawns
- **Network Synchronization**: Replication and prediction overhead

### Optimization Strategies

1. **Component Pooling**: Reuse pawn components when possible
2. **LOD System**: Reduce update frequency for distant pawns
3. **Prediction Limits**: Limit prediction time to prevent divergence
4. **Network Batching**: Group pawn updates for efficient replication

### Best Practices

- **Minimal Components**: Keep pawn component count reasonable
- **Efficient Input**: Cache input values and avoid redundant processing
- **Network Awareness**: Design with network limitations in mind
- **State Synchronization**: Only replicate necessary pawn state

## Architecture Diagrams

### Pawn Class Hierarchy

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

APawn (Controllable Actor)
├── Possession: Controller ownership
├── Input Processing: Controller input handling
├── Movement Authority: Networked movement
├── Always Replicated: Network synchronization
├── Authority State: Server/client authority
└── Possession Events: OnPossessed/OnUnPossessed
```

### Possession System Architecture

```
Controller Hierarchy
├── AController (Base Controller)
│   ├── APlayerController (Player Input)
│   └── AAIController (AI Logic)
│
Pawn Hierarchy
└── APawn (Controllable Actor)
    ├── ACharacter (Player Character)
    ├── AVehicle (Vehicle Pawn)
    └── Custom Pawns (Game-specific)

Possession Relationship
Controller ◄─── Possession ───► Pawn
    │                            │
    ▼                            ▼
Input Source               Physical Entity
(Player/AI)               (In World)
```

### Input Processing Flow

```
Input Device ──► Platform Input ──► Controller ──► Pawn ──► Action
     │                │                 │            │        │
     ▼                ▼                 ▼            ▼        ▼
Raw Events     Input Mapping      Input Binding  Input Handling Game Logic
(Key/Mouse)    (Actions/Axes)     (Functions)    (Override)  (Movement)
```

### Network Synchronization

```
Client Movement Flow:
Client Input ──► Local Prediction ──► Server RPC ──► Server Validation
      │                │                      │              │
      ▼                ▼                      ▼              ▼
Immediate     Visual Update     Authoritative    State Update
Response      (No Delay)        Movement Check   (Correction)

Server Broadcast:
Server State ──► Multicast RPC ──► Client Interpolation
      │                │                      │
      ▼                ▼                      ▼
Authoritative    Network Update     Smooth Movement
Position         (All Clients)      (Remote Pawns)
```

## Troubleshooting

### Common Issues

#### Pawn Not Possessible

**Symptoms:** Controller cannot possess pawn

**Possible Causes:**
- Pawn already possessed by another controller
- Pawn not properly initialized
- Controller not valid
- Network authority issues

**Solutions:**
```cpp
// Check possession state
void DebugPossession(APawn* pawn, AController* controller) {
    if (!pawn) {
        FLogger::Log(ELogLevel::Error, "Pawn is null");
        return;
    }

    if (!controller) {
        FLogger::Log(ELogLevel::Error, "Controller is null");
        return;
    }

    if (pawn->IsPossessed()) {
        AController* currentController = pawn->GetController();
        FLogger::Log(ELogLevel::Warning,
            FString::Format("Pawn already possessed by {}", currentController->GetName()));
        return;
    }

    // Check network authority
    if (!controller->HasAuthority()) {
        FLogger::Log(ELogLevel::Warning, "Controller does not have possession authority");
    }

    // Attempt possession
    controller->Possess(pawn);

    if (pawn->GetController() == controller) {
        FLogger::Log(ELogLevel::Debug, "Possession successful");
    } else {
        FLogger::Log(ELogLevel::Error, "Possession failed");
    }
}
```

#### Input Not Working on Pawn

**Symptoms:** Pawn doesn't respond to controller input

**Possible Causes:**
- Input not bound in `SetupPlayerInput()`
- Controller not possessing pawn
- Input processing disabled
- Network prediction issues

**Solutions:**
```cpp
// Debug input binding
void DebugPawnInput(APawn* pawn) {
    if (!pawn->IsPossessed()) {
        FLogger::Log(ELogLevel::Error, "Pawn is not possessed");
        return;
    }

    AController* controller = pawn->GetController();
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Pawn possessed by: {}", controller->GetName()));

    // Test input binding
    if (APlayerController* pc = dynamic_cast<APlayerController*>(controller)) {
        // Check if input actions are bound
        bool hasMoveForward = pc->HasInputBinding("MoveForward");
        bool hasMoveRight = pc->HasInputBinding("MoveRight");
        bool hasJump = pc->HasInputBinding("Jump");

        FLogger::Log(ELogLevel::Debug,
            FString::Format("Input bindings - Forward: {}, Right: {}, Jump: {}",
                hasMoveForward, hasMoveRight, hasJump));
    }
}

// Verify input processing
void TestPawnInputProcessing(APawn* pawn) {
    // Simulate input
    FInputAction moveForward("MoveForward", 1.0f);
    pawn->HandleInput(moveForward, 1.0f);

    FInputAction jump("Jump", 1.0f);
    pawn->HandleInput(jump, 1.0f);

    FLogger::Log(ELogLevel::Debug, "Input processing test completed");
}
```

#### Pawn Movement Desynchronization

**Symptoms:** Pawn position differs between clients

**Possible Causes:**
- Prediction errors not reconciled
- Network latency issues
- Movement simulation differences
- Authority conflicts

**Solutions:**
```cpp
// Debug movement synchronization
void DebugPawnMovement(APawn* pawn) {
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Pawn: {}", pawn->GetName()));
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Position: ({}, {}, {})",
            pawn->GetTransform().GetPosition().x,
            pawn->GetTransform().GetPosition().y,
            pawn->GetTransform().GetPosition().z));

    if (pawn->IsPossessed()) {
        AController* controller = pawn->GetController();
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Possessed by: {}", controller->GetName()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Has Authority: {}", controller->HasAuthority()));
    }

    // Check pending moves
    FLogger::Log(ELogLevel::Debug,
        FString::Format("Pending moves: {}", pawn->GetPendingMoveCount()));
}

// Force synchronization
void ForcePawnSync(APawn* pawn) {
    if (pawn->IsPossessed()) {
        AController* controller = pawn->GetController();
        if (controller->HasAuthority()) {
            // Force position update
            pawn->ForceNetUpdate();
        }
    }
}
```

#### AI Pawn Not Behaving Correctly

**Symptoms:** AI-controlled pawns not following expected behavior

**Possible Causes:**
- AI controller not properly configured
- Behavior tree issues
- Perception system problems
- State transition bugs

**Solutions:**
```cpp
// Debug AI pawn behavior
void DebugAIPawn(AAICharacter* aiPawn) {
    if (!aiPawn->IsPossessed()) {
        FLogger::Log(ELogLevel::Error, "AI pawn is not possessed");
        return;
    }

    AController* controller = aiPawn->GetController();
    if (AAIController* aiController = dynamic_cast<AAIController*>(controller)) {
        FLogger::Log(ELogLevel::Debug,
            FString::Format("AI Controller: {}", aiController->GetName()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("AI State: {}", aiController->GetCurrentState()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Target: {}",
                aiController->GetTarget() ? aiController->GetTarget()->GetName() : "None"));
    }

    // Check perception
    UPerceptionComponent* perception = aiPawn->GetComponent<UPerceptionComponent>("Perception");
    if (perception) {
        TArray<AActor*> perceived = perception->GetPerceivedActors();
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Perceived actors: {}", perceived.size()));
    }
}

// Reset AI state
void ResetAIPawn(AAICharacter* aiPawn) {
    aiPawn->GetController()->UnPossess();

    AAIController* newController = GetWorld()->SpawnActor<AAIController>("AIController");
    newController->Possess(aiPawn);

    FLogger::Log(ELogLevel::Debug, "AI pawn reset");
}
```

### Performance Issues

**Symptoms:** Low frame rate with many pawns

**Possible Causes:**
- Too many pawns updating simultaneously
- Expensive AI calculations
- Network overhead
- Physics simulation cost

**Solutions:**
```cpp
// Implement pawn culling
class PawnCullingSystem {
public:
    void UpdateCulling(const FVector3& cameraPosition, Float32 maxDistance) {
        for (APawn* pawn : allPawns) {
            Float32 distance = (pawn->GetTransform().GetPosition() - cameraPosition).Length();

            bool shouldBeActive = distance <= maxDistance;
            if (pawn->IsActive() != shouldBeActive) {
                pawn->SetActive(shouldBeActive);

                // Disable AI for distant pawns
                if (!shouldBeActive && pawn->IsPossessed()) {
                    if (AAIController* ai = dynamic_cast<AAIController*>(pawn->GetController())) {
                        ai->SetAIEnabled(false);
                    }
                }
            }
        }
    }
};

// Profile pawn performance
class PawnProfiler {
public:
    void ProfilePawns(const TVector<APawn*>& pawns) {
        for (APawn* pawn : pawns) {
            auto start = std::chrono::high_resolution_clock::now();

            pawn->Tick(1.0f / 60.0f);

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            if (duration.count() > 5000) { // > 5ms
                FLogger::Log(ELogLevel::Warning,
                    FString::Format("Slow pawn tick: {} ({}us)", pawn->GetName(), duration.count()));
            }
        }
    }
};
```

### Debug Tools

Enable comprehensive pawn debugging:

```cpp
class PawnDebugger {
public:
    static void LogPawnState(APawn* pawn) {
        FLogger::Log(ELogLevel::Debug, "=== Pawn Debug Info ===");
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Name: {}", pawn->GetName()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Active: {}", pawn->IsActive()));
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Possessed: {}", pawn->IsPossessed()));

        if (pawn->IsPossessed()) {
            AController* controller = pawn->GetController();
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Controller: {}", controller->GetName()));
            FLogger::Log(ELogLevel::Debug,
                FString::Format("Controller Type: {}",
                    dynamic_cast<APlayerController*>(controller) ? "Player" : "AI"));
        }

        FTransform transform = pawn->GetTransform();
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Position: ({}, {}, {})",
                transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z));

        auto components = pawn->GetComponents<UActorComponent>();
        FLogger::Log(ELogLevel::Debug,
            FString::Format("Components: {}", components.size()));

        for (size_t i = 0; i < components.size(); ++i) {
            FLogger::Log(ELogLevel::Debug,
                FString::Format("  {}: {} (Active: {})",
                    i, components[i]->GetName(), components[i]->IsActive()));
        }
    }

    static void VisualizePawnMovement(APawn* pawn, UCanvas* canvas) {
        // Draw pawn movement vectors
        FVector3 position = pawn->GetTransform().GetPosition();
        FVector3 velocity = pawn->GetVelocity();

        if (velocity.Length() > 0.1f) {
            FVector2 screenPos = WorldToScreen(position);
            FVector2 velocityEnd = WorldToScreen(position + velocity);

            canvas->DrawLine(screenPos, velocityEnd, FColor::Red, 2.0f);
            canvas->DrawText("Velocity", velocityEnd, nullptr, FColor::Red);
        }
    }

    static void TestPawnPossession(APawn* pawn, AController* controller) {
        FLogger::Log(ELogLevel::Debug, "Testing pawn possession...");

        // Unpossess if already possessed
        if (pawn->IsPossessed()) {
            pawn->GetController()->UnPossess();
        }

        // Attempt possession
        controller->Possess(pawn);

        // Verify
        if (pawn->GetController() == controller) {
            FLogger::Log(ELogLevel::Debug, "Possession test passed");

            // Test input
            FInputAction testInput("TestAction", 1.0f);
            pawn->HandleInput(testInput, 1.0f);

            FLogger::Log(ELogLevel::Debug, "Input test completed");
        } else {
            FLogger::Log(ELogLevel::Error, "Possession test failed");
        }
    }

    static void ValidatePawnHierarchy(const TVector<APawn*>& pawns) {
        for (APawn* pawn : pawns) {
            // Check possession consistency
            if (pawn->IsPossessed()) {
                AController* controller = pawn->GetController();
                if (!controller) {
                    FLogger::Log(ELogLevel::Error,
                        FString::Format("Pawn {} has invalid controller", pawn->GetName()));
                } else if (controller->GetPawn() != pawn) {
                    FLogger::Log(ELogLevel::Error,
                        FString::Format("Possession relationship broken for {}", pawn->GetName()));
                }
            }

            // Check component validity
            auto components = pawn->GetComponents<UActorComponent>();
            for (UActorComponent* component : components) {
                if (!component) {
                    FLogger::Log(ELogLevel::Error,
                        FString::Format("Pawn {} has null component", pawn->GetName()));
                }
            }
        }
    }
};
```

## Future Enhancements

### Planned Features

1. **Advanced AI**: Behavior trees, navigation meshes, and crowd simulation
2. **Physics Integration**: Ragdoll physics and advanced collision detection
3. **Multi-Possession**: Multiple controllers possessing single pawn
4. **Pawn Templates**: Blueprint-like pawn creation system
5. **Network Optimization**: Delta compression and prediction improvements
6. **Animation System**: Advanced animation blending and state machines
7. **Interaction System**: Pawn-to-pawn and pawn-to-world interactions
8. **Customization**: Runtime pawn appearance and ability modification

### Extension Points

The pawn system allows for easy extension:

```cpp
class AdvancedPawn : public APawn {
public:
    // Add custom possession logic
    virtual void OnPossessed(AController* newController) override {
        APawn::OnPossessed(newController);
        // Custom possession behavior
    }

    // Add advanced input processing
    virtual void SetupPlayerInput(AController* controller) override {
        APawn::SetupPlayerInput(controller);
        // Additional input setup
    }

    // Add custom movement
    virtual FTransform SimulateMovement(const FVector3& inputVector,
                                      Float32 deltaTime,
                                      const FTransform& startTransform) override {
        // Enhanced movement simulation
        return APawn::SimulateMovement(inputVector, deltaTime, startTransform);
    }

    // Add pawn abilities
    void AddAbility(UPawnAbility* ability) {
        abilities.push_back(ability);
    }

    void RemoveAbility(UPawnAbility* ability) {
        // Remove ability logic
    }

private:
    TVector<UPawnAbility*> abilities;
};

// Custom pawn types
class AFlyingPawn : public APawn {
    // Flight-specific logic
};

class AWaterPawn : public APawn {
    // Swimming-specific logic
};

class ASpacePawn : public APawn {
    // Zero-gravity movement logic
};
```

### Advanced Features

Future versions may include:
- **Cooperative Control**: Multiple players controlling single pawn
- **Dynamic Possession**: Runtime possession changes during gameplay
- **Pawn Morphing**: Shape-shifting and form-changing pawns
- **Holographic Pawns**: Non-physical, projection-based pawns
- **Time Manipulation**: Time-controlled pawn behavior
- **Dimensional Pawns**: Cross-dimensional pawn interactions
- **Neural Control**: AI-assisted player control systems

---

*This documentation covers the complete APawn implementation as of TKD Engine v1.0.0. For the latest updates and additional examples, refer to the engine's source code and test suites.*
