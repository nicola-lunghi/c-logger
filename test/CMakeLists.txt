cmake_minimum_required(VERSION 3.5)
project(logger_tests C)

set(tests
    logger_console_test
    logger_file_test
    logger_loglevel_test
    logger_multi_test
    loggerconf_test
)
set(test_libraries
    logger
)
foreach(test IN LISTS tests)
    add_executable(${test} ${test}.c)
    target_include_directories(${test} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    target_link_libraries(${test} ${test_libraries})
    add_test(NAME ${test}
             COMMAND ${test}
             WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endforeach()