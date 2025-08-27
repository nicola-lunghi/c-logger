#!/bin/bash
set -euf -o pipefail

# Advanced testing methods for missing headers

echo "=== Method 2: Using -include to override config ==="
mkdir -p build-override-config || exit 1
cd build-override-config || exit 1
# Copy our custom config that disables all features
cp ../custom_config.h config.h
# Clean up any existing CMake cache
rm -rf CMakeCache.txt CMakeFiles/ cmake_install.cmake Makefile
# Build with -include to force our config
cmake ../../.. -DCMAKE_C_FLAGS="-include $(pwd)/config.h" -DBUILD_TESTS=ON -DWARNINGS_AS_ERRORS=ON
make 2>&1 | head -15
cd .. || exit 1

echo
echo "=== Method 3: Direct header file override ==="
mkdir -p build-header-override || exit 1
cd build-header-override || exit 1
# Create fake empty headers to simulate missing ones
mkdir -p fake-sys
touch fake-sys/time.h
rm -rf CMakeCache.txt CMakeFiles/ cmake_install.cmake Makefile
cmake ../../.. -DCMAKE_C_FLAGS="-I$(pwd)/fake-sys -DHAVE_SYS_TIME_H=0" -DBUILD_TESTS=ON -DWARNINGS_AS_ERRORS=ON
make 2>&1 | head -15
cd .. || exit 1

echo
echo "=== Method 4: Using preprocessor to hide functions ==="
mkdir -p build-hide-functions || exit 1
cd build-hide-functions || exit 1
rm -rf CMakeCache.txt CMakeFiles/ cmake_install.cmake Makefile
cmake ../../.. -DCMAKE_C_FLAGS="-DHAVE_GETTIMEOFDAY=0 -DHAVE_LOCALTIME_R=0 -Dgettimeofday=missing_gettimeofday -Dlocaltime_r=missing_localtime_r" -DBUILD_TESTS=ON -DWARNINGS_AS_ERRORS=ON
make 2>&1 | head -15
cd .. || exit 1

echo
echo "=== Method 5: Testing Custom I/O Functions ==="
mkdir -p build-custom-io-test || exit 1
cd build-custom-io-test || exit 1
# Test with alternate platform that includes custom I/O functions
rm -rf CMakeCache.txt CMakeFiles/ cmake_install.cmake Makefile
cmake ../../.. -DCMAKE_C_FLAGS="-DLOGGER_PLATFORM_ALT" -DBUILD_TESTS=ON -DWARNINGS_AS_ERRORS=ON
make 2>&1 | head -15
cd .. || exit 1

echo
echo "=== Method 6: Testing Minimal I/O Environment ==="
mkdir -p build-minimal-io || exit 1
cd build-minimal-io || exit 1
# Test environment with no stdio, only custom I/O
rm -rf CMakeCache.txt CMakeFiles/ cmake_install.cmake Makefile
cmake ../../.. -DCMAKE_C_FLAGS="-DLOGGER_PLATFORM_ALT -DHAVE_STDIO_H=0" -DBUILD_TESTS=ON -DWARNINGS_AS_ERRORS=ON
make 2>&1 | head -15
cd .. || exit 1
