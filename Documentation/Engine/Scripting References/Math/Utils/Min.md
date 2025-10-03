# Min

Returns the minimum of two values.

## Syntax

```cpp
template <typename T>
static T Min(const T a, const T b)
```

## Parameters

- `a`: The first value.
- `b`: The second value.

## Return Value

The minimum of the two values.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    int a = 5;
    int b = 10;
    int minVal = Math<int>::Min(a, b); // minVal = 5

    float fa = 3.14f;
    float fb = 2.71f;
    float fmin = Math<float>::Min(fa, fb); // fmin = 2.71f
}
```
