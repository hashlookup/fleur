add_executable(suite_2_app
test_fleur.c
)

target_link_libraries(suite_2_app
fnv
fleur
Unity
)

add_test(suite_2_test suite_2_app)