# TVector Class Documentation

## Overview

The `TVector<T>` class is a dynamic array container that provides a wrapper around the standard C++ `std::vector<T>`. It offers a comprehensive set of methods for managing collections of elements of type `T`, including insertion, deletion, access, and iteration operations.

## Namespace

```cpp
namespace tkd
```

## Template Parameters

- `T`: The type of elements stored in the vector.

## Public Type Aliases

- `Reference`: Equivalent to `std::vector<T>::reference`
- `ConstReference`: Equivalent to `std::vector<T>::const_reference`
- `Iterator`: Equivalent to `typename std::vector<T>::iterator`
- `RIterator`: Equivalent to `typename std::vector<T>::reverse_iterator`
- `ConstIterator`: Equivalent to `typename std::vector<T>::const_iterator`
- `RConstIterator`: Equivalent to `typename std::vector<T>::const_reverse_iterator`

## Constructors

### Default Constructor

```cpp
TVector(void);
```

Constructs an empty vector.

**Example:**
```cpp
tkd::TVector<int> vec;
```

## Operators

### Subscript Operator (Non-const)

```cpp
T& operator[](SizeT index);
```

Accesses the element at the specified index without bounds checking.

**Parameters:**
- `index`: The index of the element to access.

**Returns:** A reference to the element at the specified index.

**Example:**
```cpp
tkd::TVector<int> vec;
vec.PushBack(10);
vec.PushBack(20);
int value = vec[0]; // value = 10
```

### Subscript Operator (Const)

```cpp
const T& operator[](SizeT index) const;
```

Accesses the element at the specified index without bounds checking (const version).

**Parameters:**
- `index`: The index of the element to access.

**Returns:** A constant reference to the element at the specified index.

**Example:**
```cpp
const tkd::TVector<int> vec = {10, 20};
int value = vec[1]; // value = 20
```

## Capacity Methods

### Size

```cpp
SizeT Size(void) const;
```

Returns the number of elements in the vector.

**Returns:** The size of the vector.

**Example:**
```cpp
tkd::TVector<int> vec;
vec.PushBack(1);
vec.PushBack(2);
SizeT size = vec.Size(); // size = 2
```

### Empty

```cpp
bool Empty(void) const;
```

Checks if the vector is empty.

**Returns:** `true` if the vector is empty, `false` otherwise.

**Example:**
```cpp
tkd::TVector<int> vec;
bool isEmpty = vec.Empty(); // isEmpty = true
vec.PushBack(1);
isEmpty = vec.Empty(); // isEmpty = false
```

### Capacity

```cpp
SizeT Capacity(void) const;
```

Returns the current capacity of the vector.

**Returns:** The capacity of the vector.

**Example:**
```cpp
tkd::TVector<int> vec;
vec.Reserve(10);
SizeT cap = vec.Capacity(); // cap >= 10
```

### Reserve

```cpp
void Reserve(SizeT size);
```

Reserves space for at least `size` elements.

**Parameters:**
- `size`: The number of elements to reserve space for.

**Example:**
```cpp
tkd::TVector<int> vec;
vec.Reserve(100); // Reserves space for 100 elements
```

### ShrinkToFit

```cpp
void ShrinkToFit(void);
```

Reduces the capacity of the vector to fit its size.

**Example:**
```cpp
tkd::TVector<int> vec;
vec.Reserve(100);
vec.PushBack(1);
vec.ShrinkToFit(); // Capacity is reduced to 1 or more
```

## Element Access Methods

### At (Non-const)

```cpp
T& At(SizeT index);
```

Accesses the element at the specified index with bounds checking.

**Parameters:**
- `index`: The index of the element to access.

**Returns:** A reference to the element at the specified index.

**Throws:** `std::out_of_range` if index is out of bounds.

**Example:**
```cpp
tkd::TVector<int> vec = {10, 20, 30};
int value = vec.At(1); // value = 20
// vec.At(5); // Throws std::out_of_range
```

### At (Const)

```cpp
const T& At(SizeT index) const;
```

Accesses the element at the specified index with bounds checking (const version).

**Parameters:**
- `index`: The index of the element to access.

**Returns:** A constant reference to the element at the specified index.

**Throws:** `std::out_of_range` if index is out of bounds.

**Example:**
```cpp
const tkd::TVector<int> vec = {10, 20, 30};
int value = vec.At(2); // value = 30
```

### Front (Non-const)

```cpp
Reference Front(void);
```

Returns a reference to the first element in the vector.

**Returns:** A reference to the first element.

**Precondition:** The vector must not be empty.

**Example:**
```cpp
tkd::TVector<int> vec = {10, 20, 30};
int& first = vec.Front(); // first = 10
first = 5; // Modifies the first element
```

### Front (Const)

```cpp
ConstReference Front(void) const;
```

Returns a constant reference to the first element in the vector.

**Returns:** A constant reference to the first element.

**Precondition:** The vector must not be empty.

**Example:**
```cpp
const tkd::TVector<int> vec = {10, 20, 30};
int first = vec.Front(); // first = 10
```

### Back (Non-const)

```cpp
Reference Back(void);
```

Returns a reference to the last element in the vector.

**Returns:** A reference to the last element.

**Precondition:** The vector must not be empty.

**Example:**
```cpp
tkd::TVector<int> vec = {10, 20, 30};
int& last = vec.Back(); // last = 30
last = 40; // Modifies the last element
```

### Back (Const)

```cpp
ConstReference Back(void) const;
```

Returns a constant reference to the last element in the vector.

**Returns:** A constant reference to the last element.

**Precondition:** The vector must not be empty.

**Example:**
```cpp
const tkd::TVector<int> vec = {10, 20, 30};
int last = vec.Back(); // last = 30
```

### Data (Non-const)

```cpp
T* Data(void);
```

Returns a pointer to the underlying data array.

**Returns:** A pointer to the underlying data.

**Example:**
```cpp
tkd::TVector<int> vec = {1, 2, 3};
int* data = vec.Data();
// data[0] = 1, data[1] = 2, data[2] = 3
```

### Data (Const)

```cpp
const T* Data(void) const;
```

Returns a constant pointer to the underlying data array.

**Returns:** A constant pointer to the underlying data.

**Example:**
```cpp
const tkd::TVector<int> vec = {1, 2, 3};
const int* data = vec.Data();
// data[0] = 1, data[1] = 2, data[2] = 3
```

## Modifiers

### Clear

```cpp
void Clear(void);
```

Removes all elements from the vector.

**Example:**
```cpp
tkd::TVector<int> vec = {1, 2, 3};
vec.Clear(); // vec is now empty
```

### Resize

```cpp
void Resize(SizeT size);
```

Changes the size of the vector to `size`. If `size` is smaller than the current size, elements are removed. If larger, default-constructed elements are added.

**Parameters:**
- `size`: The new size of the vector.

**Example:**
```cpp
tkd::TVector<int> vec = {1, 2, 3};
vec.Resize(5); // vec = {1, 2, 3, 0, 0}
vec.Resize(2); // vec = {1, 2}
```

### PushBack (Lvalue)

```cpp
void PushBack(const T& value);
```

Adds an element to the end of the vector.

**Parameters:**
- `value`: The element to add.

**Example:**
```cpp
tkd::TVector<int> vec;
vec.PushBack(10);
vec.PushBack(20); // vec = {10, 20}
```

### PushBack (Rvalue)

```cpp
void PushBack(T&& value);
```

Adds an element to the end of the vector using move semantics.

**Parameters:**
- `value`: The element to add (moved).

**Example:**
```cpp
tkd::TVector<std::string> vec;
std::string str = "hello";
vec.PushBack(std::move(str)); // str is moved, vec = {"hello"}
```

### PushFront (Lvalue)

```cpp
void PushFront(const T& value);
```

Adds an element to the front of the vector.

**Parameters:**
- `value`: The element to add.

**Example:**
```cpp
tkd::TVector<int> vec = {2, 3};
vec.PushFront(1); // vec = {1, 2, 3}
```

### PushFront (Rvalue)

```cpp
void PushFront(T&& value);
```

Adds an element to the front of the vector using move semantics.

**Parameters:**
- `value`: The element to add (moved).

**Example:**
```cpp
tkd::TVector<std::string> vec = {"world"};
std::string str = "hello";
vec.PushFront(std::move(str)); // vec = {"hello", "world"}
```

### PopBack (Non-const)

```cpp
Reference PopBack(void);
```

Removes and returns the last element of the vector.

**Returns:** A reference to the removed element.

**Precondition:** The vector must not be empty.

**Example:**
```cpp
tkd::TVector<int> vec = {1, 2, 3};
int last = vec.PopBack(); // last = 3, vec = {1, 2}
```

### PopBack (Const)

```cpp
ConstReference PopBack(void) const;
```

Removes and returns the last element of the vector (const version).

**Returns:** A constant reference to the removed element.

**Precondition:** The vector must not be empty.

**Note:** This const version modifies the vector, which is unusual. Use with caution.

### PopFront (Non-const)

```cpp
Reference PopFront(void);
```

Removes and returns the first element of the vector.

**Returns:** A reference to the removed element.

**Precondition:** The vector must not be empty.

**Example:**
```cpp
tkd::TVector<int> vec = {1, 2, 3};
int first = vec.PopFront(); // first = 1, vec = {2, 3}
```

### PopFront (Const)

```cpp
ConstReference PopFront(void) const;
```

Removes and returns the first element of the vector (const version).

**Returns:** A constant reference to the removed element.

**Precondition:** The vector must not be empty.

**Note:** This const version modifies the vector, which is unusual. Use with caution.

### Assign (Count and Value)

```cpp
void Assign(SizeT count, const T& value);
```

Assigns `count` copies of `value` to the vector, replacing its current contents.

**Parameters:**
- `count`: The number of elements to assign.
- `value`: The value to assign.

**Example:**
```cpp
tkd::TVector<int> vec;
vec.Assign(3, 5); // vec = {5, 5, 5}
```

### Assign (Range)

```cpp
template <typename InputIt>
void Assign(InputIt first, InputIt last);
```

Assigns elements from the range `[first, last)` to the vector, replacing its current contents.

**Parameters:**
- `first`: The beginning of the range to assign.
- `last`: The end of the range to assign.

**Example:**
```cpp
tkd::TVector<int> vec;
std::vector<int> source = {1, 2, 3};
vec.Assign(source.begin(), source.end()); // vec = {1, 2, 3}
```

### Insert (Single Element, Lvalue)

```cpp
Iterator Insert(ConstIterator pos, const T& value);
```

Inserts an element at the specified position.

**Parameters:**
- `pos`: The position to insert the element at.
- `value`: The value to insert.

**Returns:** An iterator to the inserted element.

**Example:**
```cpp
tkd::TVector<int> vec = {1, 3};
auto it = vec.Insert(vec.Begin() + 1, 2); // vec = {1, 2, 3}, it points to 2
```

### Insert (Single Element, Rvalue)

```cpp
Iterator Insert(ConstIterator pos, T&& value);
```

Inserts an element at the specified position using move semantics.

**Parameters:**
- `pos`: The position to insert the element at.
- `value`: The value to insert (moved).

**Returns:** An iterator to the inserted element.

### Insert (Multiple Elements)

```cpp
Iterator Insert(ConstIterator pos, SizeT count, const T& value);
```

Inserts `count` copies of `value` at the specified position.

**Parameters:**
- `pos`: The position to insert the elements at.
- `count`: The number of elements to insert.
- `value`: The value to insert.

**Returns:** An iterator to the first inserted element.

**Example:**
```cpp
tkd::TVector<int> vec = {1, 4};
auto it = vec.Insert(vec.Begin() + 1, 2, 2); // vec = {1, 2, 2, 4}, it points to first 2
```

### Insert (Range)

```cpp
template <typename InputIt>
Iterator Insert(ConstIterator pos, InputIt first, InputIt last);
```

Inserts elements from the range `[first, last)` at the specified position.

**Parameters:**
- `pos`: The position to insert the elements at.
- `first`: The beginning of the range to insert.
- `last`: The end of the range to insert.

**Returns:** An iterator to the first inserted element.

### Emplace

```cpp
template <typename... Args>
Iterator Emplace(ConstIterator pos, Args&&... args);
```

Constructs an element in-place at the specified position.

**Parameters:**
- `pos`: The position to emplace the element at.
- `args`: The arguments to forward to the constructor.

**Returns:** An iterator to the emplaced element.

**Example:**
```cpp
tkd::TVector<std::pair<int, int>> vec;
auto it = vec.Emplace(vec.End(), 1, 2); // vec = {{1, 2}}
```

### EmplaceBack

```cpp
template <typename... Args>
Reference EmplaceBack(Args&&... args);
```

Constructs an element in-place at the end of the vector.

**Parameters:**
- `args`: The arguments to forward to the constructor.

**Returns:** A reference to the emplaced element.

**Example:**
```cpp
tkd::TVector<std::pair<int, int>> vec;
auto& elem = vec.EmplaceBack(3, 4); // vec = {{3, 4}}, elem is {3, 4}
```

### EmplaceFront

```cpp
template <typename... Args>
Reference EmplaceFront(Args&&... args);
```

Constructs an element in-place at the front of the vector.

**Parameters:**
- `args`: The arguments to forward to the constructor.

**Returns:** A reference to the emplaced element.

**Example:**
```cpp
tkd::TVector<std::pair<int, int>> vec = {{5, 6}};
auto& elem = vec.EmplaceFront(7, 8); // vec = {{7, 8}, {5, 6}}, elem is {7, 8}
```

### Erase (Range)

```cpp
void Erase(Iterator begin, Iterator end);
```

Erases elements in the range `[begin, end)`.

**Parameters:**
- `begin`: The beginning of the range to erase.
- `end`: The end of the range to erase.

**Example:**
```cpp
tkd::TVector<int> vec = {1, 2, 3, 4, 5};
vec.Erase(vec.Begin() + 1, vec.Begin() + 4); // vec = {1, 5}
```

### Erase (Const Range)

```cpp
void Erase(ConstIterator begin, ConstIterator end);
```

Erases elements in the range `[begin, end)` (const iterator version).

**Parameters:**
- `begin`: The beginning of the range to erase.
- `end`: The end of the range to erase.

### Swap

```cpp
void Swap(TVector<T>& other);
```

Swaps the contents of this vector with another vector.

**Parameters:**
- `other`: The other vector to swap with.

**Example:**
```cpp
tkd::TVector<int> vec1 = {1, 2};
tkd::TVector<int> vec2 = {3, 4};
vec1.Swap(vec2); // vec1 = {3, 4}, vec2 = {1, 2}
```

## Iterators

### Begin (Non-const)

```cpp
Iterator Begin(void);
Iterator begin(void);
```

Returns an iterator to the beginning of the vector.

**Returns:** An iterator to the first element.

**Example:**
```cpp
tkd::TVector<int> vec = {1, 2, 3};
auto it = vec.Begin(); // it points to 1
```

### Begin (Const)

```cpp
ConstIterator Begin(void) const;
ConstIterator begin(void) const;
```

Returns a constant iterator to the beginning of the vector.

**Returns:** A constant iterator to the first element.

### End (Non-const)

```cpp
Iterator End(void);
Iterator end(void);
```

Returns an iterator to the end of the vector (past the last element).

**Returns:** An iterator to the end of the vector.

**Example:**
```cpp
tkd::TVector<int> vec = {1, 2, 3};
auto it = vec.End(); // it points past 3
```

### End (Const)

```cpp
ConstIterator End(void) const;
ConstIterator end(void) const;
```

Returns a constant iterator to the end of the vector (past the last element).

**Returns:** A constant iterator to the end of the vector.

### RBegin (Non-const)

```cpp
RIterator RBegin(void);
RIterator rbegin(void);
```

Returns a reverse iterator to the beginning of the reversed vector (i.e., the last element).

**Returns:** A reverse iterator to the beginning of the reversed vector.

**Example:**
```cpp
tkd::TVector<int> vec = {1, 2, 3};
auto rit = vec.RBegin(); // rit points to 3
```

### RBegin (Const)

```cpp
RConstIterator RBegin(void) const;
RConstIterator rbegin(void) const;
```

Returns a constant reverse iterator to the beginning of the reversed vector.

**Returns:** A constant reverse iterator to the beginning of the reversed vector.

### REnd (Non-const)

```cpp
RIterator REnd(void);
RIterator rend(void);
```

Returns a reverse iterator to the end of the reversed vector (i.e., before the first element).

**Returns:** A reverse iterator to the end of the reversed vector.

### REnd (Const)

```cpp
RConstIterator REnd(void) const;
RConstIterator rend(void) const;
```

Returns a constant reverse iterator to the end of the reversed vector.

**Returns:** A constant reverse iterator to the end of the reversed vector.

## Utility Methods

### ToStdVector (Non-const)

```cpp
std::vector<T>& ToStdVector(void);
```

Returns a reference to the underlying `std::vector`.

**Returns:** A reference to the underlying `std::vector`.

**Example:**
```cpp
tkd::TVector<int> vec = {1, 2, 3};
std::vector<int>& stdVec = vec.ToStdVector();
// Can now use std::vector methods directly
```

### ToStdVector (Const)

```cpp
const std::vector<T>& ToStdVector(void) const;
```

Returns a constant reference to the underlying `std::vector`.

**Returns:** A constant reference to the underlying `std::vector`.

### ForEach

```cpp
void ForEach(const std::function<void(T&)>& func);
```

Applies a function to each element in the vector.

**Parameters:**
- `func`: A function that takes a reference to an element and modifies it.

**Example:**
```cpp
tkd::TVector<int> vec = {1, 2, 3};
vec.ForEach([](int& x) { x *= 2; }); // vec = {2, 4, 6}
```

## Notes

- The `SizeT` type is assumed to be defined elsewhere (likely as `size_t` or similar).
- Some methods have both const and non-const versions where appropriate.
- The `PopBack` and `PopFront` const versions are unusual as they modify the container despite being const. This may be an oversight in the design.
- All operations that modify the vector invalidate iterators, pointers, and references as per standard `std::vector` behavior.
