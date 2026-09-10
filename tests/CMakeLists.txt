include(FetchContent)

# Downloads and builds googletest as part of configuring.
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(googletest)

add_executable(hero_tests
  SanityTest.cpp
)

target_link_libraries(hero_tests PRIVATE GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(hero_tests)