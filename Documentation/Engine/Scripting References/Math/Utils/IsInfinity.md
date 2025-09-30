# IsInfinity

Checks if a value is infinity.

## Syntax

```cpp
template <typename T>
static Bool IsInfinity(const T value)
```

## Parameters

- `value`: The value to check.

## Return Value

True if the value is infinity, false otherwise. For non-floating-point types, always returns false.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>
#include <limits>

int main() {
    float inf = std::numeric_limits<float>::infinity();
    bool isInf = Math<float>::IsInfinity(inf); // isInf = true

    float finite = 3.14f;
    bool isFinite = Math<float>::IsInfinity(finite); // isFinite = false

    int ival = 42;
    bool iinf = Math<int>::IsInfinity(ival); // iinf = false
}
```
