# IsEven

Checks if an integer is even.

## Syntax

```cpp
template <typename T>
static Bool IsEven(const T value)
```

## Parameters

- `value`: The value to check.

## Return Value

True if the value is even, false otherwise. For non-integral types, always returns false.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    int val1 = 4;
    bool even1 = Math<int>::IsEven(val1); // even1 = true

    int val2 = 5;
    bool even2 = Math<int>::IsEven(val2); // even2 = false

    float fval = 4.0f;
    bool feven = Math<float>::IsEven(fval); // feven = false
}
```
