[![Build Status](https://travis-ci.org/yksz/c-logger.svg?branch=master)](https://travis-ci.org/yksz/c-logger)

# C logger

## Table of Contents
- [What is this?](#what-is-this)
- [Installation](#installation)
- [Testing](#testing)
- [Platform Customization](#platform-customization)
- [Platform](#platform)
- [Benchmark](#benchmark)
- [Log format](#log-format)
- [Example](#example)
- [License](#license)


## What is this?
This is a simple logging library for C/C++.

This library's main features:
- Lightweight - only 500-line source code
- C89 support
- Thread-safe
- 2 logging types:
  - Console logging
  - File logging rotated by file size
- Custom with a configuration file


## Installation
```
./build.sh
cd build
make
sudo make install
```
or

```
build.bat
```
or

Copy files in src directory to your project


## Testing

### Basic Testing
```bash
# Build and run tests
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON
make
ctest
```

### Cross-Platform Testing
The library includes comprehensive cross-platform testing infrastructure to ensure compatibility across different environments and platforms.

#### Quick Cross-Platform Test
```bash
# Run automated cross-platform tests
cd test/cross-platform-testing
./test-missing-headers.sh
```

#### Advanced Cross-Platform Testing
```bash
# Test with specific platform constraints
cd test/cross-platform-testing
./advanced-tests.sh
```

#### Manual Cross-Platform Testing
Test the library with missing headers/functions to simulate different platforms:

```bash
# Test without time.h support
cmake -DCMAKE_C_FLAGS="-DHAVE_SYS_TIME_H=0" ..

# Test without threading support
cmake -DCMAKE_C_FLAGS="-DHAVE_PTHREAD_H=0 -DENABLE_THREADING=0" ..

# Test embedded environment (minimal features)
cmake -DCMAKE_C_FLAGS="-DHAVE_SYS_TIME_H=0 -DHAVE_GETTIMEOFDAY=0 -DHAVE_PTHREAD_H=0 -DENABLE_THREADING=0 -DHAVE_UNISTD_H=0 -DHAVE_SYS_SYSCALL_H=0" ..

# Test with alternate platform implementation
cmake -DCMAKE_C_FLAGS="-DLOGGER_PLATFORM_ALT" ..
```

#### Testing with Custom Config
```bash
# Create custom configuration for specific platform
cd test/cross-platform-testing
echo "#define HAVE_SYS_TIME_H 0" > custom_config.h
cmake -DCMAKE_C_FLAGS="-include \$(pwd)/custom_config.h" ..
```

### Supported Test Scenarios
- **Embedded systems** (minimal C standard library)
- **RTOS environments** (no threading, limited system calls)
- **Windows-like environments** (different header availability)
- **Custom platform implementations** (alternate function implementations)

See `test/cross-platform-testing/README.md` and `test/cross-platform-testing/CROSS_PLATFORM_TESTING.md` for detailed testing instructions.


## Platform Customization

The logger library supports extensive platform customization to adapt to different environments, operating systems, and hardware constraints.

### Using Alternate Platform Implementation

For platforms that require completely custom implementations (e.g., embedded systems, RTOS, bare metal), you can provide alternate platform functions:

#### Step 1: Enable Alternate Platform Mode
Define `LOGGER_PLATFORM_ALT` in your build:
```bash
cmake -DCMAKE_C_FLAGS="-DLOGGER_PLATFORM_ALT" ..
```

#### Step 2: Create Custom Platform Header
Copy and modify the example alternate platform header:
```bash
cp include/logger_platform_alt.h my_platform_logger.h
# Edit my_platform_logger.h with your platform-specific implementations
```

#### Step 3: Implement Required Functions
Your custom platform header must implement these functions:

```c
// Platform initialization
void logger_platform_init(void);

// Thread synchronization
void logger_platform_lock(void);
void logger_platform_unlock(void);

// Thread identification
long logger_platform_get_current_thread_id(void);

// Time formatting
void logger_platform_get_timestamp(const struct logger_timeval* time, char* timestamp, size_t size);

// Initialization check
int logger_platform_is_initialized(void);
```

#### Step 4: Example Custom Implementation
```c
// Example for an RTOS platform
void logger_platform_init(void) {
    // Initialize RTOS-specific resources
    rtos_mutex_create(&logger_mutex);
    rtos_timer_init();
}

void logger_platform_lock(void) {
    rtos_mutex_lock(&logger_mutex);
}

void logger_platform_unlock(void) {
    rtos_mutex_unlock(&logger_mutex);
}

long logger_platform_get_current_thread_id(void) {
    return rtos_get_current_task_id();
}

void logger_platform_get_timestamp(const struct logger_timeval* time, char* timestamp, size_t size) {
    rtos_format_time(time->tv_sec, time->tv_usec, timestamp, size);
}

int logger_platform_is_initialized(void) {
    return rtos_is_ready();
}
```

### Platform-Specific Configuration

#### Embedded Systems (Minimal Resources)
```bash
# Disable threading and time functions
cmake -DCMAKE_C_FLAGS="-DHAVE_PTHREAD_H=0 -DENABLE_THREADING=0 -DHAVE_SYS_TIME_H=0 -DHAVE_GETTIMEOFDAY=0" ..
```

#### RTOS Environment
```bash
# Use custom threading and time implementations
cmake -DCMAKE_C_FLAGS="-DLOGGER_PLATFORM_ALT -DHAVE_PTHREAD_H=0 -DHAVE_UNISTD_H=0" ..
```

#### Windows Platform
```bash
# Use Windows-specific headers and functions
cmake -DCMAKE_C_FLAGS="-DPLATFORM_WINDOWS=1 -DHAVE_SYS_TIME_H=0" ..
```

#### Bare Metal / No OS
```bash
# Minimal implementation with custom platform functions
cmake -DCMAKE_C_FLAGS="-DLOGGER_PLATFORM_ALT -DHAVE_SYS_TIME_H=0 -DHAVE_PTHREAD_H=0 -DHAVE_UNISTD_H=0 -DHAVE_SYS_SYSCALL_H=0 -DENABLE_THREADING=0" ..
```

#### Custom I/O Functions
For platforms that require custom I/O implementations (e.g., embedded systems with custom UART drivers, or systems without standard C library):

```c
// Custom I/O implementations
int logger_platform_printf(FILE* stream, const char* format, ...) {
    // Your custom printf implementation
    return custom_printf(stream, format, ...);
}

int logger_platform_vprintf(FILE* stream, const char* format, va_list args) {
    // Your custom vprintf implementation  
    return custom_vprintf(stream, format, args);
}

int logger_platform_flush(FILE* stream) {
    // Your custom flush implementation
    return custom_flush(stream);
}
```

### Custom Configuration Header

Create a custom `logger_config.h` to override default settings:

```c
// Example custom configuration for embedded system
#ifndef LOGGER_CONFIG_H
#define LOGGER_CONFIG_H

// Disable standard headers
#define HAVE_SYS_TIME_H 0
#define HAVE_PTHREAD_H 0
#define HAVE_UNISTD_H 0
#define HAVE_SYS_SYSCALL_H 0

// Disable threading
#define ENABLE_THREADING 0

// Use alternate platform implementation
#define LOGGER_PLATFORM_ALT

// Custom platform header
#define LOGGER_PLATFORM_ALT_H "my_custom_platform.h"

#endif /* LOGGER_CONFIG_H */
```

### Integration Examples

#### Arduino Platform
```c
// Arduino-specific implementation
void logger_platform_init(void) {
    Serial.begin(9600);
}

void logger_platform_lock(void) {
    // Arduino is single-threaded, no-op
}

long logger_platform_get_current_thread_id(void) {
    return 1; // Single thread
}
```

#### FreeRTOS Platform
```c
// FreeRTOS-specific implementation
static SemaphoreHandle_t logger_mutex;

void logger_platform_init(void) {
    logger_mutex = xSemaphoreCreateMutex();
}

void logger_platform_lock(void) {
    xSemaphoreTake(logger_mutex, portMAX_DELAY);
}

void logger_platform_unlock(void) {
    xSemaphoreGive(logger_mutex);
}
```

#### Custom Hardware Timer
```c
// Custom timer implementation
void logger_platform_get_timestamp(const struct logger_timeval* time, char* timestamp, size_t size) {
    uint32_t ticks = custom_timer_get_ticks();
    uint32_t seconds = ticks / CUSTOM_TIMER_FREQ;
    uint32_t microseconds = (ticks % CUSTOM_TIMER_FREQ) * 1000000 / CUSTOM_TIMER_FREQ;
    
    snprintf(timestamp, size, "%02d-%02d-%02d %02d:%02d:%02d.%06d",
             0, 0, 0,  // Date not available
             (seconds / 3600) % 24, (seconds / 60) % 60, seconds % 60,
             microseconds);
}
```

### Best Practices

1. **Test Thoroughly**: Always test your custom implementations with the cross-platform testing scripts
2. **Document Limitations**: Document any platform-specific limitations or differences
3. **Maintain Compatibility**: Keep the API compatible with the standard implementation when possible
4. **Handle Errors Gracefully**: Implement proper error handling for platform-specific failures
5. **Performance Considerations**: Optimize for your platform's constraints (memory, speed, etc.)

See `include/logger_platform_alt.h` for a complete example implementation and `test/cross-platform-testing/` for testing your custom platform code.


## Platform
- Windows (Visual Studio 2008+)
- Mac OS X (clang 3.6+)
- Linux (gcc 4.8.4+)

Note: Operations are confirmed on the above platforms


## Benchmark
This following table comparing other libraries is benchmark results to log 1,000,000 times.

| threads  | c-logger | glog     |
|:---------|---------:|---------:|
| 1        | 0.832s   | 1.267s   |
| 10       | 1.386s   | 1.183s   |

**Benchmark environment**
- Processor: Intel Core i3-4170 CPU @ 3.70GHz
- Memory: 8.0GB
- OS: Ubuntu 16.04 64bit


## Log format
```
level yy-MM-dd hh:mm:ss:uuuuuu threadid file:line: message
```


## Example
#### Console logging
```c
logger_initConsoleLogger(stderr);
logger_setLevel(LogLevel_DEBUG);
LOG_INFO("console logging");
```

#### File logging
```c
logger_initFileLogger("logs/log.txt", 1024 * 1024, 5);
logger_setLevel(LogLevel_DEBUG);
LOG_INFO("file logging");
LOG_DEBUG("format example: %d%c%s", 1, '2', "3");
```

The output of the file logger is the following:
```
I 15-11-10 00:32:43.771455 2854 filelogger.c:6: file logging
D 15-11-10 00:32:43.771564 2854 filelogger.c:7: format example: 123
```

#### Multi logging
```c
logger_initConsoleLogger(NULL);
logger_initFileLogger("logs/log.txt", 0, 0);
LOG_INFO("multi logging");
```


## License
The MIT license
