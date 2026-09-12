# C++17 rather than 20 because that's what LLVM itself is built with

cmake_minimum_required(VERSION 3.20)
project(hero LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Writes build/compile_commands.json, which is what editors and clang-tidy
# read to know the exact flags each file is compiled with.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(HERO_BUILD_TESTS "Build the unit tests" ON)
option(HERO_WERROR "Turn warnings into errors" OFF)

# One place for the warning flags instead of repeating them in three
# CMakeLists. INTERFACE means it has no sources of its own, it just hands
# flags to whatever links it. Only applies to my targets, so googletest
# can warn all it likes without failing the build.
add_library(hero_warnings INTERFACE)
target_compile_options(hero_warnings INTERFACE -Wall -Wextra)
if(HERO_WERROR)
  target_compile_options(hero_warnings INTERFACE -Werror)
endif()

add_subdirectory(src)
add_subdirectory(tools/heroc)

if(HERO_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()