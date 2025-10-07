# FBinaryReader

## Overview

The `FBinaryReader` class is designed for reading binary data from a fixed-size buffer. It provides methods to read various data types, including primitive types, strings, vectors, and raw bytes, while maintaining an internal offset to track the current read position. This ensures safe and efficient parsing of binary data without buffer overflows.

The class is part of the TKD Engine's networking module and is used for deserializing data received over the network or from files.

## Public Members

### Constructor

#### `FBinaryReader(const UInt8* data, SizeT size)`

Initializes the binary reader with a pointer to the data buffer and its size.

- **Parameters:**
  - `data`: Pointer to the binary data buffer.
  - `size`: The size of the buffer in bytes.

### Methods

#### `template <typename T> bool Read(T& value)`

Reads a trivially copyable type from the buffer into the provided value.

- **Template Parameters:**
  - `T`: The type to read, must be trivially copyable (e.g., int, float, structs without pointers).
- **Parameters:**
  - `value`: Reference to the variable where the read value will be stored.
- **Returns:** `true` if the read was successful, `false` if there are not enough bytes remaining.

#### `template <typename T> bool Read(std::vector<T>& vec)`

Reads a vector of elements from the buffer. First reads the size of the vector, then reads each element.

- **Template Parameters:**
  - `T`: The type of elements in the vector.
- **Parameters:**
  - `vec`: Reference to the vector to populate.
- **Returns:** `true` if the read was successful, `false` otherwise.

#### `bool Read(std::string& value)`

Reads a string from the buffer. First reads the size of the string, then reads the characters.

- **Parameters:**
  - `value`: Reference to the string to populate.
- **Returns:** `true` if the read was successful, `false` otherwise.

#### `bool Read(FString& value)`

Reads an `FString` from the buffer. Similar to reading a `std::string`, but uses the `FString` class methods.

- **Parameters:**
  - `value`: Reference to the `FString` to populate.
- **Returns:** `true` if the read was successful, `false` otherwise.

#### `bool ReadBytes(void* buffer, SizeT size)`

Reads raw bytes into a provided buffer.

- **Parameters:**
  - `buffer`: Pointer to the buffer where bytes will be copied.
  - `size`: Number of bytes to read.
- **Returns:** `true` if the read was successful, `false` if there are not enough bytes remaining.

#### `SizeT GetOffset(void) const`

Returns the current read offset in the buffer.

- **Returns:** The current offset in bytes from the start of the buffer.

#### `SizeT GetRemaining(void) const`

Returns the number of bytes remaining to be read.

- **Returns:** The number of unread bytes in the buffer.

## Usage Examples

### Basic Usage

```cpp
#include <Engine/Network/FBinaryReader.hpp>

// Assume we have binary data
const UInt8 data[] = { /* binary data */ };
SizeT dataSize = sizeof(data);

// Create a reader
tkd::FBinaryReader reader(data, dataSize);

// Read an integer
int value;
if (reader.Read(value)) {
    // Use value
}

// Read a string
std::string str;
if (reader.Read(str)) {
    // Use str
}
```

### Reading a Vector

```cpp
#include <Engine/Network/FBinaryReader.hpp>
#include <vector>

// Binary data containing a vector of ints
const UInt8 data[] = { /* size followed by ints */ };
tkd::FBinaryReader reader(data, sizeof(data));

std::vector<int> vec;
if (reader.Read(vec)) {
    // vec now contains the read elements
}
```

### Checking Progress

```cpp
#include <Engine/Network/FBinaryReader.hpp>

// After some reads
SizeT offset = reader.GetOffset();
SizeT remaining = reader.GetRemaining();
```

### Error Handling

All read methods return `false` if the operation would exceed the buffer size, allowing for safe parsing.

```cpp
if (!reader.Read(someValue)) {
    // Handle error: not enough data
}
```
