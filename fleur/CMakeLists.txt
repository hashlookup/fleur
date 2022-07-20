add_library(fleur STATIC
    fleur.c
)

target_link_libraries(fleur
fnv
myutils
m)

target_include_directories(fleur PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)