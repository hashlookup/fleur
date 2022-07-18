add_executable(suite_2_app
test_bloom.c
)

target_link_libraries(suite_2_app
fnv
bloom
Unity
)

add_test(suite_2_test suite_2_app)