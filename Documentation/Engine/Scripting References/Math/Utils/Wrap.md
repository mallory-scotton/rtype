# Wrap

Wraps a value around a range.

## Syntax

```cpp
template <typename T>
static T Wrap(const T value, const T min, const T max)
```

## Parameters

- `value`: The value to wrap.
- `min`: The minimum of the range.
- `max`: The maximum of the range.

## Return Value

The wrapped value within the range [min, max).

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    float value = 12.0f;
    float min = 0.0f;
    float max = 10.0f;
    float wrapped = Math<float>::Wrap(value, min, max); // wrapped = 2.0f
}
```
