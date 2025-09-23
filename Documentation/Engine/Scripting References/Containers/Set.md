# TSet

## Overview

The `TSet` template class provides an unordered collection of unique elements. It is similar in purpose to `std::unordered_set` but follows the engine's coding conventions so it can be used consistently in engine and scripting code. `TSet` offers fast average-time lookup, insertion and removal and exposes iterators and common set operations.

Use `TSet` when you need to store unique values with fast membership tests and you don't care about element ordering.

## Header

```cpp
#include <Engine/Core/Containers/TSet.hpp>
```

## Template Parameters

- `KeyType` — the element type stored in the set (must be Hashable and EqualityComparable).
- `Hash = std::hash<KeyType>` — (optional) hashing functor type.
- `KeyEqual = std::equal_to<KeyType>` — (optional) key equality functor type.

## Basic Usage

```cpp
#include <Engine/Core/Containers/TSet.hpp>
#include <iostream>

int main() {
    tkd::TSet<int> numbers;

    // Add elements
    numbers.Add(3);
    numbers.Add(7);
    numbers.Add(3); // duplicate, ignored

    // Check membership
    if (numbers.Contains(7)) {
        std::cout << "Contains 7" << std::endl;
    }

    // Iterate
    for (const auto& v : numbers) {
        std::cout << v << std::endl;
    }

    return 0;
}
```

## Constructors

- `TSet()` — default constructor, creates an empty set.
- `TSet(size_t bucketCount)` — reserve initial bucket count to avoid rehashes.
- `TSet(std::initializer_list<T> init)` — construct from initializer list.
- Copy & move constructors supported.

## Capacity

- `bool Empty() const` — returns true if the set is empty.
- `SizeType Num() const` — returns number of elements in the set.
- `void Reserve(SizeType count)` — reserve space for at least count elements.
- `SizeType GetBucketCount() const` — returns the number of buckets in use.

## Modifiers

- `bool Add(const KeyType& value)` — adds `value`; returns true if addition took place (i.e., element was not already present).
- `bool Add(KeyType&& value)` — move-add.
- `template<typename... Args> bool Emplace(Args&&... args)` — constructs element in-place and adds it.
- `bool Remove(const KeyType& value)` — removes element by value; returns true if an element was removed.
- `void Clear()` — removes all elements.

### Examples

```cpp
tkd::TSet<std::string> words;
words.Add("hello");
words.Add("world");
words.Remove("hello");
words.Clear();

// In-place construction with Emplace
struct Point { int x, y; };
tkd::TSet<Point> points;
points.Emplace(1, 2);  // Constructs Point{1, 2} in-place
```

## Lookup

- `bool Contains(const KeyType& value) const` — returns true if `value` exists in the set.
- `iterator Find(const KeyType& value)` — returns iterator to element if found, otherwise `end()`.
- `const_iterator Find(const KeyType& value) const` — const version of Find.
- `iterator FindOrAdd(const KeyType& value)` — returns iterator to existing element or adds and returns iterator to new element.

## Iterators

`TSet` provides forward iterators compatible with range-based for loops.

- `iterator begin() noexcept` / `iterator end() noexcept` — mutable iterators
- `const_iterator begin() const noexcept` / `const_iterator end() const noexcept` — const iterators
- `const_iterator cbegin() const noexcept` / `const_iterator cend() const noexcept` — explicit const iterators

Example:

```cpp
for (const auto& e : mySet) {
    // use e
}

// Using explicit iterators
for (auto it = mySet.begin(); it != mySet.end(); ++it) {
    // use *it
}
```

## Advanced Usage

### Underlying Container Access

- `underlying_type&& ExtractUnderlying() &&` — moves out the underlying `std::unordered_set`

## Set Operations

The class provides both operator-based and method-based set operations:

### Operator-based Set Operations

- `operator|=(const TSet& other)` — union-assignment: adds all elements from other into this set
- `operator&=(const TSet& other)` — intersection-assignment: keeps only elements present in both sets
- `operator-=(const TSet& other)` — difference-assignment: removes elements present in other
- `operator|(const TSet& lho, const TSet& rho)` — returns new set with elements from both sets
- `operator&(const TSet& lho, const TSet& rho)` — returns new set with elements present in both sets
- `operator-(const TSet& lho, const TSet& rho)` — returns new set with elements in lho but not in rho

### Method-based Set Operations

- `SizeType Append(const TSet& other)` — adds elements from other set, returns number added
- `SizeType Intersection(const TSet& other, TSet& out) const` — computes intersection into out parameter

Example:

```cpp
tkd::TSet<int> a {1,2,3};
tkd::TSet<int> b {3,4};
tkd::TSet<int> result;

// Using operators
auto u = a | b;  // union: {1,2,3,4}
auto i = a & b;  // intersection: {3}
auto d = a - b;  // difference: {1,2}

// Using methods
a.Append(b);     // a now contains {1,2,3,4}
a.Intersection(b, result);  // result contains {3}
```

## Comparison Operators

- `operator==` / `operator!=` — equality compares set contents (order-independent).

## Performance and Complexity

- Average-case complexity for `Insert`, `Erase`, and `Contains` is O(1).
- Worst-case complexity can degrade to O(n) if many collisions occur or a poor hash is used.
- Use `Reserve` when you have an estimate of the expected size to avoid repeated rehashing.

## Thread Safety

`TSet` is not thread-safe for concurrent modification. Concurrent reads are typically safe only if no thread is modifying the set. Use external synchronization for multi-threaded access.

## Edge Cases and Notes

- Duplicate inserts are ignored; `Insert` returns false if an equal element already exists.
- The exact iteration order is unspecified and may change after rehashes. Do not rely on order.
- Elements must be hashable and equality-comparable. For custom types, provide `hash<T>` specialization or a custom `THash` parameter.
- If `T` stores pointers, equality is pointer equality unless a custom comparator is provided.

## Examples

Comprehensive example demonstrating usage patterns:

```cpp
#include <Engine/Core/Containers/TSet.hpp>
#include <iostream>

struct Entity {
    int id;
    bool operator==(const Entity& other) const { return id == other.id; }
};

// Provide a hash specialization
namespace std {
    template<> struct hash<Entity> {
        size_t operator()(Entity const& e) const noexcept { return std::hash<int>()(e.id); }
    };
}

int main() {
    tkd::TSet<Entity> entities;
    entities.Add(Entity{1});
    entities.Add(Entity{2});

    if (entities.Contains(Entity{2})) {
        std::cout << "Found entity 2" << std::endl;
    }

    // Set difference example
    tkd::TSet<int> a {1,2,3,4};
    tkd::TSet<int> b {3,4,5};
    auto diff = a - b;
    for (auto v : diff) std::cout << v << std::endl; // prints 1 2 (order unspecified)

    return 0;
}
```

## Related Classes

- `TMap` — associative container mapping keys to values.
- `TArray` — ordered contiguous containers.

## Type Aliases

The class provides these type aliases for convenience:

```cpp
using ElementType = KeyType;          // Element type stored in the set
using SizeType = std::size_t;         // Size type
using HashType = Hash;                // Hash functor type
using KeyEqualType = KeyEqual;        // Key equality functor type
using underlying_type = std::unordered_set<KeyType, Hash, KeyEqual>;
using iterator = typename underlying_type::iterator;
using const_iterator = typename underlying_type::const_iterator;
```

## Best Practices

- Prefer `TSet` when only uniqueness and fast lookup are needed
- Use `TSet` for membership tests rather than `TArray` + linear search
- Provide efficient `hash` and `operator==` for custom types
- Use `Reserve()` when you know the approximate element count
- Use `Emplace()` for in-place construction of complex types
- Consider `FindOrAdd()` when you need to ensure an element exists
