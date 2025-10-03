# IsNaN

Checks if a value is NaN (Not a Number).

## Syntax

```cpp
template <typename T>
static Bool IsNaN(const T value)
```

## Parameters

- `value`: The value to check.

## Return Value

True if the value is NaN, false otherwise. For non-floating-point types, always returns false.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>
#include <limits>

int main() {
    float nan = std::numeric_limits<float>::quiet_NaN();
    bool isNan = Math<float>::IsNaN(nan); // isNan = true

    float finite = 3.14f;
    bool isFinite = Math<float>::IsNaN(finite); // isFinite = false

    int ival = 42;
    bool inan = Math<int>::IsNaN(ival); // inan = false
}
```
