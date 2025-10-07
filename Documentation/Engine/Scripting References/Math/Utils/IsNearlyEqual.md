# IsNearlyEqual

Checks if two floating point values are nearly equal.

## Syntax

```cpp
template <typename T>
static Bool IsNearlyEqual(
    const T a,
    const T b,
    const T epsilon = std::numeric_limits<T>::epsilon()
)
```

## Parameters

- `a`: The first value.
- `b`: The second value.
- `epsilon`: The tolerance (default is machine epsilon for the type).

## Return Value

True if the values are nearly equal within the given epsilon, false otherwise. For non-floating-point types, checks for exact equality.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    float a = 1.0000001f;
    float b = 1.0000002f;
    bool nearly = Math<float>::IsNearlyEqual(a, b); // nearly = true

    float c = 1.0f;
    float d = 2.0f;
    bool notNearly = Math<float>::IsNearlyEqual(c, d); // notNearly = false

    int ia = 1;
    int ib = 1;
    bool equal = Math<int>::IsNearlyEqual(ia, ib); // equal = true
}
```
