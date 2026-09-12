add_executable(heroc main.cpp)
target_link_libraries(heroc PRIVATE hero_frontend hero_warnings)

# hero_warnings carries -Wall -Wextra, and -Werror when CI asks for it.