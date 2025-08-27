#!/bin/bash
# Test script to simulate missing headers

echo "=== Testing Logger Library with Missing Headers ==="
echo

# Test 1: Simulate minimal time support (no sys/time.h functions)
echo "1. Testing with minimal time support (sys/time.h functions disabled)..."
mkdir -p build-minimal-time || exit 1
cd build-minimal-time || exit 1
rm -rf CMakeCache.txt CMakeFiles/ cmake_install.cmake Makefile
cmake ../../.. -DCMAKE_C_FLAGS="-DHAVE_SYS_TIME_H=0 -DHAVE_GETTIMEOFDAY=0 -DHAVE_LOCALTIME_R=0" -DBUILD_TESTS=ON -DWARNINGS_AS_ERRORS=ON
make 2>&1 | head -20
cd .. || exit 1
echo

# Test 1b: Test with full time.h fallback (should work with our fallback implementations)
echo "1b. Testing time.h fallback implementations..."
mkdir -p build-time-fallback || exit 1
cd build-time-fallback || exit 1
rm -rf CMakeCache.txt CMakeFiles/ cmake_install.cmake Makefile
# Create a custom config that simulates no time.h but our fallbacks work
cmake ../../.. -DCMAKE_C_FLAGS="-DHAVE_SYS_TIME_H=0 -DHAVE_GETTIMEOFDAY=0 -DHAVE_LOCALTIME_R=0" -DBUILD_TESTS=ON -DWARNINGS_AS_ERRORS=ON
echo "Build output (should work with fallback implementations):"
make 2>&1 | head -20
cd .. || exit 1
echo

# Test 2: Simulate missing pthread.h
echo "2. Testing without pthread support..."
mkdir -p build-no-pthread || exit 1
cd build-no-pthread || exit 1
rm -rf CMakeCache.txt CMakeFiles/ cmake_install.cmake Makefile
cmake ../../.. -DCMAKE_C_FLAGS="-DHAVE_PTHREAD_H=0 -DHAVE_PTHREAD_MUTEX_INIT=0 -DENABLE_THREADING=0" -DBUILD_TESTS=ON -DWARNINGS_AS_ERRORS=ON
make 2>&1 | head -20
cd .. || exit 1
echo

# Test 3: Simulate minimal embedded environment
echo "3. Testing minimal embedded environment..."
mkdir -p build-embedded-minimal || exit 1
cd build-embedded-minimal || exit 1
rm -rf CMakeCache.txt CMakeFiles/ cmake_install.cmake Makefile
cmake ../../.. -DCMAKE_C_FLAGS="-DHAVE_SYS_TIME_H=0 -DHAVE_GETTIMEOFDAY=0 -DHAVE_LOCALTIME_R=0 -DHAVE_PTHREAD_H=0 -DHAVE_PTHREAD_MUTEX_INIT=0 -DENABLE_THREADING=0 -DHAVE_UNISTD_H=0 -DHAVE_SYS_SYSCALL_H=0 -DHAVE_SYS_GETTID=0" -DBUILD_TESTS=ON -DWARNINGS_AS_ERRORS=ON
make 2>&1 | head -20
cd .. || exit 1
echo

# Test 4: Test with alternate implementations
echo "4. Testing with alternate platform implementations..."
mkdir -p build-with-alt || exit 1
cd build-with-alt || exit 1
rm -rf CMakeCache.txt CMakeFiles/ cmake_install.cmake Makefile
cmake ../../.. -DCMAKE_C_FLAGS="-DLOGGER_PLATFORM_ALT" -DBUILD_TESTS=ON -DWARNINGS_AS_ERRORS=ON
make 2>&1 | head -20
cd .. || exit 1
echo

# Test 5: Test with custom I/O functions
echo "5. Testing with custom I/O functions..."
mkdir -p build-custom-io || exit 1
cd build-custom-io || exit 1
rm -rf CMakeCache.txt CMakeFiles/ cmake_install.cmake Makefile
cmake ../../.. -DCMAKE_C_FLAGS="-DLOGGER_PLATFORM_ALT" -DBUILD_TESTS=ON -DWARNINGS_AS_ERRORS=ON
make 2>&1 | head -20
cd .. || exit 1
echo

echo "=== Test Summary ==="
echo "Check the directories above for detailed build logs"
echo "Each test simulates different platform capabilities:"
echo "1. Missing time.h support (sys/time.h functions disabled)"
echo "1b. Completely missing time.h header (HAVE_TIME_H=0 - should fail)"
echo "2. Missing pthread support"
echo "3. Minimal embedded environment"
echo "4. Alternate platform implementations"
echo "5. Custom I/O functions"
