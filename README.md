<h1 align="center">
periodic-function
</h1>

[![say thanks](https://img.shields.io/badge/Say%20Thanks-👍-1EAEDB.svg)](https://github.com/DeveloperPaul123/periodic-function/stargazers)
[![Discord](https://img.shields.io/discord/652515194572111872)](https://img.shields.io/discord/652515194572111872)

[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/MacOS/badge.svg)](https://github.com/DeveloperPaul123/periodic-function/actions)
[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/Windows/badge.svg)](https://github.com/DeveloperPaul123/periodic-function/actions)
[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/Ubuntu/badge.svg)](https://github.com/DeveloperPaul123/periodic-function/actions)
[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/Ubuntu-20.04/badge.svg)](https://github.com/DeveloperPaul123/periodic-function/actions)
[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/Style/badge.svg)](https://github.com/DeveloperPaul123/periodic-function/actions)
[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/Install/badge.svg)](https://github.com/DeveloperPaul123/periodic-function/actions)

Simple library to repeatedly call a member function, free function or lambda at a given time interval.

## Features

* Use various callback types:
  * Class member functions via `std::bind`
  * Free functions
  * Lambdas
* RAII cleanup, don't have to worry about explicitly calling `stop()`.

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

## Building

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

Performance on MacOS is currently not up to my standards. This is actively being investigated.
