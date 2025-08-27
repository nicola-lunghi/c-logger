#ifndef LOGGER_CONFIG_H
#define LOGGER_CONFIG_H

/**
 * \file logger_config.h
 *
 * \brief Configuration options for the logger library
 *
 * This file contains compile-time configuration options that allow users
 * to customize the logger library behavior, including providing alternate
 * implementations for platform-specific functions.
 */

/* Platform abstraction layer alternate implementations */

/**
 * \def LOGGER_PLATFORM_ALT
 *
 * Uncomment to provide your own alternate implementation for platform functions:
 * - logger_platform_init()
 * - logger_platform_lock()
 * - logger_platform_unlock()
 * - logger_platform_get_current_thread_id()
 * - logger_platform_get_timestamp()
 * - logger_platform_is_initialized()
 *
 * When enabled, you must provide a header "logger_platform_alt.h" with
 * declarations and implementations of these functions.
 *
 * This allows you to provide custom platform-specific implementations
 * for environments that are not supported by the default implementation.
 */
//#define LOGGER_PLATFORM_ALT

/**
 * \def LOGGER_THREADING_ALT
 *
 * Uncomment to provide your own alternate implementation for threading functions:
 * - logger_platform_lock()
 * - logger_platform_unlock()
 *
 * When enabled, you must provide implementations for these functions
 * in your alternate platform header.
 *
 * This is useful for platforms with custom threading models or
 * when you want to integrate with existing threading libraries.
 */
//#define LOGGER_THREADING_ALT

/**
 * \def LOGGER_TIMING_ALT
 *
 * Uncomment to provide your own alternate implementation for timing functions:
 * - logger_platform_get_timestamp()
 *
 * When enabled, you must provide an implementation for this function
 * in your alternate platform header.
 *
 * This is useful for platforms with custom time sources or
 * high-precision timing requirements.
 */
//#define LOGGER_TIMING_ALT

#endif /* LOGGER_CONFIG_H */
