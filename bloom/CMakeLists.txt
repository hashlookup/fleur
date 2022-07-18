add_library(bloom STATIC
    bloom.c
)

target_include_directories(bloom PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)