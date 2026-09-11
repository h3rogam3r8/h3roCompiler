#include "hero/Parser.h"

namespace hero {

Parser::Parser(std::string_view source) {
  tokens_ = Lexer(source).tokenize();
}

std::unique_ptr<Program> Parser::parse() {
  // Not written yet.
  return nullptr;
}

}  // namespace hero