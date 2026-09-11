add_library(hero_frontend
  Lexer.cpp
  Parser.cpp
)

target_include_directories(hero_frontend
  PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(hero_frontend PRIVATE -Wall -Wextra)