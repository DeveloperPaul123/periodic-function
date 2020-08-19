[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/MacOS/badge.svg)](https://github.com/TheLartians/ModernCppStarter/actions)
[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/Windows/badge.svg)](https://github.com/TheLartians/ModernCppStarter/actions)
[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/Ubuntu/badge.svg)](https://github.com/TheLartians/ModernCppStarter/actions)
[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/Style/badge.svg)](https://github.com/TheLartians/ModernCppStarter/actions)
[![Actions Status](https://github.com/DeveloperPaul123/periodic-function/workflows/Install/badge.svg)](https://github.com/TheLartians/ModernCppStarter/actions)
<!-- [![codecov](https://codecov.io/gh/TheLartians/ModernCppStarter/branch/master/graph/badge.svg)](https://codecov.io/gh/TheLartians/ModernCppStarter) -->

# periodic-function

Simple library to repeatedly call a member function, free function or lambda at a given time interval.

## Features

## Usage

```cpp
#include <periodic_function/periodic_function.hpp>

// call function every 300 milliseconds
dp::periodic_function heartbeat([]() {
    // do something here...
}, 300U);

// start calling function
heartbeat.start();

// stop calling the function, function will stop being called upon desctruction if stop() isn't called.
heartbeat.stop();
```

### Building

Use the following commands to build the project

```bash
cmake -S <source_dir> -B build
cmake --build build --config Release
```

To run tests, simple `cd` into the build directory and run:

```bash
ctest --build-config Debug
```

To collect code coverage information, run CMake with the `-DENABLE_TEST_COVERAGE=1` option.
