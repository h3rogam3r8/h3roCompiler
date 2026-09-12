#ifndef HERO_LEXER_H
#define HERO_LEXER_H

#include "hero/SourceFile.h"
#include <string>
#include <string_view>
#include <vector>
#include <cstddef>

namespace hero {

enum class TokenKind {
  // No more input. tokenize() always ends with one of these.
  Eof,

  // These carry text that matters.
  Identifier,
  IntLiteral,
  FloatLiteral,

  // Keywords.
  KwFn,
  KwLet,
  KwCast,
  KwTrue,
  KwFalse,
  KwTensor,

  // Dtype names. Keywords too, just grouped separately because the
  // parser will treat them as a set.
  KwF32,
  KwF16,
  KwBf16,
  KwI32,
  KwI8,
  KwBool,

  // Operators and punctuation.
  Plus,
  Minus,
  Star,
  Slash,
  Equals,
  Semicolon,
  Comma,
  Colon,
  Arrow,
  LParen,
  RParen,
  LBrace,
  RBrace,
  LBracket,
  RBracket,
  Less,
  Greater,

  // A character the lexer couldn't make sense of. Keeping it as a token
  // instead of stopping means one bad character doesn't kill the rest of
  // the file. Helps for error handling.
  Unknown,
};

struct Token {
  TokenKind kind = TokenKind::Eof;
  std::string text;  // exactly the characters this token covers
  SourceLoc loc;
};

// Name of a token kind, for error messages and test output.
const char *tokenKindName(TokenKind kind);

class Lexer {
public:
  explicit Lexer(std::string_view source);

  // The next token. Keeps returning Eof once the input runs out.
  Token next();
  std::vector<Token> tokenize();

private:
  bool atEnd() const;
  char peek(size_t ahead = 0) const;

  // Consume one character, keeping line and column up to date.
  char advance();

  // Consume the next character only if it's the one expected.
  bool match(char expected);

  void skipWhitespaceAndComments();

  Token makeToken(TokenKind kind, SourceLoc start, size_t startPos) const;
  Token lexWord(SourceLoc start, size_t startPos);
  Token lexNumber(SourceLoc start, size_t startPos);

  std::string_view source_;
  size_t pos_ = 0;
  SourceLoc loc_;
};

}  // namespace hero

#endif  // HERO_LEXER_H