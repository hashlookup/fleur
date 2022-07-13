add_executable(suite_1_app
test_vectors.c
)

target_link_libraries(suite_1_app
fnv
Unity
)

add_test(suite_1_test suite_1_app)