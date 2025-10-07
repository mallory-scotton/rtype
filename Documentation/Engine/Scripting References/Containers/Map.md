# TMap

## Overview

The `TMap` template class is an unordered map wrapper that provides a key/value associative container. It follows the engine's coding conventions and is used to store and manage key/value pairs with fast lookup, insertion, and removal operations. It is similar in concept to `std::unordered_map` but integrates seamlessly with the other engine container types.

Use `TMap` when you need a hash-based dictionary for quick access by unique keys.

## Header

```cpp
#include <Engine/Core/Containers/TMap.hpp>
```

## Template Parameters

- `KeyType` — The type of keys used in the map (must be Hashable and EqualityComparable).
- `ValueType` — The type of values stored in the map.
- `Hash = std::hash<KeyType>` — (optional) The hashing functor type.
- `KeyEqual = std::equal_to<KeyType>` — (optional) The equality comparator type.

## Basic Usage

```cpp
#include <Engine/Core/Containers/TMap.hpp>
#include <iostream>

int main() {
    tkd::TMap<int, std::string> map;

    // Add key/value pairs
    map.Add(1, "one");
    map.Add(2, "two");

    // Check for existence
    if (map.Contains(1)) {
        std::cout << "Key 1 found: " << *(map.Find(1)) << std::endl;
    }

    // Iterate over the map
    for (auto it = map.begin(); it != map.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }

    return 0;
}
```

## Constructors

- `TMap()` — Default constructor, creates an empty map.
- `TMap(size_t bucketCount)` — Reserves space for approximately `bucketCount` buckets to optimize rehashing.
- `TMap(std::initializer_list<PairType> init)` — Constructs a map from an initializer list of key/value pairs.
- Copy and move constructors are supported.

## Capacity

- `bool Empty() const` — Returns true if the map is empty.
- `SizeType Num() const` — Returns the number of elements in the map.
- `void Reserve(SizeType count)` — Reserves space for at least `count` elements to reduce rehashing.
- `SizeType GetBucketCount() const` — Returns the number of buckets used by the underlying container.

## Modifiers

- `bool Add(const KeyType& key, const ValueType& value)` — Adds a key/value pair; returns true if the key was not already present.
- `bool Add(KeyType&& key, ValueType&& value)` — Move-add version for key and value.
- `template<typename... Args> bool Emplace(const KeyType& key, Args&&... args)` — Constructs a value in-place for the given key and adds the new pair if the key does not exist.
- `bool Remove(const KeyType& key)` — Removes an element by key; returns true if an element was removed.
- `void Clear()` — Removes all elements from the map.

## Lookup

- `bool Contains(const KeyType& key) const` — Returns true if the key is present in the map.
- `ValueType* Find(const KeyType& key)` — Returns a pointer to the value if found; otherwise `nullptr`.
- `const ValueType* Find(const KeyType& key) const` — Const version of `Find`.
- `ValueType& FindOrAdd(const KeyType& key, const ValueType& defaultValue = ValueType())` — If the key exists, returns a reference to its value; otherwise inserts `defaultValue` and returns a reference to the new value.

## Iterators

`TMap` provides forward iterators to traverse the contained key/value pairs:

- `iterator begin() noexcept` / `iterator end() noexcept` — Mutable iterators.
- `const_iterator begin() const noexcept` / `const_iterator end() const noexcept` — Const iterators.
- `const_iterator cbegin() const noexcept` / `const_iterator cend() const noexcept` — Explicit const iterators.

## Advanced Usage

### Underlying Container Access

- `underlying_type&& ExtractUnderlying() &&` — Moves the underlying `std::unordered_map` out of the `TMap`.

### Utility Functions

- `std::vector<KeyType> GenerateKeyArray() const` — Returns a vector containing copies of the keys.
- `std::vector<ValueType> GenerateValueArray() const` — Returns a vector containing copies of the values.

## Map Operations

The class provides both operator-based and method-based map operations:

### Operator-based Map Operations

- `operator|=(const TMap& other)` — Union-assignment: adds all key/value pairs from `other` into this map (overwriting existing values).
- `operator&=(const TMap& other)` — Intersection-assignment: keeps only key/value pairs present in both maps with equal values.
- `operator-=(const TMap& other)` — Difference-assignment: removes keys present in `other` from this map.
- `operator|(const TMap& lho, const TMap& rho)` — Returns a new map with the union of key/value pairs from `lho` and `rho`.
- `operator&(const TMap& lho, const TMap& rho)` — Returns a new map containing key/value pairs that appear in both maps with equal values.

### Method-based Map Operations

- `SizeType Append(const TMap& other)` — Appends elements from `other` into this map. Existing keys are overwritten. Returns the number of new key/value pairs inserted.
- `SizeType Intersection(const TMap& other, TMap& out) const` — Computes the intersection between this map and `other`, writing the common key/value pairs to `out`. Returns the number of pairs in the intersection.

## Comparison Operators

- `operator==` and `operator!=` — Compare two maps for equality. Two maps are equal if they contain the same key/value pairs.

## Performance and Complexity

- Average-case complexity for lookup, insertion, and removal operations is O(1). Worst-case may degrade to O(n) in case of many hash collisions or a poor hash function.
- Use `Reserve` to optimize for known or expected sizes to reduce rehashing.

## Thread Safety

`TMap` is not thread-safe for concurrent modifications. Concurrent read operations are generally safe as long as no thread is modifying the map. Use appropriate synchronization for multi-threaded access.

## Edge Cases and Notes

- Duplicate key insertions are ignored; `Add` returns false if the key already exists.
- The order of key/value pairs is not defined and may change after rehashing.
- Keys and values must be hashable and comparable. For custom types, ensure that proper specializations or overloads for `hash` and `operator==` are provided.

## Examples

Comprehensive example demonstrating typical usage:

```cpp
#include <Engine/Core/Containers/TMap.hpp>
#include <iostream>

struct Entity {
    int id;
    std::string name;
    bool operator==(const Entity& other) const { return id == other.id && name == other.name; }
};

// Provide a hash specialization for Entity
namespace std {
    template<> struct hash<Entity> {
        size_t operator()(const Entity& e) const noexcept {
            return std::hash<int>()(e.id) ^ std::hash<std::string>()(e.name);
        }
    };
}

int main() {
    tkd::TMap<int, Entity> entities;
    entities.Add(1, Entity{1, "Player"});
    entities.Add(2, Entity{2, "Enemy"});

    if (entities.Contains(1)) {
        std::cout << "Entity 1: " << entities.Find(1)->name << std::endl;
    }

    // Map operations
    tkd::TMap<int, Entity> otherEntities = {
        {2, Entity{2, "Enemy"}},
        {3, Entity{3, "NPC"}}
    };

    // Union: This will overwrite key 2 with the value from otherEntities
    auto unionMap = entities | otherEntities;

    // Intersection: Keeps only entities that are the same in both maps
    tkd::TMap<int, Entity> intersectMap;
    entities.Intersection(otherEntities, intersectMap);

    return 0;
}
```

## Related Classes

- `TSet` — Unordered set container for unique elements.
- `TArray` — Ordered contiguous container.

## Type Aliases

```cpp
using KeyT = KeyType;             // Key type
using ValueT = ValueType;         // Value type
using PairType = std::pair<const KeyType, ValueType>;
using SizeType = std::size_t;     // Size type
using HashType = Hash;            // Hash functor type
using KeyEqualType = KeyEqual;    // Key equality functor type
using underlying_type = std::unordered_map<KeyType, ValueType, Hash, KeyEqual>;
using iterator = typename underlying_type::iterator;
using const_iterator = typename underlying_type::const_iterator;
```

## Best Practices

- Use `TMap` for fast key/value lookups and when you require associative mapping.
- Ensure custom key and value types are hashable and equatable for correct behavior.
- Use `Reserve()` when an approximate element count is known to improve performance by reducing rehash operations.
- Prefer `FindOrAdd()` when you need to ensure a key exists, optionally setting a default value.
