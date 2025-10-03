# Sign

Returns the sign of a value.

## Syntax

```cpp
template <typename T>
static T Sign(const T value)
```

## Parameters

- `value`: The value.

## Return Value

-1 if negative, 1 if positive, 0 if zero.

## Example

```cpp
#include <Engine/Core/Math/Utils.hpp>

int main() {
    int pos = 5;
    int signPos = Math<int>::Sign(pos); // signPos = 1

    int neg = -3;
    int signNeg = Math<int>::Sign(neg); // signNeg = -1

    int zero = 0;
    int signZero = Math<int>::Sign(zero); // signZero = 0
}
```
