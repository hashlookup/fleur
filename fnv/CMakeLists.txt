add_library(fnv STATIC
    fnv.c
)

target_include_directories(fnv PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)