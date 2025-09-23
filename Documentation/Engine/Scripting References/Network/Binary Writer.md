# FBinaryWriter

## Overview

The `FBinaryWriter` class is designed for writing binary data to a resizable buffer. It provides methods to write various data types, including primitive types, strings, vectors, and raw bytes, while automatically resizing the buffer as needed. This ensures efficient serialization of data for network transmission or file storage.

The class is part of the TKD Engine's networking module and is used for serializing data to be sent over the network or saved to files.

## Public Members

### Constructor

#### `explicit FBinaryWriter(std::vector<UInt8>& buffer)`

Initializes the binary writer with a reference to a buffer where data will be written.

- **Parameters:**
  - `buffer`: Reference to the `std::vector<UInt8>` buffer to write to.

### Methods

#### `template <typename T> void Write(const T& value)`

Writes a trivially copyable type to the buffer.

- **Template Parameters:**
  - `T`: The type to write, must be trivially copyable (e.g., int, float, structs without pointers).
- **Parameters:**
  - `value`: The value to write to the buffer.

#### `template <typename T> void Write(const std::vector<T>& array)`

Writes a vector of trivially copyable elements to the buffer. First writes the size of the vector, then the elements.

- **Template Parameters:**
  - `T`: The type of elements in the vector.
- **Parameters:**
  - `array`: The vector of values to write.

#### `void Write(const std::vector<std::string>& array)`

Writes a vector of strings to the buffer. First writes the size of the vector, then each string.

- **Parameters:**
  - `array`: The vector of strings to write.

#### `void Write(const std::vector<FString>& array)`

Writes a vector of `FString` objects to the buffer. Similar to writing a vector of `std::string`.

- **Parameters:**
  - `array`: The vector of `FString` objects to write.

#### `void Write(const std::string& str)`

Writes a string to the buffer. First writes the size of the string, then the characters.

- **Parameters:**
  - `str`: The string to write.

#### `void Write(const FString& str)`

Writes an `FString` to the buffer. Similar to writing a `std::string`, but uses the `FString` class methods.

- **Parameters:**
  - `str`: The `FString` to write.

#### `void WriteBytes(const UInt8* data, SizeT size)`

Writes raw bytes to the buffer.

- **Parameters:**
  - `data`: Pointer to the byte array to write.
  - `size`: Number of bytes to write.

#### `SizeT GetOffset(void) const`

Returns the current write offset in the buffer.

- **Returns:** The current offset in bytes from the start of the buffer.

#### `void SetOffset(SizeT offset)`

Sets the current write offset in the buffer. This can be used to overwrite data or reposition for writing.

- **Parameters:**
  - `offset`: The new write offset in bytes.

## Usage Examples

### Basic Usage

```cpp
#include <Engine/Network/FBinaryWriter.hpp>
#include <vector>

// Create a buffer
std::vector<UInt8> buffer;

// Create a writer
tkd::FBinaryWriter writer(buffer);

// Write an integer
int value = 42;
writer.Write(value);

// Write a string
std::string str = "Hello";
writer.Write(str);

// Now buffer contains the serialized data
```

### Writing a Vector

```cpp
#include <Engine/Network/FBinaryWriter.hpp>
#include <vector>

// Buffer and writer
std::vector<UInt8> buffer;
tkd::FBinaryWriter writer(buffer);

// Vector of ints
std::vector<int> vec = {1, 2, 3, 4};
writer.Write(vec);

// Vector of strings
std::vector<std::string> strVec = {"a", "b", "c"};
writer.Write(strVec);
```

### Writing Raw Bytes

```cpp
#include <Engine/Network/FBinaryWriter.hpp>

// Buffer and writer
std::vector<UInt8> buffer;
tkd::FBinaryWriter writer(buffer);

// Raw data
const UInt8 data[] = {0x01, 0x02, 0x03};
writer.WriteBytes(data, sizeof(data));
```

### Managing Offset

```cpp
#include <Engine/Network/FBinaryWriter.hpp>

// After some writes
SizeT currentOffset = writer.GetOffset();

// Reposition to overwrite
writer.SetOffset(0);
writer.Write(newValue);  // Overwrites from the beginning
```
