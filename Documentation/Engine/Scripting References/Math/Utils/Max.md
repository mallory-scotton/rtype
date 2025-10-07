# Max

Returns the maximum of two values.

## Syntax

```cpp
template <typename T>
static T Max(const T a, const T b)
```

## Parameters

- `a`: The first value.
- `b`: The second value.

## Return Value

The maximum of the two values.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    int a = 5;
    int b = 10;
    int maxVal = Math<int>::Max(a, b); // maxVal = 10

    float fa = 3.14f;
    float fb = 2.71f;
    float fmax = Math<float>::Max(fa, fb); // fmax = 3.14f
}
```
