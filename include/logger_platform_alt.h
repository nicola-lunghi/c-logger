#ifndef LOGGER_PLATFORM_ALT_H
#define LOGGER_PLATFORM_ALT_H

/**
 * \file logger_platform_alt.h
 *
 * \brief Example alternate platform implementation for the logger library
 *
 * This file demonstrates how to provide custom platform-specific implementations
 * for the logger library by defining LOGGER_PLATFORM_ALT in logger_config.h.
 *
 * When LOGGER_PLATFORM_ALT is defined, the logger library will use the functions
 * declared in this file instead of the default implementations.
 *
 * To use this alternate implementation:
 * 1. Copy this file to your project
 * 2. Modify the implementations as needed for your platform
 * 3. Define LOGGER_PLATFORM_ALT in your logger_config.h
 * 4. Include this header in your build
 */

#include <stddef.h>

// Example custom time structure for demonstration
struct logger_timeval {
    long tv_sec;    // seconds
    long tv_usec;   // microseconds
};

// Alternate platform functions - implement these for your platform

/**
 * \brief Initialize platform-specific resources
 *
 * This function is called once to initialize any platform-specific
 * resources needed by the logger (e.g., mutexes, timers, etc.)
 */
void logger_platform_init(void);

/**
 * \brief Acquire the logger mutex/lock
 *
 * This function should block until the logger lock is acquired.
 * It's used to make logging operations thread-safe.
 */
void logger_platform_lock(void);

/**
 * \brief Release the logger mutex/lock
 *
 * This function should release the logger lock previously acquired
 * by logger_platform_lock().
 */
void logger_platform_unlock(void);

/**
 * \brief Get the current thread ID
 *
 * \return A unique identifier for the current thread, or 0 if
 *         threading is not supported or available.
 */
long logger_platform_get_current_thread_id(void);

/**
 * \brief Format a timestamp from a time structure
 *
 * \param time     Pointer to a time structure (struct timeval or logger_timeval)
 * \param timestamp Buffer to store the formatted timestamp string
 * \param size     Size of the timestamp buffer (must be at least 25 characters)
 *
 * The timestamp should be formatted as: "yy-mm-dd hh:mm:ss.microseconds"
 */
void logger_platform_get_timestamp(const struct logger_timeval* time, char* timestamp, size_t size);

/**
 * \brief Check if the platform layer has been initialized
 *
 * \return Non-zero if initialized, 0 if not initialized
 */
int logger_platform_is_initialized(void);

/**
 * \brief Platform-specific printf function
 *
 * This function should handle formatted output to the specified stream.
 * It's used for all logging output operations.
 *
 * \param stream The output stream (FILE*)
 * \param format Format string
 * \param ... Additional arguments
 * \return Number of characters written, or negative value on error
 */
int logger_platform_printf(FILE* stream, const char* format, ...);

/**
 * \brief Platform-specific vprintf function
 *
 * This function should handle formatted output to the specified stream using va_list.
 * It's used for all logging output operations with variable arguments.
 *
 * \param stream The output stream (FILE*)
 * \param format Format string
 * \param args Variable argument list
 * \return Number of characters written, or negative value on error
 */
int logger_platform_vprintf(FILE* stream, const char* format, va_list args);

/**
 * \brief Platform-specific flush function
 *
 * This function should flush the specified stream.
 * It's used to ensure all buffered output is written.
 *
 * \param stream The output stream (FILE*)
 * \return 0 on success, EOF on error
 */
int logger_platform_flush(FILE* stream);

// Example implementations (replace with your platform-specific code)

/*
void logger_platform_init(void)
{
    // Initialize your platform-specific resources here
    // For example: custom_mutex_init(&logger_mutex);
    printf("Custom platform init called\n");
}

void logger_platform_lock(void)
{
    // Acquire your platform-specific lock
    // For example: custom_mutex_lock(&logger_mutex);
    printf("Custom platform lock called\n");
}

void logger_platform_unlock(void)
{
    // Release your platform-specific lock
    // For example: custom_mutex_unlock(&logger_mutex);
    printf("Custom platform unlock called\n");
}

long logger_platform_get_current_thread_id(void)
{
    // Return your platform-specific thread ID
    // For example: return custom_get_thread_id();
    return 12345; // Example thread ID
}

void logger_platform_get_timestamp(const struct logger_timeval* time, char* timestamp, size_t size)
{
    // Format timestamp using your platform-specific time functions
    // For example: custom_format_time(time, timestamp, size);
    snprintf(timestamp, size, "yy-mm-dd hh:mm:ss.%06ld", time->tv_usec);
}

int logger_platform_is_initialized(void)
{
    // Return whether your platform is initialized
    // For example: return custom_is_initialized();
    return 1; // Always initialized in this example
}

int logger_platform_printf(FILE* stream, const char* format, ...)
{
    // Handle formatted output using your platform-specific functions
    // For example: return custom_printf(stream, format, ...);
    va_list args;
    va_start(args, format);
    int result = vfprintf(stream, format, args);
    va_end(args);
    return result;
}

int logger_platform_vprintf(FILE* stream, const char* format, va_list args)
{
    // Handle formatted output with va_list using your platform-specific functions
    // For example: return custom_vprintf(stream, format, args);
    return vfprintf(stream, format, args);
}

int logger_platform_flush(FILE* stream)
{
    // Flush the stream using your platform-specific functions
    // For example: return custom_flush(stream);
    return fflush(stream);
}
*/

#endif /* LOGGER_PLATFORM_ALT_H */
