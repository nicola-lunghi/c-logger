cmake_minimum_required(VERSION 3.5)
project(logger C)

include(GNUInstallDirs)

# Build options
option(BUILD_TESTS "Build all of own tests" OFF)
option(BUILD_EXAMPLES "Build example programs" OFF)
option(BUILD_DOCS "Build doxygen documentation" OFF)
option(BUILD_SHARED "Build shared library" OFF)
option(ENABLE_THREADING "Enable thread safety" ON)
option(WARNINGS_AS_ERRORS "Treat all warnings as errors" OFF)

# Set default build type to Release
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Cross-platform feature detection
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformChecks.cmake)

# Compiler flags
set(CMAKE_C_FLAGS "-Wall -Wextra -std=c99")
if(WARNINGS_AS_ERRORS)
    message(STATUS "WARNINGS_AS_ERRORS=ON: Treating warnings as errors")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
endif()
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")

### Library
set(logger_sources
    src/logger.c
    src/logger.h
    src/logger_platform.c
    src/logger_platform.h
    src/loggerconf.c
    src/loggerconf.h
    include/logger_config.h
)
if(BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED ${logger_sources})
else()
    add_library(${PROJECT_NAME} STATIC ${logger_sources})
endif()

# Export the include directory
target_include_directories(
    ${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

### Install
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
file(GLOB header_files src/*.h)
install(FILES ${header_files} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

### Document
if(BUILD_DOCS)
    add_custom_target(doc COMMAND "doxygen" "${PROJECT_SOURCE_DIR}/doc/Doxyfile")
endif()

### Test
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

### Example
if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
