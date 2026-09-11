add_executable(heroc main.cpp)
target_link_libraries(heroc PRIVATE hero_frontend)
target_compile_options(heroc PRIVATE -Wall -Wextra)

# -Wall -Wextra turns on warnings. Catches bugs that you don't usually see.