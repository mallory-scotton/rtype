# IsOdd

Checks if an integer is odd.

## Syntax

```cpp
template <typename T>
static Bool IsOdd(const T value)
```

## Parameters

- `value`: The value to check.

## Return Value

True if the value is odd, false otherwise. For non-integral types, always returns false.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    int val1 = 5;
    bool odd1 = Math<int>::IsOdd(val1); // odd1 = true

    int val2 = 4;
    bool odd2 = Math<int>::IsOdd(val2); // odd2 = false

    float fval = 5.0f;
    bool fodd = Math<float>::IsOdd(fval); // fodd = false
}
```
