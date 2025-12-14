# Universal Hardware Timer

> [!NOTE]
> This repository is still being developed and is currently not in a stable state

General purpose hardware timer for common microcontrollers. This is made to be adaptable for multiple different boards and development environments, not just for Arduino API's. See [here](docs/Support.md) for microcontroller support and [here](docs/Implement.md) for how to implement into projects. To test the implementation of timers for a given board and environment, a [test project](https://github.com/CamrenChraplak/universal_hardware_timer_tests) was created.

## Usage

After installing the library, include `universal_hardware_timer.h` into the source file.

```c
#include <universal_hardware_timer.h>
```

To set a continuous timer, a function must be created of type `void` and with params `void*`.

```c
void functionName(void *params) {
	// implementation
}
```

To then set the timer, call `setHardTimer`.

```c
hard_timer_freq_t freq = 1000; // desired frequency in Hz
bool success = setHardTimer(
	HARD_TIMER_INVALID, // hardware timer to use, this input uses best available
	&freq, // pointer to desired frequency, may not be actual frequency
	&functionName, // pointer to callback function
	NULL, // parameters to pass into callback function
	0 // priority of function from 0 (lowest) to 255 (highest)
);
```

`setHardTimer` will return if the timer was able to successfully set. The `freq` variable will be updated with the actual frequency achieved. Since each platform/board has different methods for setting timers, none are guaranteed to be the exact frequency given.