# IsPowerOfTwo

Checks if an integer is a power of two.

## Syntax

```cpp
template <typename T>
static Bool IsPowerOfTwo(const T value)
```

## Parameters

- `value`: The integer to check.

## Return Value

True if the value is a power of two, false otherwise. For non-integral types, always returns false.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    int val1 = 8;
    bool power1 = Math<int>::IsPowerOfTwo(val1); // power1 = true

    int val2 = 7;
    bool power2 = Math<int>::IsPowerOfTwo(val2); // power2 = false

    float fval = 8.0f;
    bool fpower = Math<float>::IsPowerOfTwo(fval); // fpower = false
}
```
