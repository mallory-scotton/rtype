# Process

## Overview

The TKD Engine provides process management through the `IProcess` interface and its implementation `FProcess`. This abstraction allows creating and managing child processes using Unix fork semantics, enabling parallel execution and workload distribution.

## EProcessStatus Enumeration

Defines the possible states of a process:

- **Running**: Process is currently executing
- **Stopped**: Process is stopped (not running)
- **Finished**: Process completed successfully
- **Error**: Process encountered an error or was terminated abnormally

## IProcess Interface

The `IProcess` interface defines the contract for process implementations.

### Methods

#### `virtual void Start(void) = 0`

Starts the process execution.

#### `virtual void Wait(void) = 0`

Blocks until the process finishes execution.

#### `virtual bool IsRunning(void) = 0`

Checks if the process is currently running.

- **Returns:** `true` if running, `false` otherwise.

#### `virtual int GetReturnValue(void) = 0`

Gets the exit code returned by the process.

- **Returns:** Exit code (0 for success, non-zero for errors).

#### `virtual bool IsParent(void) const = 0`

Checks if the current execution context is the parent process.

- **Returns:** `true` if parent, `false` if child.

#### `virtual bool IsChild(void) const = 0`

Checks if the current execution context is the child process.

- **Returns:** `true` if child, `false` if parent.

## FProcess Implementation

`FProcess` implements process management using Unix `fork()` and `exec()` system calls.

### Features

- **Fork-Based**: Creates child processes using `fork()`
- **Function Execution**: Runs std::function in child process
- **Status Tracking**: Monitors process state and exit codes
- **Signal Handling**: Graceful termination with SIGTERM/SIGKILL
- **Exception Safety**: Handles exceptions in child processes

### Constructor

#### `explicit FProcess(std::function<void()> function)`

Creates a process that will execute the provided function.

- **Parameters:**
  - `function`: Function to execute in the child process

### Usage Examples

#### Basic Process Creation

```cpp
#include <Engine/Core/Concurrency/FProcess.hpp>

void WorkerFunction() {
    std::cout << "Child process executing" << std::endl;
    // Do work here
}

tkd::FProcess process(WorkerFunction);
process.Start();

// In parent process
process.Wait();
int exitCode = process.GetReturnValue();
```

#### Parallel Processing

```cpp
#include <Engine/Core/Concurrency/FProcess.hpp>
#include <vector>

std::vector<tkd::FProcess> processes;

for (int i = 0; i < 4; ++i) {
    processes.emplace_back([i]() {
        std::cout << "Worker " << i << " processing" << std::endl;
        // Simulate work
        std::this_thread::sleep_for(std::chrono::seconds(1));
    });
}

// Start all processes
for (auto& proc : processes) {
    proc.Start();
}

// Wait for all to complete
for (auto& proc : processes) {
    proc.Wait();
}
```

#### Process Monitoring

```cpp
#include <Engine/Core/Concurrency/FProcess.hpp>

tkd::FProcess process([]() {
    // Long running task
    std::this_thread::sleep_for(std::chrono::seconds(10));
});

process.Start();

// Check status periodically
while (process.IsRunning()) {
    std::cout << "Process still running..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

std::cout << "Process finished with code: " << process.GetReturnValue() << std::endl;
```

#### Parent/Child Context Detection

```cpp
#include <Engine/Core/Concurrency/FProcess.hpp>

tkd::FProcess process([]() {
    if (process.IsChild()) {
        std::cout << "Running in child process" << std::endl;
        // Child-specific logic
    }
});

process.Start();

if (process.IsParent()) {
    std::cout << "Running in parent process" << std::endl;
    process.Wait();
}
```

## Process Lifecycle

### States

1. **Created**: Process object instantiated
2. **Started**: `Start()` called, child process forked
3. **Running**: Child executing the function
4. **Finished/Error**: Child exits, status updated
5. **Waited**: Parent calls `Wait()`, reaps child

### Fork Behavior

- **Parent Process**: `m_pid > 0`, continues execution
- **Child Process**: `m_pid == 0`, executes function and exits
- **Error**: `m_pid < 0`, fork failed

## Error Handling

### Exception Handling

```cpp
tkd::FProcess process([]() {
    try {
        // Risky operation
        throw std::runtime_error("Something went wrong");
    } catch (const std::exception& e) {
        std::cerr << "Child exception: " << e.what() << std::endl;
        throw; // Re-throw to exit with error code
    }
});

process.Start();
process.Wait();

if (process.GetReturnValue() != 0) {
    std::cout << "Process exited with error" << std::endl;
}
```

### Common Errors

- **Fork Failure**: System out of resources, check errno
- **Wait Failure**: Process already waited on or invalid PID
- **Signal Issues**: Problems sending termination signals

## Resource Management

### Automatic Cleanup

- **Destructor**: Automatically terminates running processes
- **Graceful Shutdown**: Sends SIGTERM, then SIGKILL if needed
- **Zombie Prevention**: Properly reaps child processes

### Manual Management

```cpp
{
    tkd::FProcess process(workerFunction);
    process.Start();

    // Process automatically cleaned up when going out of scope
    // Even if not explicitly waited on
}
```

## Performance Considerations

### Overhead

- **Fork Cost**: Memory duplication, file descriptor copying
- **Context Switching**: Kernel scheduling between processes
- **IPC Overhead**: Communication between parent/child

### Optimization Tips

- **Pool Processes**: Reuse process objects for similar tasks
- **Minimize Forking**: Use threads for lightweight parallelism
- **Share Memory**: Use shared memory segments for large data

## Platform Notes

### Unix/Linux Systems

- **Implementation**: Uses `fork()`, `waitpid()`, `kill()`
- **Signals**: Supports SIGTERM, SIGKILL for termination
- **PID Management**: Standard Unix process IDs

### macOS

- **Compatible**: Same Unix implementation as Linux
- **Optimizations**: Apple-specific process optimizations

### Windows

- **Not Supported**: Current implementation is Unix-specific
- **Future**: May add Windows process abstraction

## Thread Safety

### Guarantees

- **Process Creation**: Thread-safe, can create processes from any thread
- **Status Queries**: Safe to call from multiple threads
- **Wait Operations**: Blocking, should be called from appropriate context

### Best Practices

- **Avoid Concurrent Access**: Don't manipulate process objects from multiple threads simultaneously
- **Signal Handling**: Be careful with signals in multi-threaded applications

## Future Enhancements

### Planned Features

- **Windows Support**: Add Windows process abstraction
- **Process Groups**: Manage groups of related processes
- **Resource Limits**: Set CPU/memory limits on child processes
- **IPC Channels**: Built-in inter-process communication
- **Process Monitoring**: Advanced monitoring and statistics
