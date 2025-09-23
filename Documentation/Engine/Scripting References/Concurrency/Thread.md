# Thread

## Overview

The TKD Engine provides thread management through the `IThread` interface and its implementation `FThread`. This abstraction simplifies thread creation and lifecycle management while providing access to standard C++ thread functionality.

## IThread Interface

The `IThread` interface defines the contract for thread implementations.

### Methods

#### `virtual void Start(void) = 0`

Starts the thread execution. The thread will begin executing the associated function.

#### `virtual void Join(void) = 0`

Blocks the calling thread until this thread finishes execution.

#### `virtual void Detach(void) = 0`

Detaches the thread from the calling thread, allowing it to run independently.

#### `virtual bool Joinable(void) const = 0`

Checks if the thread can be joined.

- **Returns:** `true` if the thread is joinable, `false` otherwise.

## FThread Implementation

`FThread` implements thread management using `std::thread`.

### Features

- **Function Execution**: Runs `std::function<void()>` in a separate thread
- **RAII Design**: Automatic cleanup in destructor
- **Exception Safety**: Handles thread creation and joining exceptions
- **Status Tracking**: Tracks thread lifecycle state
- **Atomic Running Flag**: Thread-safe status indication

### Constructor

#### `explicit FThread(std::function<void()> function)`

Creates a thread that will execute the provided function.

- **Parameters:**
  - `function`: Function to execute in the thread

### Public Members

#### `std::atomic<bool> running`

Thread-safe flag indicating if the thread is currently running.

- **Access:** Can be checked from any thread without synchronization

### Usage Examples

#### Basic Thread Creation

```cpp
#include <Engine/Core/Concurrency/FThread.hpp>

void WorkerFunction() {
    std::cout << "Thread executing" << std::endl;
    // Do work here
}

tkd::FThread thread(WorkerFunction);
thread.Start();
thread.Join();  // Wait for completion
```

#### Parallel Processing

```cpp
#include <Engine/Core/Concurrency/FThread.hpp>
#include <vector>

std::vector<tkd::FThread> threads;

for (int i = 0; i < 4; ++i) {
    threads.emplace_back([i]() {
        std::cout << "Worker " << i << " processing" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    });
}

// Start all threads
for (auto& thread : threads) {
    thread.Start();
}

// Wait for all to complete
for (auto& thread : threads) {
    thread.Join();
}
```

#### Detach Pattern

```cpp
#include <Engine/Core/Concurrency/FThread.hpp>

tkd::FThread backgroundThread([]() {
    while (true) {
        // Background work
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
});

backgroundThread.Start();
backgroundThread.Detach();  // Run independently

// Main thread continues without waiting
```

#### Status Monitoring

```cpp
#include <Engine/Core/Concurrency/FThread.hpp>

tkd::FThread thread([]() {
    std::this_thread::sleep_for(std::chrono::seconds(5));
});

thread.Start();

// Check if thread is joinable
if (thread.Joinable()) {
    std::cout << "Thread can be joined" << std::endl;
}

// Check running status (thread-safe)
if (thread.running.load()) {
    std::cout << "Thread is still running" << std::endl;
}

thread.Join();
```

#### Exception Handling

```cpp
#include <Engine/Core/Concurrency/FThread.hpp>

try {
    tkd::FThread thread([]() {
        throw std::runtime_error("Something went wrong");
    });

    thread.Start();
    thread.Join();
} catch (const std::exception& e) {
    std::cout << "Thread error: " << e.what() << std::endl;
}
```

## Thread Lifecycle

### States

1. **Created**: Thread object instantiated with function
2. **Started**: `Start()` called, thread begins execution
3. **Running**: Function executing in thread context
4. **Joinable**: Thread finished but not yet joined
5. **Joined/Detached**: Thread resources cleaned up

### Join vs Detach

#### Join
- **Blocking**: Waits for thread completion
- **Resource Management**: Ensures proper cleanup
- **Exception Propagation**: Can handle thread exceptions
- **Use Case**: When parent thread needs thread result or completion confirmation

#### Detach
- **Non-blocking**: Thread runs independently
- **Resource Management**: Thread cleans up automatically on completion
- **Exception Handling**: Exceptions in detached threads terminate the program
- **Use Case**: Fire-and-forget background tasks

## Error Handling

### Common Exceptions

- **Already Started**: Calling `Start()` on an already started thread
- **No Function**: Starting a thread without a function
- **Join/Detach Failures**: System-level thread management errors

### Exception Safety

```cpp
tkd::FThread thread(workerFunction);

try {
    thread.Start();
    // Thread is running
} catch (const std::runtime_error& e) {
    // Handle start failure
    std::cout << "Failed to start thread: " << e.what() << std::endl;
}

try {
    thread.Join();
} catch (const std::runtime_error& e) {
    // Handle join failure
    std::cout << "Failed to join thread: " << e.what() << std::endl;
}
```

## Thread Safety

### Guarantees

- **Construction**: Thread-safe
- **Start/Join/Detach**: Not thread-safe (call from single thread)
- **Running Flag**: Thread-safe atomic access
- **Joinable Check**: Thread-safe

### Best Practices

- **Single Responsibility**: Each thread should have one clear purpose
- **Avoid Shared State**: Minimize shared mutable data
- **Use Synchronization**: Protect shared data with mutexes
- **Handle Exceptions**: Don't let exceptions escape thread functions
- **Resource Management**: Use RAII for thread-local resources

## Performance Considerations

### Overhead

- **Creation Cost**: Stack allocation, OS thread creation
- **Context Switching**: Kernel scheduling overhead
- **Memory Usage**: Separate stack for each thread

### Optimization Tips

- **Thread Pools**: Reuse threads for similar tasks
- **Minimize Threads**: Use fewer threads than CPU cores for CPU-bound work
- **I/O Bound**: Use more threads for I/O operations
- **Stack Size**: Consider custom stack sizes for memory-constrained environments

## Platform Notes

### Linux
- **Implementation**: Uses POSIX threads (pthreads)
- **Stack Size**: Default 8MB, configurable
- **Scheduling**: CFS (Completely Fair Scheduler)

### macOS
- **Implementation**: Uses Grand Central Dispatch (GCD) internally
- **Stack Size**: Default 512KB, grows as needed
- **Optimizations**: Apple-specific thread optimizations

### Windows
- **Implementation**: Uses Windows threads
- **Stack Size**: Default 1MB, configurable
- **Scheduling**: Windows thread scheduler

## Integration with Engine Systems

### Network Threads

```cpp
// In FNetworkBase
m_networkThread = std::make_unique<FThread>([this]() {
    RunNetworkThread();
});
m_networkThread->Start();
```

### Event Systems

```cpp
// Background event processing
tkd::FThread eventThread([this]() {
    while (running.load()) {
        ProcessEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
});
eventThread.Start();
eventThread.Detach();
```

## Future Enhancements

### Planned Features

- **Thread Pools**: Managed pool of reusable threads
- **Thread Priorities**: Set thread scheduling priorities
- **Thread Names**: Debugging support with named threads
- **Thread Local Storage**: Engine-managed TLS
- **Cancellation Tokens**: Cooperative thread cancellation
- **Thread Monitoring**: Performance and health monitoring
