#ifndef CUSTOM_CONFIG_H
#define CUSTOM_CONFIG_H

// Test with a custom config.h that simulates missing headers
#define HAVE_SYS_TIME_H 0
#define HAVE_GETTIMEOFDAY 0
#define HAVE_LOCALTIME_R 0
#define HAVE_PTHREAD_H 0
#define HAVE_PTHREAD_MUTEX_INIT 0
#define ENABLE_THREADING 0
#define HAVE_UNISTD_H 0
#define HAVE_SYS_SYSCALL_H 0
#define HAVE_SYS_GETTID 0

#endif /* CUSTOM_CONFIG_H */
