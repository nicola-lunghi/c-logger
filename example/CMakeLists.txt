set(examples
    logger_console_example
    logger_file_example
    logger_multi_example
    loggerconf_example
)
foreach(example IN LISTS examples)
    add_executable(${example} ${example}.c)
    target_include_directories(${example} PRIVATE ${PROJECT_SOURCE_DIR}/src)
    target_link_libraries(${example} ${PROJECT_NAME})
endforeach()
