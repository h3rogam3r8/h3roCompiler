add_executable(heroc main.cpp)
target_compile_options(heroc PRIVATE -Wall -Wextra)

# -Wall -Wextra turns on warnings. Catches bugs that you don't usually see.