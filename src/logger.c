#include "logger.h"
#include "platform_config.h"
#include "logger_platform.h"
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

enum {
    /* Logger type */
    kConsoleLogger = 1 << 0,
    kFileLogger = 1 << 1,

    kMaxFileNameLen = 255, /* without null character */
    kDefaultMaxFileSize = 1048576L, /* 1 MB */
};

/* Console logger */
static struct {
    FILE* output;
    unsigned long long flushedTime;
} s_clog;

/* File logger */
static struct {
    FILE* output;
    char filename[kMaxFileNameLen + 1];
    long maxFileSize;
    unsigned char maxBackupFiles;
    long currentFileSize;
    unsigned long long flushedTime;
} s_flog;

static volatile int s_logger;
static volatile LogLevel s_logLevel = LogLevel_INFO;
static volatile long s_flushInterval = 0; /* msec, 0 is auto flush off */

static void init(void)
{
    logger_platform_init();
}

static void lock(void)
{
    logger_platform_lock();
}

static void unlock(void)
{
    logger_platform_unlock();
}

#if defined(PLATFORM_WINDOWS)
static int gettimeofday(struct timeval* tv, void* tz)
{
    const UINT64 epochFileTime = 116444736000000000ULL;
    FILETIME ft;
    ULARGE_INTEGER li;
    UINT64 t;

    if (tv == NULL) {
        return -1;
    }
    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    t = (li.QuadPart - epochFileTime) / 10;
    tv->tv_sec = (long) (t / 1000000);
    tv->tv_usec = t % 1000000;
    return 0;
}
#endif /* PLATFORM_WINDOWS */

int logger_initConsoleLogger(FILE* output)
{
    output = (output != NULL) ? output : stdout;
    if (output != stdout && output != stderr) {
        assert(0 && "output must be stdout or stderr");
        return 0;
    }

    init();
    lock();
    s_clog.output = output;
    s_logger |= kConsoleLogger;
    unlock();
    return 1;
}

static long getFileSize(const char* filename)
{
    FILE* fp;
    long size;

    if ((fp = logger_platform_fopen(filename, "rb")) == NULL) {
        return 0;
    }
    logger_platform_fseek(fp, 0, SEEK_END);
    size = logger_platform_ftell(fp);
    logger_platform_fclose(fp);
    return size;
}

int logger_initFileLogger(const char* filename, long maxFileSize, unsigned char maxBackupFiles)
{
    int ok = 0; /* false */

    if (filename == NULL) {
        assert(0 && "filename must not be NULL");
        return 0;
    }
    if (strlen(filename) > kMaxFileNameLen) {
        assert(0 && "filename exceeds the maximum number of characters");
        return 0;
    }

    init();
    lock();
    if (s_flog.output != NULL) { /* reinit */
        logger_platform_fclose(s_flog.output);
    }
    s_flog.output = logger_platform_fopen(filename, "a");
    if (s_flog.output == NULL) {
        logger_platform_error("ERROR: logger: Failed to open file: `%s`\n", filename);
        goto cleanup;
    }
    s_flog.currentFileSize = getFileSize(filename);
    strncpy(s_flog.filename, filename, sizeof(s_flog.filename) - 1);
    s_flog.filename[sizeof(s_flog.filename) - 1] = '\0'; /* Ensure null termination */
    s_flog.maxFileSize = (maxFileSize > 0) ? maxFileSize : kDefaultMaxFileSize;
    s_flog.maxBackupFiles = maxBackupFiles;
    s_logger |= kFileLogger;
    ok = 1; /* true */
cleanup:
    unlock();
    return ok;
}

void logger_setLevel(LogLevel level)
{
    s_logLevel = level;
}

LogLevel logger_getLevel(void)
{
    return s_logLevel;
}

int logger_isEnabled(LogLevel level)
{
    return s_logLevel <= level;
}

void logger_autoFlush(long interval)
{
    s_flushInterval = interval > 0 ? interval : 0;
}

static int hasFlag(int flags, int flag)
{
    return (flags & flag) == flag;
}

void logger_flush()
{
    if (s_logger == 0 || !logger_platform_is_initialized()) {
        assert(0 && "logger is not initialized");
        return;
    }

    if (hasFlag(s_logger, kConsoleLogger)) {
        logger_platform_flush(s_clog.output);
    }
    if (hasFlag(s_logger, kFileLogger)) {
        logger_platform_flush(s_flog.output);
    }
}

static char getLevelChar(LogLevel level)
{
    switch (level) {
        case LogLevel_TRACE: return 'T';
        case LogLevel_DEBUG: return 'D';
        case LogLevel_INFO:  return 'I';
        case LogLevel_WARN:  return 'W';
        case LogLevel_ERROR: return 'E';
        case LogLevel_FATAL: return 'F';
        default: return ' ';
    }
}

static void getBackupFileName(const char* basename, unsigned char index,
        char* backupname, size_t size)
{
    char indexname[5];

    assert(size >= strlen(basename) + sizeof(indexname));

    strncpy(backupname, basename, size);
    if (index > 0) {
        sprintf(indexname, ".%d", index);
        /* Calculate available space: total size - current length - 1 for null terminator */
        size_t available_space = size - strlen(backupname) - 1;
        strncat(backupname, indexname, available_space);
    }
}

static int isFileExist(const char* filename)
{
    FILE* fp;

    if ((fp = logger_platform_fopen(filename, "r")) == NULL) {
        return 0;
    } else {
        logger_platform_fclose(fp);
        return 1;
    }
}

static int rotateLogFiles(void)
{
    int i;
    /* backup filename: <filename>.xxx (xxx: 1-255) */
    char src[kMaxFileNameLen + 5], dst[kMaxFileNameLen + 5]; /* with null character */

    if (s_flog.currentFileSize < s_flog.maxFileSize) {
        return s_flog.output != NULL;
    }
    logger_platform_fclose(s_flog.output);
    for (i = (int) s_flog.maxBackupFiles; i > 0; i--) {
        getBackupFileName(s_flog.filename, i - 1, src, sizeof(src));
        getBackupFileName(s_flog.filename, i, dst, sizeof(dst));
        if (isFileExist(dst)) {
            if (logger_platform_remove(dst) != 0) {
                logger_platform_error("ERROR: logger: Failed to remove file: `%s`\n", dst);
            }
        }
        if (isFileExist(src)) {
            if (logger_platform_rename(src, dst) != 0) {
                logger_platform_error("ERROR: logger: Failed to rename file: `%s` -> `%s`\n", src, dst);
            }
        }
    }
    s_flog.output = logger_platform_fopen(s_flog.filename, "a");
    if (s_flog.output == NULL) {
        logger_platform_error("ERROR: logger: Failed to open file: `%s`\n", s_flog.filename);
        return 0;
    }
    s_flog.currentFileSize = getFileSize(s_flog.filename);
    return 1;
}

static long vflog(FILE* fp, char levelc, const char* timestamp, long threadID,
        const char* file, int line, const char* fmt, va_list arg,
        unsigned long long currentTime, unsigned long long* flushedTime)
{
    int size;
    long totalsize = 0;

    if ((size = logger_platform_printf(fp, "%c %s %ld %s:%d: ", levelc, timestamp, threadID, file, line)) > 0) {
        totalsize += size;
    }
    if ((size = logger_platform_vprintf(fp, fmt, arg)) > 0) {
        totalsize += size;
    }
    if ((size = logger_platform_printf(fp, "\n")) > 0) {
        totalsize += size;
    }
    if (s_flushInterval > 0) {
        if (currentTime - *flushedTime > (unsigned long long)s_flushInterval) {
            logger_platform_flush(fp);
            *flushedTime = currentTime;
        }
    }
    return totalsize;
}

void logger_log(LogLevel level, const char* file, int line, const char* fmt, ...)
{
    struct timeval now;
    unsigned long long currentTime; /* milliseconds */
    char levelc;
    char timestamp[32];
    long threadID;
    va_list carg, farg;

    if (s_logger == 0 || !logger_platform_is_initialized()) {
        assert(0 && "logger is not initialized");
        return;
    }

    if (!logger_isEnabled(level)) {
        return;
    }
#ifdef HAVE_GETTIMEOFDAY
    gettimeofday(&now, NULL);
#else
    {
        time_t t = time(NULL);
        now.tv_sec = t;
        now.tv_usec = 0;
    }
#endif
    currentTime = now.tv_sec * 1000 + now.tv_usec / 1000;
    levelc = getLevelChar(level);
#ifdef HAVE_SYS_TIME_H
    logger_platform_get_timestamp(&now, timestamp, sizeof(timestamp));
#else
    {
        struct logger_timeval lt = {now.tv_sec, now.tv_usec};
        logger_platform_get_timestamp(&lt, timestamp, sizeof(timestamp));
    }
#endif
    threadID = logger_platform_get_current_thread_id();
    lock();
    if (hasFlag(s_logger, kConsoleLogger)) {
        va_start(carg, fmt);
        vflog(s_clog.output, levelc, timestamp, threadID,
                file, line, fmt, carg, currentTime, &s_clog.flushedTime);
        va_end(carg);
    }
    if (hasFlag(s_logger, kFileLogger)) {
        if (rotateLogFiles()) {
            va_start(farg, fmt);
            s_flog.currentFileSize += vflog(s_flog.output, levelc, timestamp, threadID,
                    file, line, fmt, farg, currentTime, &s_flog.flushedTime);
            va_end(farg);
        }
    }
    unlock();
}

void logger_exitFileLogger()
{
    if (s_flog.output)
        logger_platform_fclose(s_flog.output);
}