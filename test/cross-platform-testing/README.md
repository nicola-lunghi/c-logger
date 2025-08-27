# Comprehensive Cross-Platform Testing Guide for Logger Library

This guide demonstrates mul### Bare Metal / No OS
```bash
cmake -DCMAKE_C_FLAGS="-DLOGGER_PLATFORM_ALT -DHAVE_SYS_TIME_H=0 -DHAVE_PTHREAD_H=0 -DHAVE_UNISTD_H=0 \
-DHAVE_SYS_SYSCALL_H=0 -DENABLE_THREADING=0 -DHAVE_GETTIMEOFDAY=0 \
-DHAVE_LOCALTIME_R=0" ..
```

### Custom I/O Environment
```bash
cmake -DCMAKE_C_FLAGS="-DLOGGER_PLATFORM_ALT -DHAVE_STDIO_H=0" ..
# Requires custom logger_platform_printf, logger_platform_vprintf, logger_platform_flush
# implementations for UART, memory-mapped I/O, or other custom output methods
```methods to test compilation when certain headers or functions are not available, simulating different platforms and environments.

## Quick Start

### Automated Testing
```bash
# Run all cross-platform tests automatically
./test-missing-headers.sh

# Run advanced testing scenarios
./advanced-tests.sh
```

### Manual Testing Examples

## Method 1: CMake Compiler Flags Override
Use -D flags to override the detected HAVE_* values
```bash
cmake -DCMAKE_C_FLAGS="-DHAVE_SYS_TIME_H=0 -DHAVE_PTHREAD_H=0" ..
```

## Method 2: Custom Config Header Override
Create a custom config.h and force include it
```bash
echo "#define HAVE_SYS_TIME_H 0" > custom_config.h
cmake -DCMAKE_C_FLAGS="-include \$(pwd)/custom_config.h" ..
```

## Method 3: Header File Shadowing
Create empty/dummy headers to hide real ones
```bash
mkdir fake-headers
touch fake-headers/time.h
cmake -DCMAKE_C_FLAGS="-I\$(pwd)/fake-headers -DHAVE_SYS_TIME_H=0" ..
```

## Method 4: Function Name Redefinition
Use preprocessor to make functions appear missing
```bash
cmake -DCMAKE_C_FLAGS="-Dgettimeofday=missing_gettimeofday" ..
```

## Method 5: Cross-Compilation Testing
Use different toolchains or containers
```bash
docker run --rm -v \$(pwd):/src -w /src gcc:9 \
  cmake -DCMAKE_C_FLAGS="-DHAVE_SYS_TIME_H=0" .
```

## Method 6: Static Analysis Tools
Use tools to detect missing includes
```bash
cppcheck --enable=all --std=c99 src/
```

## Method 7: Alternate Implementation Testing
Test with LOGGER_PLATFORM_ALT defined
```bash
cmake -DCMAKE_C_FLAGS="-DLOGGER_PLATFORM_ALT" ..
# This will fail to link unless you provide logger_platform_alt.h
```

## Method 8: Custom I/O Function Testing
Test with custom printf and flush implementations
```bash
cmake -DCMAKE_C_FLAGS="-DLOGGER_PLATFORM_ALT" ..
# Implement custom logger_platform_printf, logger_platform_vprintf, logger_platform_flush
# in your logger_platform_alt.h for custom UART drivers, embedded I/O, etc.
```

## Common Test Scenarios:

### Embedded System (Minimal)
```bash
cmake -DCMAKE_C_FLAGS="-DHAVE_SYS_TIME_H=0 -DHAVE_GETTIMEOFDAY=0 -DHAVE_PTHREAD_H=0 \
-DENABLE_THREADING=0 -DHAVE_UNISTD_H=0 -DHAVE_SYS_SYSCALL_H=0" ..
```

### Windows-like Environment
```bash
cmake -DCMAKE_C_FLAGS="-DHAVE_SYS_TIME_H=0 -DHAVE_PTHREAD_H=0 -DPLATFORM_LINUX=0 \
-DPLATFORM_WINDOWS=1" ..
```

### RTOS Environment
```bash
cmake -DCMAKE_C_FLAGS="-DHAVE_SYS_TIME_H=0 -DHAVE_PTHREAD_H=0 -DHAVE_UNISTD_H=0 \
-DHAVE_SYS_SYSCALL_H=0 -DENABLE_THREADING=0" ..
```

### Bare Metal / No OS
```bash
cmake -DCMAKE_C_FLAGS="-DHAVE_SYS_TIME_H=0 -DHAVE_PTHREAD_H=0 -DHAVE_UNISTD_H=0 \
-DHAVE_SYS_SYSCALL_H=0 -DENABLE_THREADING=0 -DHAVE_GETTIMEOFDAY=0 \
-DHAVE_LOCALTIME_R=0" ..
```

## Automated Testing Script
See `test-missing-headers.sh` for automated testing
See `advanced-tests.sh` for more sophisticated scenarios

## Tips:
1. Always test with -Werror to catch warnings as errors
2. Use multiple compilers (gcc, clang, msvc) when possible
3. Test both debug and release builds
4. Verify that all tests still pass in degraded mode
5. Document platform-specific limitations

## Docker Testing Examples

### Alpine Linux (Minimal)
```bash
docker run --rm -v \$(pwd):/src -w /src alpine:latest \
  apk add --no-cache cmake gcc make && \
  cmake -DCMAKE_C_FLAGS="-DHAVE_SYS_TIME_H=0" . && \
  make
```

### Ubuntu with Different GCC Versions
```bash
docker run --rm -v \$(pwd):/src -w /src ubuntu:18.04 \
  apt-get update && apt-get install -y cmake gcc make && \
  cmake -DCMAKE_C_FLAGS="-DHAVE_PTHREAD_H=0" . && \
  make
```

### CentOS / RHEL
```bash
docker run --rm -v \$(pwd):/src -w /src centos:7 \
  yum install -y cmake gcc make && \
  cmake -DCMAKE_C_FLAGS="-DHAVE_SYS_TIME_H=0 -DHAVE_PTHREAD_H=0" . && \
  make
```
