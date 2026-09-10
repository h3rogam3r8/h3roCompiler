#include "hero/Lexer.h"

#include <gtest/gtest.h>

#include <ostream>
#include <string_view>
#include <vector>

namespace hero {

// gtest prints enums as raw numbers by default, which is useless when a
// test fails. This teaches it to print the name instead.
void PrintTo(const TokenKind &kind, std::ostream *os) {
  *os << tokenKindName(kind);
}

}  // namespace hero

using namespace hero;

namespace {

// Most tests only care about the sequence of kinds, not the text.
std::vector<TokenKind> kindsOf(std::string_view source) {
  std::vector<TokenKind> kinds;
  for (const Token &token : Lexer(source).tokenize())
    kinds.push_back(token.kind);
  return kinds;
}

}  // namespace

TEST(Lexer, EmptyInputIsJustEof) {
  const std::vector<TokenKind> expected = {TokenKind::Eof};
  EXPECT_EQ(kindsOf(""), expected);
}

TEST(Lexer, WhitespaceIsSkipped) {
  const std::vector<TokenKind> expected = {
      TokenKind::Identifier, TokenKind::Identifier, TokenKind::Eof};
  EXPECT_EQ(kindsOf("  a \n\t b  "), expected);
}

TEST(Lexer, LineCommentRunsToEndOfLine) {
  const std::vector<TokenKind> expected = {TokenKind::Identifier,
                                           TokenKind::Eof};
  EXPECT_EQ(kindsOf("// let fn tensor\nx"), expected);
}

TEST(Lexer, BlockCommentIsSkipped) {
  const std::vector<TokenKind> expected = {TokenKind::Identifier,
                                           TokenKind::Eof};
  EXPECT_EQ(kindsOf("/* let fn\n   more */ x"), expected);
}

TEST(Lexer, IdentifierKeepsItsText) {
  const std::vector<Token> tokens = Lexer("my_var2").tokenize();
  ASSERT_EQ(tokens.size(), 2u);
  EXPECT_EQ(tokens[0].kind, TokenKind::Identifier);
  EXPECT_EQ(tokens[0].text, "my_var2");
}

TEST(Lexer, KeywordsAreNotIdentifiers) {
  const std::vector<TokenKind> expected = {
      TokenKind::KwFn, TokenKind::KwLet, TokenKind::KwTensor,
      TokenKind::KwF16, TokenKind::Eof};
  EXPECT_EQ(kindsOf("fn let tensor f16"), expected);
}

// "fnord" starts with "fn" but isn't the keyword. The lexer has to read
// the whole word before deciding.
TEST(Lexer, WordsThatMerelyStartWithAKeywordAreIdentifiers) {
  const std::vector<TokenKind> expected = {TokenKind::Identifier,
                                           TokenKind::Eof};
  EXPECT_EQ(kindsOf("fnord"), expected);
}

TEST(Lexer, IntegersAndFloatsAreDifferentKinds) {
  const std::vector<TokenKind> expected = {
      TokenKind::IntLiteral, TokenKind::FloatLiteral, TokenKind::Eof};
  EXPECT_EQ(kindsOf("42 3.14"), expected);
}

TEST(Lexer, ExponentIsPartOfTheFloat) {
  const std::vector<Token> tokens = Lexer("1e-5").tokenize();
  ASSERT_EQ(tokens.size(), 2u);
  EXPECT_EQ(tokens[0].kind, TokenKind::FloatLiteral);
  EXPECT_EQ(tokens[0].text, "1e-5");
}

// The spec says there are no negative literals, so this is a minus
// applied to a 3.
TEST(Lexer, NegativeNumberIsTwoTokens) {
  const std::vector<TokenKind> expected = {
      TokenKind::Minus, TokenKind::IntLiteral, TokenKind::Eof};
  EXPECT_EQ(kindsOf("-3"), expected);
}

TEST(Lexer, ArrowIsOneTokenNotMinusThenGreater) {
  const std::vector<TokenKind> expected = {TokenKind::Arrow, TokenKind::Eof};
  EXPECT_EQ(kindsOf("->"), expected);
}

TEST(Lexer, TensorTypeComesOutAsExpected) {
  const std::vector<TokenKind> expected = {
      TokenKind::KwTensor,   TokenKind::Less,     TokenKind::LBracket,
      TokenKind::IntLiteral, TokenKind::Comma,    TokenKind::IntLiteral,
      TokenKind::RBracket,   TokenKind::Comma,    TokenKind::KwF16,
      TokenKind::Greater,    TokenKind::Eof};
  EXPECT_EQ(kindsOf("tensor<[128, 768], f16>"), expected);
}

TEST(Lexer, LinesAndColumnsStartAtOne) {
  const std::vector<Token> tokens = Lexer("a\n  b").tokenize();
  ASSERT_EQ(tokens.size(), 3u);
  EXPECT_EQ(tokens[0].loc.line, 1);
  EXPECT_EQ(tokens[0].loc.column, 1);
  EXPECT_EQ(tokens[1].loc.line, 2);
  EXPECT_EQ(tokens[1].loc.column, 3);
}