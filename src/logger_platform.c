#include "logger_platform.h"
#include "platform_config.h"

// Enable GNU extensions only on Linux where they are needed
#if defined(PLATFORM_LINUX)
#define _GNU_SOURCE  // Enable GNU extensions for syscall, localtime_r
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include time.h first if available to avoid conflicts
#ifdef HAVE_TIME_H
#include <time.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifndef HAVE_TIME_H
// Fallback implementations when time.h is not available

// Basic time types
typedef long time_t;

// Basic tm structure with essential fields
struct tm {
    int tm_sec;   // seconds after the minute [0-60]
    int tm_min;   // minutes after the hour [0-59]
    int tm_hour;  // hours since midnight [0-23]
    int tm_mday;  // day of the month [1-31]
    int tm_mon;   // months since January [0-11]
    int tm_year;  // years since 1900
    int tm_wday;  // days since Sunday [0-6]
    int tm_yday;  // days since January 1 [0-365]
    int tm_isdst; // Daylight Saving Time flag
};

// Fallback implementation of localtime
static struct tm* localtime(const time_t* timer) {
    static struct tm tm_buf;
    // Use the timer parameter to avoid unused parameter warning
    (void)timer;
    // This is a very basic implementation - in a real scenario you'd need
    // platform-specific time conversion logic here
    // For now, we'll return a zero-initialized structure
    memset(&tm_buf, 0, sizeof(struct tm));
    return &tm_buf;
}

// Fallback implementation of strftime
static size_t strftime(char* str, size_t count, const char* format, const struct tm* timeptr) {
    // Use the parameters to avoid unused parameter warnings
    (void)format;
    (void)timeptr;

    if (count == 0) return 0;

    // Very basic implementation - just return a fixed string
    // In a real implementation, you'd parse the format string
    const char* fallback_time = "00-00-00 00:00:00";
    size_t len = strlen(fallback_time);

    if (len >= count) {
        return 0; // Not enough space
    }

    strcpy(str, fallback_time);
    return len;
}

#endif

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_SYSCALL_H
#include <sys/syscall.h>
#endif

/* Function declarations for functions that may not be declared by default */
#ifdef HAVE_LOCALTIME_R
extern struct tm *localtime_r(const time_t *timep, struct tm *result);
#endif

// Declare syscall if it's not declared by the headers
#if (defined(HAVE_SYS_SYSCALL_H) || defined(HAVE_UNISTD_H)) && !defined(__APPLE__)
extern long syscall(long number, ...);
#endif

// Provide default implementations only if alternate implementations are not requested
#if !defined(LOGGER_PLATFORM_ALT)

// Platform-specific mutex variable
#if defined(PLATFORM_WINDOWS)
#include <windows.h>
static CRITICAL_SECTION s_mutex;
#elif defined(ENABLE_THREADING) && defined(HAVE_PTHREAD_MUTEX_INIT)
static pthread_mutex_t s_mutex;
#else
/* No threading support - operations will be non-thread-safe */
static int s_no_threading_dummy;
#endif

static volatile int s_initialized = 0; // false

void logger_platform_init(void)
{
    if (s_initialized) {
        return;
    }
#if defined(PLATFORM_WINDOWS)
    InitializeCriticalSection(&s_mutex);
#elif defined(ENABLE_THREADING) && defined(HAVE_PTHREAD_MUTEX_INIT)
    pthread_mutex_init(&s_mutex, NULL);
#endif
    s_initialized = 1; // true
}

int logger_platform_is_initialized(void)
{
    return s_initialized;
}

void logger_platform_lock(void)
{
#if defined(PLATFORM_WINDOWS)
    EnterCriticalSection(&s_mutex);
#elif defined(ENABLE_THREADING) && defined(HAVE_PTHREAD_MUTEX_INIT)
    pthread_mutex_lock(&s_mutex);
#endif
}

void logger_platform_unlock(void)
{
#if defined(PLATFORM_WINDOWS)
    LeaveCriticalSection(&s_mutex);
#elif defined(ENABLE_THREADING) && defined(HAVE_PTHREAD_MUTEX_INIT)
    pthread_mutex_unlock(&s_mutex);
#endif
}

long logger_platform_get_current_thread_id(void)
{
#if defined(PLATFORM_WINDOWS)
    return GetCurrentThreadId();
#elif defined(PLATFORM_LINUX) && defined(HAVE_SYS_GETTID)
    return syscall(SYS_gettid);
#elif defined(PLATFORM_MACOS)
    return syscall(SYS_thread_selfid);
#elif defined(HAVE_PTHREAD_H)
    return (long) pthread_self();
#else
    return 0; /* No threading support */
#endif
}

#if defined(PLATFORM_WINDOWS)
// Windows-specific implementation of localtime_r
static struct tm* localtime_r(const time_t* timep, struct tm* result)
{
    localtime_s(result, timep);
    return result;
}
#endif

#ifdef HAVE_SYS_TIME_H
void logger_platform_get_timestamp(const struct timeval* time, char* timestamp, size_t size)
#else
void logger_platform_get_timestamp(const struct logger_timeval* time, char* timestamp, size_t size)
#endif
{
    time_t sec = time->tv_sec; // a necessary variable to avoid a runtime error on Windows
    struct tm calendar;

    assert(size >= 25);

#ifdef HAVE_LOCALTIME_R
    localtime_r(&sec, &calendar);
#else
    calendar = *localtime(&sec);
#endif
    strftime(timestamp, size, "%y-%m-%d %H:%M:%S", &calendar);
    sprintf(&timestamp[17], ".%06ld", (long) time->tv_usec);
}

#include <stdarg.h>

// Platform-specific I/O functions
int logger_platform_printf(FILE* stream, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vfprintf(stream, format, args);
    va_end(args);
    return result;
}

int logger_platform_vprintf(FILE* stream, const char* format, va_list args)
{
    return vfprintf(stream, format, args);
}

int logger_platform_flush(FILE* stream)
{
    return fflush(stream);
}

// Platform-specific file operations
FILE* logger_platform_fopen(const char* filename, const char* mode)
{
    return fopen(filename, mode);
}

int logger_platform_fclose(FILE* stream)
{
    return fclose(stream);
}

int logger_platform_fseek(FILE* stream, long offset, int whence)
{
    return fseek(stream, offset, whence);
}

long logger_platform_ftell(FILE* stream)
{
    return ftell(stream);
}

int logger_platform_remove(const char* filename)
{
    return remove(filename);
}

int logger_platform_rename(const char* old_filename, const char* new_filename)
{
    return rename(old_filename, new_filename);
}

// Platform-specific error reporting (for initialization-time errors)
int logger_platform_error(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int result = vfprintf(stderr, format, args);
    va_end(args);
    return result;
}

#endif /* !defined(LOGGER_PLATFORM_ALT) */
