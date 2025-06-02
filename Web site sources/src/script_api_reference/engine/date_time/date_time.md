# DateTime

Add this in your code:
```cpp
#include <engine/time/date_time.h>
```

This class represents an instant in time.

## Static methods

---
### GetNow
Get the current date and time.
```cpp
DateTime GetNow()
```
Code sample:
```cpp
DateTime currentDateTime = DateTime::GetNow();
```

## Public methods

---
### ToString
Get a string representation of the date and time in the format: "hour:minute:second day/month/year".
```cpp
std::string ToString() const
```
Code sample:
```cpp
DateTime currentDateTime = DateTime::GetNow();
Debug::Print(currentDateTime.ToString());
// Prints: "12:49:53 31/5/2025"
```

## Variables

---
| Name | Type | Description |
|-|-|-|
second | uint32_t | Number of seconds
minute | uint32_t | Number of minutes
hour | uint32_t | Number of hours
day | uint32_t | Number of days
month | uint32_t | Number of months
year | uint32_t | Number of years