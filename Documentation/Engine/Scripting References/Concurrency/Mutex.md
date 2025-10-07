# Mutex

## Overview

The TKD Engine provides a mutex abstraction through the `IMutex` interface and its implementation `FMutex`. This abstraction allows for consistent mutex usage across the engine while providing access to the underlying `std::mutex` for advanced use cases.

## IMutex Interface

The `IMutex` interface defines the contract for mutex implementations in the TKD Engine.

### Methods

#### `virtual operator std::mutex&() = 0`

Conversion operator to access the underlying `std::mutex`.

#### `virtual void Lock(void) = 0`

Locks the mutex. Blocks until the lock is acquired.

#### `virtual void Unlock(void) = 0`

Unlocks the mutex. Must be called by the thread that locked it.

#### `virtual bool TryLock(void) = 0`

Attempts to lock the mutex without blocking.

- **Returns:** `true` if the lock was acquired, `false` otherwise.

#### `virtual std::mutex& GetNativeMutex(void) = 0`

Provides direct access to the underlying `std::mutex` for advanced usage.

## FMutex Implementation

`FMutex` is the standard implementation of `IMutex` using `std::mutex`.

### Features

- **Thread-Safe**: Uses `std::mutex` for reliable synchronization
- **Non-Copyable**: Prevents accidental copying of mutexes
- **RAII Compatible**: Works with `std::lock_guard` and `std::unique_lock`
- **Exception Safe**: Destructor handles cleanup automatically

### Usage Examples

#### Basic Locking

```cpp
#include <Engine/Core/Concurrency/FMutex.hpp>

tkd::FMutex mutex;
std::vector<int> sharedData;

// Thread-safe access
void AddToSharedData(int value) {
    mutex.Lock();
    sharedData.push_back(value);
    mutex.Unlock();
}
```

#### RAII Locking (Recommended)

```cpp
#include <Engine/Core/Concurrency/FMutex.hpp>
#include <mutex>  // for std::lock_guard

tkd::FMutex mutex;
std::vector<int> sharedData;

void AddToSharedData(int value) {
    std::lock_guard<tkd::FMutex> lock(mutex);
    sharedData.push_back(value);
    // Automatically unlocked when lock goes out of scope
}
```

#### Try-Lock Pattern

```cpp
#include <Engine/Core/Concurrency/FMutex.hpp>

tkd::FMutex mutex;

void TryUpdateData(int value) {
    if (mutex.TryLock()) {
        // Update data
        sharedData.push_back(value);
        mutex.Unlock();
    } else {
        // Could not acquire lock, handle gracefully
        std::cout << "Resource busy, try again later" << std::endl;
    }
}
```

#### Using with Standard Library

```cpp
#include <Engine/Core/Concurrency/FMutex.hpp>
#include <mutex>

tkd::FMutex mutex;

// Access native mutex for std::condition_variable
std::condition_variable cv;

void WaitForCondition() {
    std::unique_lock<tkd::FMutex> lock(mutex);
    cv.wait(lock, []{ return conditionMet; });
}
```

## Thread Safety

### Guarantees

- **Mutual Exclusion**: Only one thread can hold the lock at a time
- **Memory Visibility**: Changes made by one thread are visible to others after unlock
- **Exception Safety**: RAII usage prevents deadlocks from exceptions

### Best Practices

#### Always Use RAII

```cpp
// ✅ Good: RAII ensures unlock
{
    std::lock_guard<tkd::FMutex> lock(mutex);
    // Critical section
} // Automatically unlocked

// ❌ Bad: Manual lock/unlock prone to errors
mutex.Lock();
// Critical section - what if exception here?
mutex.Unlock(); // May not be reached
```

#### Avoid Nested Locks

```cpp
// ❌ Risky: Can cause deadlocks
void FunctionA() {
    std::lock_guard<tkd::FMutex> lock1(mutex1);
    FunctionB(); // Calls FunctionB which locks mutex2
}

void FunctionB() {
    std::lock_guard<tkd::FMutex> lock2(mutex2);
    // ...
}
```

#### Use Consistent Lock Ordering

```cpp
// ✅ Good: Always lock in same order
void TransferResources() {
    std::lock_guard<tkd::FMutex> lock1(mutex1);
    std::lock_guard<tkd::FMutex> lock2(mutex2);
    // Transfer logic
}
```

## Performance Considerations

### Overhead

- **Lock Contention**: Minimal overhead when uncontended
- **System Calls**: May involve OS kernel calls on some platforms
- **Memory Barriers**: Ensures proper memory ordering

### Optimization Tips

- **Minimize Critical Sections**: Keep locked code as short as possible
- **Use Reader-Writer Locks**: For read-heavy workloads (future enhancement)
- **Consider Lock-Free Alternatives**: For simple operations where possible

## Error Handling

### Common Issues

#### Deadlocks
- **Cause**: Circular waiting for locks
- **Prevention**: Consistent lock ordering, avoid nested locks

#### Race Conditions
- **Cause**: Accessing shared data without proper synchronization
- **Prevention**: Always protect shared data with mutexes

#### Forgotten Unlocks
- **Cause**: Manual lock/unlock without RAII
- **Prevention**: Use `std::lock_guard` or `std::unique_lock`

## Platform Notes

### Linux
- Uses `pthread_mutex_t` internally
- Supports priority inheritance (if enabled)

### macOS
- Uses `os_unfair_lock` or `pthread_mutex_t`
- Optimized for Apple platforms

### Windows
- Uses `SRWLOCK` or `CRITICAL_SECTION`
- Integrated with Windows synchronization primitives

## Future Enhancements

### Planned Features
- **Reader-Writer Mutex**: For read-heavy workloads
- **Recursive Mutex**: For cases requiring recursive locking
- **Timed Locks**: With timeout support
- **Lock Profiling**: Performance monitoring and deadlock detection
