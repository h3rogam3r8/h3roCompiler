add_library(hero_frontend
  Lexer.cpp
)

# PUBLIC avoids redundancy for header inclusions. PRIVATE makes it my problem.
target_include_directories(hero_frontend
  PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(hero_frontend PRIVATE -Wall -Wextra)