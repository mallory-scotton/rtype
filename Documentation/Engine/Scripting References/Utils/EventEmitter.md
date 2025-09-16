# EventEmitter

## Description

The `TEventEmitter` class template provides a flexible and type-safe event system in C++. It allows classes to emit events and register listeners (callbacks) that respond to those events. The system supports both queued event processing and immediate triggering, persistent and one-time listeners, and compile-time type safety through template metaprogramming.

Events are processed through a queue system, allowing for deferred execution, or can be triggered immediately. The implementation uses `std::variant` for type-safe event storage and `std::function` for callback management.

## Template Parameters

- **EventTypes**: Variadic template parameters representing all possible event types that can be emitted by this emitter. Each event type must be a struct or class.

## Types

### ListenerHandle
```cpp
using ListenerHandle = UInt64;
```
- **Description**: A unique identifier for managing event listeners. Returned by `On()` and `Once()` methods and used to remove specific listeners.

### Event (Nested Class)
```cpp
class Event
```
- **Description**: A wrapper class that stores event data using `std::variant<EventTypes...>`. Provides type-safe access methods.
- **Key Methods**:
  - `template<typename T> bool Is() const` - Check if event is of type T
  - `template<typename T> T* GetIf()` - Get event as type T (returns nullptr if wrong type)
  - `template<typename T> const T* GetIf() const` - Const version
  - `template<typename T> T& Get()` - Get event as type T (throws if wrong type)
  - `template<typename T> const T& Get() const` - Const version
  - `template<typename Visitor> auto Visit(Visitor&& visitor)` - Apply visitor pattern

## Constructors and Destructor

### Destructor
```cpp
virtual ~TEventEmitter() = default;
```
- **Description**: Virtual destructor for proper cleanup in inheritance hierarchies.
- **Access**: Protected

## Public Methods

### PollEvent
```cpp
std::optional<Event> PollEvent(void)
```
- **Description**: Retrieves and removes the next event from the queue.
- **Return Type**: `std::optional<Event>` - The event if available, empty optional if queue is empty
- **Thread Safety**: Not thread-safe (should be called from the same thread that emits events)

### HasEvents
```cpp
bool HasEvents(void) const
```
- **Description**: Checks if there are any events in the queue.
- **Return Type**: `bool` - True if events are available

### GetEventCount
```cpp
size_t GetEventCount(void) const
```
- **Description**: Returns the number of events currently in the queue.
- **Return Type**: `size_t` - Number of queued events

### ClearEvents
```cpp
void ClearEvents(void)
```
- **Description**: Removes all events from the queue without processing them.
- **Return Type**: `void`

### On (Persistent Listener)
```cpp
template <typename T>
ListenerHandle On(std::function<void(const T&)> callback)
```
- **Description**: Registers a persistent event listener that will be called every time an event of type T is triggered.
- **Template Parameters**:
  - **T**: The event type to listen for (must be in EventTypes...)
- **Parameters**:
  - **callback**: Function to call when event is triggered
- **Return Type**: `ListenerHandle` - Handle for removing the listener
- **Static Assertion**: Ensures T is a valid event type

### Once (One-time Listener)
```cpp
template <typename T>
ListenerHandle Once(std::function<void(const T&)> callback)
```
- **Description**: Registers a one-time event listener that will be called only once when an event of type T is triggered, then automatically removed.
- **Template Parameters**:
  - **T**: The event type to listen for (must be in EventTypes...)
- **Parameters**:
  - **callback**: Function to call when event is triggered
- **Return Type**: `ListenerHandle` - Handle for removing the listener (if needed before trigger)
- **Static Assertion**: Ensures T is a valid event type

### RemoveListener
```cpp
bool RemoveListener(ListenerHandle handle)
```
- **Description**: Removes a specific event listener by its handle.
- **Parameters**:
  - **handle**: The handle returned by `On()` or `Once()`
- **Return Type**: `bool` - True if listener was found and removed

### RemoveAllListeners (Specific Type)
```cpp
template <typename T>
void RemoveAllListeners()
```
- **Description**: Removes all listeners for a specific event type.
- **Template Parameters**:
  - **T**: The event type to remove listeners for
- **Return Type**: `void`

### RemoveAllListeners (All Types)
```cpp
void RemoveAllListeners()
```
- **Description**: Removes all listeners for all event types.
- **Return Type**: `void`

### GetListenerCount
```cpp
template <typename T>
size_t GetListenerCount() const
```
- **Description**: Returns the number of listeners registered for a specific event type.
- **Template Parameters**:
  - **T**: The event type to count listeners for
- **Return Type**: `size_t` - Number of listeners

## Protected Methods

### EmitEvent
```cpp
template <typename T>
void EmitEvent(const T& eventData)

template <typename T>
void EmitEvent(T&& eventData)
```
- **Description**: Adds an event to the queue for later processing. The event will be processed when `PollEvent()` is called.
- **Template Parameters**:
  - **T**: The event type to emit (must be in EventTypes...)
- **Parameters**:
  - **eventData**: The event data to queue
- **Return Type**: `void`
- **Static Assertion**: Ensures T is a valid event type

### TriggerEvent
```cpp
template <typename T>
void TriggerEvent(const T& eventData)

template <typename T>
void TriggerEvent(T&& eventData)
```
- **Description**: Immediately triggers all listeners for an event type, bypassing the queue.
- **Template Parameters**:
  - **T**: The event type to trigger (must be in EventTypes...)
- **Parameters**:
  - **eventData**: The event data to trigger
- **Return Type**: `void`
- **Static Assertion**: Ensures T is a valid event type

### EmitAndTrigger
```cpp
template <typename T>
void EmitAndTrigger(const T& eventData)

template <typename T>
void EmitAndTrigger(T&& eventData)
```
- **Description**: Both adds an event to the queue and immediately triggers listeners.
- **Template Parameters**:
  - **T**: The event type to emit and trigger (must be in EventTypes...)
- **Parameters**:
  - **eventData**: The event data to emit and trigger
- **Return Type**: `void`
- **Static Assertion**: Ensures T is a valid event type

## Usage Patterns

### Basic Event Definition
```cpp
// Define event types
struct KeyPressed {
    int keyCode;
    bool shiftPressed;
};

struct MouseClicked {
    int x, int y;
    int button;
};

// Create emitter
class MyInputHandler : public TEventEmitter<KeyPressed, MouseClicked> {
public:
    void ProcessInput() {
        // Emit events to queue
        EmitEvent(KeyPressed{42, false});

        // Process queued events
        while (auto event = PollEvent()) {
            if (event->Is<KeyPressed>()) {
                const auto* keyEvent = event->GetIf<KeyPressed>();
                HandleKeyPress(*keyEvent);
            }
        }
    }
};
```

### Listener Registration
```cpp
class Game : public TEventEmitter<KeyPressed, MouseClicked> {
public:
    void Initialize() {
        // Persistent listener
        auto keyHandle = On<KeyPressed>([this](const KeyPressed& event) {
            std::cout << "Key " << event.keyCode << " pressed" << std::endl;
        });

        // One-time listener
        Once<MouseClicked>([this](const MouseClicked& event) {
            std::cout << "First click at " << event.x << "," << event.y << std::endl;
        });
    }

    void HandleInput() {
        // Immediate triggering
        TriggerEvent(MouseClicked{100, 200, 1});
    }
};
```

### Event Processing Loop
```cpp
void MainLoop(TEventEmitter<KeyPressed, MouseClicked>& emitter) {
    while (running) {
        // Process all queued events
        while (auto event = emitter.PollEvent()) {
            event->Visit([](const auto& e) {
                using EventType = std::decay_t<decltype(e)>;
                if constexpr (std::is_same_v<EventType, KeyPressed>) {
                    HandleKeyEvent(e);
                } else if constexpr (std::is_same_v<EventType, MouseClicked>) {
                    HandleMouseEvent(e);
                }
            });
        }

        // Other game logic...
    }
}
```

### TEvents Pattern (Recommended)
```cpp
// Define events in a struct for better organization
template <>
struct TEvents<MyClass> {
    struct ButtonClicked { int buttonId; };
    struct ValueChanged { float newValue; };

    using All = std::tuple<ButtonClicked, ValueChanged>;
};

// Use with TEventEmitter
class MyClass : public TEventEmitter<TEvents<MyClass>::All> {
    // Now you can use ButtonClicked and ValueChanged events
};
```

## Notes

- Events are processed in FIFO (First-In-First-Out) order from the queue.
- Listeners are called in the order they were registered.
- One-time listeners are automatically removed after being called once.
- The system is not thread-safe by default; synchronization should be handled externally if needed.
- Event types must be copyable or movable (depending on usage).
- The `TEvents<T>::All` pattern is recommended for organizing event types in a central location.
- Memory management is automatic; listeners are cleaned up when the emitter is destroyed.
