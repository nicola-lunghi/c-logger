# PlatformChecks.cmake - Cross-platform feature detection for c-logger

# Include required CMake modules
include(CheckIncludeFile)
include(CheckFunctionExists)
include(CheckSymbolExists)

# Check for threading support
if(ENABLE_THREADING)
    check_include_file(pthread.h HAVE_PTHREAD_H)
    check_function_exists(pthread_mutex_init HAVE_PTHREAD_MUTEX_INIT)
endif()

# Check for time functions
check_include_file(time.h HAVE_TIME_H)
check_include_file(sys/time.h HAVE_SYS_TIME_H)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(localtime_r HAVE_LOCALTIME_R)

# Check for syscall (for thread IDs)
check_include_file(sys/syscall.h HAVE_SYS_SYSCALL_H)
if(HAVE_SYS_SYSCALL_H)
    check_symbol_exists(SYS_gettid sys/syscall.h HAVE_SYS_GETTID)
endif()

# Check for unistd.h
check_include_file(unistd.h HAVE_UNISTD_H)

# Platform detection
if(WIN32)
    set(PLATFORM_WINDOWS 1)
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX 1)
elseif(APPLE)
    set(PLATFORM_MACOS 1)
else()
    set(PLATFORM_UNKNOWN 1)
endif()

# Generate platform_config.h from template
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/platform_config.h)
