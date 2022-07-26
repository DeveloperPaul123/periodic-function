<h1 align="center">
periodic-function
</h1>

[![say thanks](https://img.shields.io/badge/Say%20Thanks-👍-1EAEDB.svg)](https://github.com/DeveloperPaul123/periodic-function/stargazers)
[![Discord](https://img.shields.io/discord/652515194572111872)](https://discord.gg/CX2ybByRnt)

[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/MacOS/badge.svg)](https://github.com/DeveloperPaul123/periodic-function/actions)
[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/Windows/badge.svg)](https://github.com/DeveloperPaul123/periodic-function/actions)
[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/Ubuntu/badge.svg)](https://github.com/DeveloperPaul123/periodic-function/actions)
[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/Ubuntu-20.04/badge.svg)](https://github.com/DeveloperPaul123/periodic-function/actions)
[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/Style/badge.svg)](https://github.com/DeveloperPaul123/periodic-function/actions)
[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/Install/badge.svg)](https://github.com/DeveloperPaul123/periodic-function/actions)

Simple library to repeatedly call a member function, free function or lambda at a given time interval.

## Features

* Use a variety of callback types:
  * Class member functions via `std::bind`
  * Free functions
  * Lambdas
* RAII cleanup, don't have to worry about explicitly calling `stop()`.
* Reliable function timing (tested to be within ~1 millisecond)
* Auto-recovery if callback takes longer than interval time.

## Usage

```cpp
#include <periodic_function/periodic_function.hpp>

// call function every 300 milliseconds
dp::periodic_function heartbeat([]() {
    // do something here...
}, 300U);

// start calling function
heartbeat.start();

// optional: stop calling the function
// function will stop being called when object goes out of scope
heartbeat.stop();
```

## Customization Points

### Handling Callbacks that Exceed the Timer Interval

How callbacks that exceed the interval are handled is passed on a template argument policy class. The current policies available are:

#### `schedule_next_missed_interval_policy` (**default**)

This will schedule the callback to be called again on the next interval timeout (the interval that was missed is skipped). This is good to use if the callback is not expected to exceed the interval time.

#### `invoke_immediately_missed_interval_policy`

This will schedule the callback to be called immediately and then control will be given back to the timer which will operate at the regular interval.

## Building

`periodic-function` **requires** C++17 support and has been tested with:

* Visual Studio 2019 (msvc)
* Visual Studio 2019 (clang-cl)
* Ubuntu 18/20.04 GCC 10
* Ubuntu 18/20.04 Clang 10

Use the following commands to build the project

```bash
cmake -S <source_dir> -B build
cmake --build build --config Release
```

To run tests, simple `cd` into the build directory and run:

```bash
ctest --build-config Debug
```

## Contributing

Contributions are very welcome. Please see [contribution guidelines for more info](CONTRIBUTING.md).

## License

The project is licensed under the MIT license. See [LICENSE](LICENSE) for more details.

## Author

| [<img src="https://avatars0.githubusercontent.com/u/6591180?s=460&v=4" width="100"><br><sub>@DeveloperPaul123</sub>](https://github.com/DeveloperPaul123) |
|:----:|

### WIP Items

:construction: MacOS support is still a WIP. :construction:
