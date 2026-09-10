#include "hero/Lexer.h"

namespace hero {

const char *tokenKindName(TokenKind kind) {
  switch (kind){
  case TokenKind::Eof:          return "eof";
  case TokenKind::Identifier:   return "identifier";
  case TokenKind::IntLiteral:   return "int literal";
  case TokenKind::FloatLiteral: return "float literal";
  case TokenKind::KwFn:         return "fn";
  case TokenKind::KwLet:        return "let";
  case TokenKind::KwCast:       return "cast";
  case TokenKind::KwTrue:       return "true";
  case TokenKind::KwFalse:      return "false";
  case TokenKind::KwTensor:     return "tensor";
  case TokenKind::KwF32:        return "f32";
  case TokenKind::KwF16:        return "f16";
  case TokenKind::KwBf16:       return "bf16";
  case TokenKind::KwI32:        return "i32";
  case TokenKind::KwI8:         return "i8";
  case TokenKind::KwBool:       return "bool";
  case TokenKind::Plus:         return "+";
  case TokenKind::Minus:        return "-";
  case TokenKind::Star:         return "*";
  case TokenKind::Slash:        return "/";
  case TokenKind::Equals:       return "=";
  case TokenKind::Semicolon:    return ";";
  case TokenKind::Comma:        return ",";
  case TokenKind::Colon:        return ":";
  case TokenKind::Arrow:        return "->";
  case TokenKind::LParen:       return "(";
  case TokenKind::RParen:       return ")";
  case TokenKind::LBrace:       return "{";
  case TokenKind::RBrace:       return "}";
  case TokenKind::LBracket:     return "[";
  case TokenKind::RBracket:     return "]";
  case TokenKind::Less:         return "<";
  case TokenKind::Greater:      return ">";
  case TokenKind::Unknown:      return "unknown";
  }
  return "unknown";
}

Lexer::Lexer(std::string_view source) : source_(source) {}

Token Lexer::next() {
  // Not written yet. Returning a default Token means every test fails,
  // which is what I want committed before I start this whole process.
  return Token{};
}

std::vector<Token> Lexer::tokenize() {
  return {};
}

}  // namespace hero