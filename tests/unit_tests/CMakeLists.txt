include(GoogleTest)

add_executable(device_unit_test device_unit_test.cpp)

target_link_libraries(
  device_unit_test PRIVATE gtest gtest_main mqss_client)
gtest_discover_tests(device_unit_test)


add_executable(job_unit_test job_unit_test.cpp)

target_link_libraries(
  job_unit_test PRIVATE gtest gtest_main mqss_client)
gtest_discover_tests(job_unit_test)


if(ENABLE_COVERAGE)
  target_compile_options(device_unit_test INTERFACE --coverage -fprofile-arcs
                                                   -ftest-coverage -O0)
  target_link_libraries(device_unit_test INTERFACE gcov --coverage)

  target_compile_options(job_unit_test INTERFACE --coverage -fprofile-arcs
                                                   -ftest-coverage -O0)
  target_link_libraries(job_unit_test INTERFACE gcov --coverage)
endif()
