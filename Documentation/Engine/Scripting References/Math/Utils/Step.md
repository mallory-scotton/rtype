# Step

Step function.

## Syntax

```cpp
template <typename T>
static T Step(const T edge, const T x)
```

## Parameters

- `edge`: The threshold value.
- `x`: The value to test.

## Return Value

0 if x < edge, 1 otherwise.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    float edge = 5.0f;
    float x1 = 3.0f;
    float step1 = Math<float>::Step(edge, x1); // step1 = 0.0f

    float x2 = 7.0f;
    float step2 = Math<float>::Step(edge, x2); // step2 = 1.0f
}
```
