# TList

The `TList` class is a template-based doubly-linked list implementation designed for game engine use. It provides comprehensive list operations with both STL-compatible and Unreal Engine-style APIs, making it suitable for performance-critical applications where frequent insertions and deletions at arbitrary positions are required.

## Features

* Template-based design supporting any type
* Doubly-linked structure with O(1) insertion/deletion at head and tail
* Full STL-compatible iterator interface (bidirectional iterators)
* Unreal Engine-style API for seamless integration with UE-like codebases
* Exception-safe operations with proper RAII
* Move semantics support for optimal performance
* Iterator bounds checking with assertions in debug builds
* Complete copy and move constructor/assignment support
* Memory-efficient node-based storage

## Usage

### Basic Construction and Management

```cpp
// Default constructor (empty list)
tkd::TList<int> list1;

// Copy constructor
tkd::TList<int> list2(list1);

// Move constructor
tkd::TList<int> list3(std::move(list1));

// Assignment operators
tkd::TList<int> list4;
list4 = list2;                    // Copy assignment
list4 = std::move(list3);         // Move assignment

// Check if empty and get size
bool isEmpty = list4.empty();     // or list4.IsEmpty()
size_t count = list4.size();      // or list4.Num()
```

### Element Access

```cpp
tkd::TList<int> list;
list.push_back(10);
list.push_back(20);
list.push_front(5);

// Access first and last elements
int& first = list.front();       // 5
int& last = list.back();         // 20

// Const access
const auto& constList = list;
const int& firstConst = constList.front();
const int& lastConst = constList.back();
```

### Adding Elements

```cpp
tkd::TList<std::string> list;

// STL-style insertion
auto it1 = list.push_front("first");     // Returns iterator to inserted element
auto it2 = list.push_back("last");       // Returns iterator to inserted element

// UE-style insertion
auto it3 = list.AddHead("head");         // Equivalent to push_front
auto it4 = list.AddTail("tail");         // Equivalent to push_back

// In-place construction (emplace)
auto it5 = list.EmplaceHead(5, 'c');  // Construct string with 5 'c's
auto it6 = list.EmplaceTail("in-place");

// Insert at specific position
auto it7 = list.insert(it2, "middle");          // Insert before it2
auto it8 = list.emplace(it7, "emplaced");       // Emplace before it7
auto it9 = list.InsertBefore(it8, "before");    // UE-style insert
```

### Removing Elements

```cpp
tkd::TList<int> list;
list.push_back(10);
list.push_back(20);
list.push_back(30);

// Remove from ends
list.pop_front();                // Removes first element
list.pop_back();                 // Removes last element

// Remove and return value (UE-style)
int headValue = list.RemoveHead();    // Remove and return first element
int tailValue = list.RemoveTail();    // Remove and return last element

// Remove at iterator position
auto it = list.begin();
auto nextIt = list.erase(it);        // Returns iterator to next element
// or
auto nextIt2 = list.RemoveNode(it);  // UE-style removal

// Clear all elements
list.clear();                        // STL-style
// or
list.Empty();                        // UE-style
```

### Iteration

```cpp
tkd::TList<int> list = {1, 2, 3, 4, 5};

// Range-based for loop
for (const auto& value : list) {
    std::cout << value << " ";
}

// Iterator-based iteration
for (auto it = list.begin(); it != list.end(); ++it) {
    std::cout << *it << " ";
}

// Const iteration
for (auto it = list.cbegin(); it != list.cend(); ++it) {
    std::cout << *it << " ";
}

// Reverse iteration (using bidirectional iterators)
for (auto it = std::prev(list.end()); it != std::prev(list.begin()); --it) {
    std::cout << *it << " ";
}
```

### Search Operations

```cpp
tkd::TList<std::string> list;
list.AddTail("apple");
list.AddTail("banana");
list.AddTail("cherry");

// Find element
auto it = list.Find("banana");
if (it != list.end()) {
    std::cout << "Found: " << *it << std::endl;
}

// Check if contains element
bool hasApple = list.Contains("apple");     // true
bool hasGrape = list.Contains("grape");     // false

// Const search
const auto& constList = list;
auto constIt = constList.Find("cherry");
```

### Node Access (Advanced Usage)

```cpp
tkd::TList<int> list;
list.push_back(42);

// Direct node access (use with caution)
auto* headNode = list.GetHead();
auto* tailNode = list.GetTail();

if (headNode) {
    int value = headNode->Value;        // Access value
    auto* nextNode = headNode->Next;    // Access next node
    auto* prevNode = headNode->Prev;    // Access previous node (nullptr for head)
}
```

### Container Operations

```cpp
tkd::TList<int> list1;
tkd::TList<int> list2;

list1.push_back(1);
list1.push_back(2);
list2.push_back(3);
list2.push_back(4);

// Swap contents (O(1) operation)
list1.swap(list2);
// list1 now contains {3, 4}
// list2 now contains {1, 2}
```

### Performance Examples

```cpp
tkd::TList<int> list;

// O(1) operations - very fast
list.push_front(1);              // O(1)
list.push_back(2);               // O(1)
list.pop_front();                // O(1)
list.pop_back();                 // O(1)

auto it = list.begin();
list.insert(it, 3);              // O(1) if you have iterator
list.erase(it);                  // O(1) if you have iterator

// O(n) operations - linear time
auto found = list.Find(42);      // O(n) - must search through list
bool exists = list.Contains(42); // O(n) - must search through list
```

### Exception Safety

```cpp
class ThrowingClass {
public:
    ThrowingClass(bool shouldThrow) {
        if (shouldThrow) throw std::runtime_error("Constructor failed");
    }
};

tkd::TList<ThrowingClass> list;

try {
    // If construction throws, no memory is leaked
    list.push_back(ThrowingClass(true));
} catch (const std::exception& e) {
    // List remains in valid state
    assert(list.empty());  // Still empty
}

// Successful construction
list.push_back(ThrowingClass(false));  // Works fine
```

### Integration with STL Algorithms

```cpp
tkd::TList<int> list;
list.push_back(3);
list.push_back(1);
list.push_back(4);
list.push_back(1);
list.push_back(5);

// Use STL algorithms
auto it = std::find(list.begin(), list.end(), 4);
if (it != list.end()) {
    std::cout << "Found 4 at position" << std::endl;
}

// Count elements
int count = std::count(list.begin(), list.end(), 1);  // Returns 2

// Transform elements
std::transform(list.begin(), list.end(), list.begin(), 
    [](int x) { return x * 2; });
```

## API Reference

### STL-Compatible Interface

| Method | Description | Complexity |
|--------|-------------|------------|
| `push_front(args...)` | Add element to front | O(1) |
| `push_back(args...)` | Add element to back | O(1) |
| `pop_front()` | Remove first element | O(1) |
| `pop_back()` | Remove last element | O(1) |
| `insert(pos, value)` | Insert before position | O(1) |
| `emplace(pos, args...)` | Construct in-place before position | O(1) |
| `erase(pos)` | Remove at position | O(1) |
| `clear()` | Remove all elements | O(n) |
| `empty()` | Check if empty | O(1) |
| `size()` | Get number of elements | O(1) |
| `front()` | Access first element | O(1) |
| `back()` | Access last element | O(1) |
| `begin()` | Get iterator to first element | O(1) |
| `end()` | Get iterator past last element | O(1) |
| `cbegin()` | Get const iterator to first element | O(1) |
| `cend()` | Get const iterator past last element | O(1) |

### Unreal Engine-Style Interface

| Method | Description | Complexity |
|--------|-------------|------------|
| `AddHead(value)` | Add element to front | O(1) |
| `AddTail(value)` | Add element to back | O(1) |
| `EmplaceHead(args...)` | Construct element at front | O(1) |
| `EmplaceTail(args...)` | Construct element at back | O(1) |
| `RemoveHead()` | Remove and return first element | O(1) |
| `RemoveTail()` | Remove and return last element | O(1) |
| `RemoveNode(pos)` | Remove at iterator position | O(1) |
| `InsertBefore(pos, value)` | Insert before position | O(1) |
| `Find(value)` | Find element by value | O(n) |
| `Contains(value)` | Check if contains value | O(n) |
| `Num()` | Get number of elements | O(1) |
| `IsEmpty()` | Check if empty | O(1) |
| `Empty()` | Remove all elements | O(n) |
| `GetHead()` | Get pointer to first node | O(1) |
| `GetTail()` | Get pointer to last node | O(1) |

## Type Aliases

Common type aliases can be defined for convenience:

```cpp
using IntList = tkd::TList<int>;
using StringList = tkd::TList<std::string>;
using FloatList = tkd::TList<float>;

// Usage
IntList numbers;
numbers.AddTail(42);

StringList names;
names.AddHead("Player1");
```

## Performance Characteristics

* **Memory**: Each element requires one node allocation (element + 2 pointers)
* **Insertion/Deletion**: O(1) at any position if you have an iterator
* **Search**: O(n) linear search required
* **Access**: O(1) for front/back, O(n) for arbitrary positions
* **Space Overhead**: 2 pointers per element (typically 16 bytes on 64-bit systems)

## When to Use TList

**Use TList when:**
* You need frequent insertions/deletions at arbitrary positions
* You have iterators to insertion points (making operations O(1))
* Memory layout is less important than insertion/deletion performance
* You need stable iterators (iterators remain valid unless the specific element is removed)

**Consider alternatives when:**
* You need random access by index (use `TArray`/`std::vector` instead)
* Memory cache locality is critical (use `TArray`/`std::vector` instead)
* You primarily access elements sequentially (use `TArray`/`std::vector` instead)
* You need to frequently search for elements (consider `TSet`/`std::unordered_set`)

## Notes

* All operations are exception-safe and provide strong exception safety guarantees
* Iterators remain valid unless the specific element they point to is removed
* The list maintains internal size count for O(1) `size()` operations
* Debug builds include iterator bounds checking via assertions
* The implementation is fully compatible with STL algorithms and range-based for loops
* Node pointers are exposed for advanced low-level operations when needed