#include "hero/Lexer.h"

#include <cctype>
#include <iomanip>
#include <ostream>
#include <unordered_map>
#include <utility>

namespace hero {
namespace {

// Every word gets read to the end and then looked up here. Checking
// prefixes character by character would turn "fnord" into the keyword fn
// followed by an identifier "ord".
const std::unordered_map<std::string_view, TokenKind> &keywordTable() {
  static const std::unordered_map<std::string_view, TokenKind> table = {
      {"fn", TokenKind::KwFn},
      {"let", TokenKind::KwLet},
      {"cast", TokenKind::KwCast},
      {"true", TokenKind::KwTrue},
      {"false", TokenKind::KwFalse},
      {"tensor", TokenKind::KwTensor},
      {"f32", TokenKind::KwF32},
      {"f16", TokenKind::KwF16},
      {"bf16", TokenKind::KwBf16},
      {"i32", TokenKind::KwI32},
      {"i8", TokenKind::KwI8},
      {"bool", TokenKind::KwBool},
  };
  return table;
}

// The cast is the standard fix.
bool isWordStart(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool isWordPart(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

bool isDigit(char c) { return c >= '0' && c <= '9'; }

}  // namespace

const char *tokenKindName(TokenKind kind) {
  switch (kind) {
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

bool Lexer::atEnd() const { return pos_ >= source_.size(); }

char Lexer::peek(size_t ahead) const {
  const size_t index = pos_ + ahead;
  return index < source_.size() ? source_[index] : '\0';
}

char Lexer::advance() {
  const char c = source_[pos_++];
  if (c == '\n') {
    loc_.line += 1;
    loc_.column = 1;
  } else {
    loc_.column += 1;
  }
  return c;
}

bool Lexer::match(char expected) {
  if (peek() != expected)
    return false;
  advance();
  return true;
}

// An unterminated block comment currently just runs to the end of the file
// without complaining. 
void Lexer::skipWhitespaceAndComments() {
  while (!atEnd()) {
    const char c = peek();

    if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
      advance();
      continue;
    }

    if (c == '/' && peek(1) == '/') {
      while (!atEnd() && peek() != '\n')
        advance();
      continue;
    }

    if (c == '/' && peek(1) == '*') {
      advance();  // '/'
      advance();  // '*'
      // The spec says block comments don't nest, so the first */ closes it.
      while (!atEnd() && !(peek() == '*' && peek(1) == '/'))
        advance();
      if (!atEnd()) {
        advance();  // '*'
        advance();  // '/'
      }
      continue;
    }

    // Not whitespace and not a comment, so a real token starts here.
    return;
  }
}

Token Lexer::makeToken(TokenKind kind, SourceLoc start,
                       size_t startPos) const {
  return Token{kind, std::string(source_.substr(startPos, pos_ - startPos)),
               start};
}

Token Lexer::lexWord(SourceLoc start, size_t startPos) {
  while (!atEnd() && isWordPart(peek()))
    advance();

  const std::string_view word = source_.substr(startPos, pos_ - startPos);

  const auto &table = keywordTable();
  const auto it = table.find(word);
  const TokenKind kind =
      it == table.end() ? TokenKind::Identifier : it->second;

  return makeToken(kind, start, startPos);
}

Token Lexer::lexNumber(SourceLoc start, size_t startPos) {
  bool isFloat = false;

  while (!atEnd() && isDigit(peek()))
    advance();

  // A dot only belongs to the number if a digit follows it. Otherwise "1."
  // would swallow the dot and leave something that isn't a number.
  if (peek() == '.' && isDigit(peek(1))) {
    isFloat = true;
    advance();  // '.'
    while (!atEnd() && isDigit(peek()))
      advance();
  }

  // Exponent is 'e' or 'E', an optional sign, then at least one digit. The
  // lookahead has to check all three before consuming anything, otherwise
  // "1e" eats the e and produces a broken float.
  if (peek() == 'e' || peek() == 'E') {
    const size_t signOffset = (peek(1) == '+' || peek(1) == '-') ? 1 : 0;
    if (isDigit(peek(1 + signOffset))) {
      isFloat = true;
      advance();  // 'e'
      if (signOffset == 1)
        advance();  // sign
      while (!atEnd() && isDigit(peek()))
        advance();
    }
  }

  return makeToken(
      isFloat ? TokenKind::FloatLiteral : TokenKind::IntLiteral, start,
      startPos);
}

Token Lexer::next() {
  skipWhitespaceAndComments();

  // Recorded before consuming anything, so the token reports where it
  // starts rather than where it ends.
  const SourceLoc start = loc_;
  const size_t startPos = pos_;

  if (atEnd())
    return Token{TokenKind::Eof, "", start};

  const char c = advance();

  if (isWordStart(c))
    return lexWord(start, startPos);

  if (isDigit(c))
    return lexNumber(start, startPos);

  TokenKind kind = TokenKind::Unknown;
  switch (c) {
  case '+': kind = TokenKind::Plus;      break;
  case '*': kind = TokenKind::Star;      break;
  case '/': kind = TokenKind::Slash;     break;
  case '=': kind = TokenKind::Equals;    break;
  case ';': kind = TokenKind::Semicolon; break;
  case ',': kind = TokenKind::Comma;     break;
  case ':': kind = TokenKind::Colon;     break;
  case '(': kind = TokenKind::LParen;    break;
  case ')': kind = TokenKind::RParen;    break;
  case '{': kind = TokenKind::LBrace;    break;
  case '}': kind = TokenKind::RBrace;    break;
  case '[': kind = TokenKind::LBracket;  break;
  case ']': kind = TokenKind::RBracket;  break;
  case '<': kind = TokenKind::Less;      break;
  case '>': kind = TokenKind::Greater;   break;
  case '-':
    // Has to be checked here. Falling through to a plain minus would make
    // "->" lex as minus followed by greater.
    kind = match('>') ? TokenKind::Arrow : TokenKind::Minus;
    break;
  default:
    kind = TokenKind::Unknown;
    break;
  }

  return makeToken(kind, start, startPos);
}

std::vector<Token> Lexer::tokenize() {
  std::vector<Token> tokens;
  for (;;) {
    Token token = next();
    const bool done = token.kind == TokenKind::Eof;
    tokens.push_back(std::move(token));
    if (done)
      return tokens;
  }
}

void printTokens(const std::vector<Token> &tokens, std::ostream &os) {
  for (const Token &tok : tokens) {
    std::string where =
        std::to_string(tok.loc.line) + ":" + std::to_string(tok.loc.column);

    os << std::left << std::setw(8) << where << std::setw(15)
       << tokenKindName(tok.kind);

    // Eof has no text and printing '' for it just looks broken.
    if (tok.kind != TokenKind::Eof)
      os << "'" << tok.text << "'";

    os << "\n";
  }
}

}  // namespace hero