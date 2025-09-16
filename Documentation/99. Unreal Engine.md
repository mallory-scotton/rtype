# Unreal Engine: Runtime Flow & Technical Architecture

This document provides a detailed overview of the core systems of Unreal Engine, explaining how its object model, world structure, player control, UI, and networking systems interact at runtime.

-----

## The Core Object Model

At the heart of Unreal Engine is a powerful object model built on a custom reflection system. This system allows for features like garbage collection, serialization, network replication, and Blueprint integration.

### `UObject` and `UClass`

  - **`UObject`**: The base class for almost everything in the engine. It provides fundamental features like reflection, garbage collection, and metadata support.
  - **`UClass`**: A special object that holds the runtime "type information" for any class derived from `UObject`. Think of it as C++'s `type_info` on steroids. When you use the `UCLASS()` macro, the **Unreal Header Tool (UHT)** parses your header file and generates reflection data. This data allows the engine to understand your class's properties (`UPROPERTY`), functions (`UFUNCTION`), and inheritance structure at runtime.

### `AActor`

  - An **`AActor`** is the fundamental building block of a game level. Anything that can be placed in the world—like characters, static meshes, lights, and cameras—is an `AActor`.
  - **Key features of `AActor`**:
      * It has a **transform** (location, rotation, scale).
      * It can be **replicated** over the network.
      * It can **tick** (execute code) every frame.
      * It is managed by the game world (`UWorld`).
  - An `AActor` acts primarily as a **container** for functionality, which is provided by Components.

### `UActorComponent`

  - A **`UActorComponent`** is a modular piece of functionality that can be attached to an `AActor`. Components are what "do the work."
  - **`USceneComponent`**: A special type of `UActorComponent` that has its own transform. This allows components to be arranged in a hierarchy within an Actor (e.g., attaching a camera to a character's head).
  - **Examples**:
      * `UStaticMeshComponent`: Renders a 3D model.
      * `UCharacterMovementComponent`: Handles character physics like walking and jumping.
      * `UAudioComponent`: Plays sounds.

### Analogy 🚗

  - **`UClass`**: The blueprint or design schematic of a car.
  - **`AActor`**: The car itself, placed in the world.
  - **`UActorComponent`**: The individual parts of the car, like the `UStaticMeshComponent` (the body), `UAudioComponent` (the engine sound), and a movement component (the wheels and engine).

-----

## The World Hierarchy

Actors and their components don't exist in a vacuum; they live inside a structured world hierarchy.

### `UWorld`

  - The **`UWorld`** is the top-level object representing a running game instance. It manages the entire simulation.
  - **Responsibilities**:
      * Manages the game loop (ticking Actors and Components).
      * Keeps track of all active `AActor`s.
      * Handles spawning and destroying Actors.
      * Oversees engine subsystems like physics, rendering, and networking for that specific world.

### `ULevel`

  - A **`ULevel`** is simply a container for a collection of `AActor`s. When you create a map in the editor, you are creating a `ULevel` asset.
  - A `UWorld` can manage multiple `ULevel`s at once:
      * **Persistent Level**: The main level that is always loaded.
      * **Streaming Levels**: Additional levels that can be loaded and unloaded dynamically to create large, seamless worlds.

### Hierarchy Analogy 🌍

  - **`UWorld`**: The entire running universe of your game session.
  - **`ULevel`**: A specific zone or area within that universe (e.g., a city, a forest, a dungeon).
  - **`AActor`**: An individual object located within a level.

-----

## Player Control and Input

### `APlayerController`

  - The **`APlayerController`** is an `AActor` that represents the "mind" or will of a human player. It is the bridge between player input and the game world.
  - **Responsibilities**:
      * Receives raw input from the keyboard, mouse, or gamepad.
      * Manages the player's camera and view.
      * Owns and displays UI elements like HUDs and menus.
      * **Possesses** an `APawn` to control it.
  - On a server, there is one `PlayerController` for each connected player. In a local split-screen game, multiple `PlayerController`s can exist on one machine.

### `APawn`

  - An **`APawn`** is an `AActor` that can be possessed and controlled by a `Controller` (either a `PlayerController` for a human or an `AIController` for AI). It is the player's physical representation or "body" in the world.
  - **`ACharacter`** is a specialized type of `Pawn` that includes a `UCharacterMovementComponent` by default, making it ideal for humanoid entities that can walk, jump, and swim.

### Input Flow

1.  **Keybindings** are defined in configuration files (`DefaultInput.ini`) or through the **Enhanced Input** asset system.
2.  The operating system detects a key press.
3.  The local **`APlayerController`** receives this input.
4.  The controller's **`InputComponent`** translates the raw input (e.g., "W key") into a mapped action (e.g., "MoveForward").
5.  This action triggers a function bound on the possessed **`APawn`**.
6.  The `Pawn`'s `MovementComponent` executes the logic, moving the `Pawn` in the `UWorld`.

-----

## User Interface (UI)

UI in Unreal is generally an overlay system managed by the `PlayerController` and is separate from the 3D world simulation.

  - **`AHUD`**: An `AActor` for drawing simple, direct-to-screen debug information. This is a legacy system.
  - **Slate**: The low-level, C++-driven UI framework used to build both the Unreal Editor and in-game UI.
  - **UMG (Unreal Motion Graphics)**: The modern, designer-friendly framework built on top of Slate. UI elements are created as **`UUserWidget`** assets (Widget Blueprints) and can be added to the screen.
  - **`UWidgetComponent`**: A component that allows a `UUserWidget` to be rendered in 3D space within the world, useful for diegetic UI like computer screens or holographic displays.

-----

## Multiplayer & Replication

Unreal Engine uses a server-authoritative network model.

  - **Core Principle**: Input is captured on the **client**, but the **server** executes the resulting action and replicates the outcome to all clients. This prevents cheating.

### How Actions are Replicated

1.  **Movement Input**: Handled automatically by the `UCharacterMovementComponent`. It uses a highly optimized system of client-side prediction and server reconciliation to feel responsive while remaining server-authoritative.

2.  **Gameplay Actions** (e.g., firing a weapon, opening a door): Handled via **Remote Procedure Calls (RPCs)**.

      * A client-side function is marked with `UFUNCTION(Server, ...)`. When called, it executes on the server.
      * The server performs the action (e.g., spawns a projectile, reduces ammo).
      * The server replicates the results back to clients using:
          * **Replicated Properties**: Variables marked with `UPROPERTY(Replicated)` are automatically synchronized from server to clients.
          * **Multicast RPCs**: Functions marked with `UFUNCTION(NetMulticast, ...)` are called by the server and execute on the server and all connected clients, perfect for cosmetic effects like explosions or sounds.

<!-- end list -->

```cpp
// Example of a client telling the server to fire a weapon
void AMyCharacter::Fire()
{
    // This is called by local player input.
    // We then call the Server RPC to run the logic on the server.
    Server_FireWeapon();
}

UFUNCTION(Server, Reliable)
void AMyCharacter::Server_FireWeapon()
{
    // This code ONLY runs on the server.
    // The server spawns the projectile, applies damage, etc.
    // The result (e.g., the projectile) will be replicated to all clients.
}
```

-----

## The Reflection System: `UCLASS`, `UFUNCTION`, `UPROPERTY`

Because C++ lacks built-in runtime reflection, Unreal created its own system to power its core features.

### How It Works

1.  **Macros**: You decorate your C++ code with macros like `UCLASS()`, `UPROPERTY()`, and `UFUNCTION()`.
2.  **Unreal Header Tool (UHT)**: During compilation, this custom tool parses your header files, looking for these macros.
3.  **Code Generation**: UHT generates a `.generated.h` file for each header. This file contains the boilerplate C++ code and metadata tables that describe your class structure, properties, and functions.
4.  **Runtime Registration**: This generated code is compiled with your project and allows the engine to have a complete runtime understanding of your types, enabling features like:
      * Details panel editing in the editor.
      * Blueprints.
      * Garbage collection.
      * Serialization (saving/loading).
      * Network replication.

It is possible to build a similar reflection system in a custom engine, either by creating a custom header parser like UHT or by using modern C++20 metaprogramming features to achieve a similar result without a separate build step.
