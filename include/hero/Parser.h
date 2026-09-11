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
  // Token helpers.
  const Token &peek(size_t ahead = 0) const;
  const Token &advance();
  bool check(TokenKind kind) const;
  bool match(TokenKind kind);
  bool expect(TokenKind kind, const char *what);
  void error(const Token &tok, const std::string &message);

  // One function per grammar rule, same names as the spec uses.
  bool parseFunction(Function &out);
  bool parseParam(Param &out);
  bool parseType(Type &out);
  bool parseBlock(Block &out);

  ExprPtr parseExpr();
  ExprPtr parseAdd();
  ExprPtr parseMul();
  ExprPtr parseUnary();
  ExprPtr parsePrimary();

  // Lexing the whole file up front instead of pulling one token at a time.
  std::vector<Token> tokens_;
  size_t index_ = 0;
  std::vector<std::string> errors_;
};

}  // namespace hero

#endif  // HERO_PARSER_H