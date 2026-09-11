#ifndef HERO_PARSER_H
#define HERO_PARSER_H

#include "hero/AST.h"
#include "hero/Lexer.h"

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace hero {

class Parser {
public:
  explicit Parser(std::string_view source);

  // Null if it didn't parse. errors() says why.
  std::unique_ptr<Program> parse();

  const std::vector<std::string> &errors() const { return errors_; }

private:
  // Lexing the whole file up front instead of pulling one token at a
  // time. Wastes memory on big files but lookahead becomes an index and
  // Hero files are tiny.
  std::vector<Token> tokens_;
  size_t index_ = 0;
  std::vector<std::string> errors_;
};

}  // namespace hero

#endif  // HERO_PARSER_H