add_executable(test test.cpp)
target_link_libraries(test mqss_client)
add_test(NAME test COMMAND test)