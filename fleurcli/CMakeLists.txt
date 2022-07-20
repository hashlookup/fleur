add_executable(fleurcli
fleurcli.c
)

target_link_libraries(fleurcli
fnv
fleur
myutils
)