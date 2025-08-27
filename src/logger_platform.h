#ifndef LOGGER_PLATFORM_H
#define LOGGER_PLATFORM_H

#include "platform_config.h"
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

// Include system headers only when available
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

// Include user alternate implementation if requested
#if defined(LOGGER_PLATFORM_ALT)
#include "logger_platform_alt.h"
#endif

// Platform-specific threading functions
void logger_platform_init(void);
void logger_platform_lock(void);
void logger_platform_unlock(void);
long logger_platform_get_current_thread_id(void);

// Platform-specific time functions
#ifdef HAVE_SYS_TIME_H
// Use system struct timeval when available
void logger_platform_get_timestamp(const struct timeval* time, char* timestamp, size_t size);
#else
// Fallback definition for platforms without sys/time.h
struct logger_timeval {
    long tv_sec;    // seconds
    long tv_usec;   // microseconds
};
void logger_platform_get_timestamp(const struct logger_timeval* time, char* timestamp, size_t size);
#endif

// Platform-specific I/O functions
int logger_platform_printf(FILE* stream, const char* format, ...);
int logger_platform_vprintf(FILE* stream, const char* format, va_list args);
int logger_platform_flush(FILE* stream);

// Platform-specific file operations
FILE* logger_platform_fopen(const char* filename, const char* mode);
int logger_platform_fclose(FILE* stream);
int logger_platform_fseek(FILE* stream, long offset, int whence);
long logger_platform_ftell(FILE* stream);
int logger_platform_remove(const char* filename);
int logger_platform_rename(const char* old_filename, const char* new_filename);

// Platform-specific error reporting (for initialization-time errors)
int logger_platform_error(const char* format, ...);

// Platform-specific initialization check
int logger_platform_is_initialized(void);

#endif // LOGGER_PLATFORM_H
