add_library(myutils STATIC
    myutils.c
)

target_include_directories(myutils PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)