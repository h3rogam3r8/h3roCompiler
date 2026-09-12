// Negative and edge case tests for the lexer. The other file checks that
// good input comes out right, this one checks that bad input doesn't take
// the whole thing down.

#include "hero/Lexer.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace hero;

namespace {

std::vector<TokenKind> kindsOf(std::string_view source) {
  std::vector<TokenKind> kinds;
  for (const Token &token : Lexer(source).tokenize())
    kinds.push_back(token.kind);
  return kinds;
}

}  // namespace

TEST(LexerErrors, UnknownCharacterBecomesUnknownToken) {
  const std::vector<TokenKind> expected = {TokenKind::Unknown, TokenKind::Eof};
  EXPECT_EQ(kindsOf("$"), expected);
}

// One junk character shouldn't stop the rest of the line getting lexed
TEST(LexerErrors, LexingKeepsGoingAfterUnknown) {
  const std::vector<TokenKind> expected = {
      TokenKind::Identifier, TokenKind::Unknown, TokenKind::Identifier,
      TokenKind::Eof};
  EXPECT_EQ(kindsOf("a $ b"), expected);
}

TEST(LexerErrors, UnknownTokenKeepsItsText) {
  std::vector<Token> tokens = Lexer("#").tokenize();
  ASSERT_EQ(tokens.size(), 2u);
  EXPECT_EQ(tokens[0].kind, TokenKind::Unknown);
  EXPECT_EQ(tokens[0].text, "#");
}

TEST(LexerErrors, UnterminatedBlockCommentDoesNotHang) {
  const std::vector<TokenKind> expected = {TokenKind::Eof};
  EXPECT_EQ(kindsOf("/* never closed"), expected);
}

TEST(LexerErrors, StarWithoutSlashDoesNotCloseTheComment) {
  const std::vector<TokenKind> expected = {TokenKind::Eof};
  EXPECT_EQ(kindsOf("/* has a * in it but no end"), expected);
}

TEST(LexerErrors, SlashAloneIsDivisionNotAComment) {
  const std::vector<TokenKind> expected = {
      TokenKind::Identifier, TokenKind::Slash, TokenKind::Identifier,
      TokenKind::Eof};
  EXPECT_EQ(kindsOf("a / b"), expected);
}

TEST(LexerErrors, ExponentWithNoDigitsIsNotAFloat) {
  const std::vector<TokenKind> expected = {
      TokenKind::IntLiteral, TokenKind::Identifier, TokenKind::Eof};
  EXPECT_EQ(kindsOf("1e"), expected);
}

TEST(LexerErrors, ExponentWithSignButNoDigitsIsNotAFloat) {
  std::vector<Token> tokens = Lexer("1e-").tokenize();
  ASSERT_EQ(tokens.size(), 4u);
  EXPECT_EQ(tokens[0].kind, TokenKind::IntLiteral);
  EXPECT_EQ(tokens[0].text, "1");
  EXPECT_EQ(tokens[1].kind, TokenKind::Identifier);
  EXPECT_EQ(tokens[2].kind, TokenKind::Minus);
}

TEST(LexerErrors, TrailingDotIsNotPartOfTheNumber) {
  std::vector<Token> tokens = Lexer("1.").tokenize();
  ASSERT_EQ(tokens.size(), 3u);
  EXPECT_EQ(tokens[0].kind, TokenKind::IntLiteral);
  EXPECT_EQ(tokens[0].text, "1");
  EXPECT_EQ(tokens[1].kind, TokenKind::Unknown);
}

TEST(LexerErrors, TokenizeAlwaysEndsWithExactlyOneEof) {
  std::vector<Token> tokens = Lexer("a b c").tokenize();
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens.back().kind, TokenKind::Eof);

  int eofCount = 0;
  for (const Token &t : tokens) {
    if (t.kind == TokenKind::Eof)
      eofCount++;
  }
  EXPECT_EQ(eofCount, 1);
}

// Calling next() past the end should keep handing back Eof rather than
// running off the buffer.
TEST(LexerErrors, NextKeepsReturningEofPastTheEnd) {
  Lexer lexer("a");
  EXPECT_EQ(lexer.next().kind, TokenKind::Identifier);
  EXPECT_EQ(lexer.next().kind, TokenKind::Eof);
  EXPECT_EQ(lexer.next().kind, TokenKind::Eof);
  EXPECT_EQ(lexer.next().kind, TokenKind::Eof);
}

TEST(LexerErrors, OnlyWhitespaceIsJustEof) {
  const std::vector<TokenKind> expected = {TokenKind::Eof};
  EXPECT_EQ(kindsOf("   \n\t\n  "), expected);
}

TEST(LexerErrors, OnlyCommentsIsJustEof) {
  const std::vector<TokenKind> expected = {TokenKind::Eof};
  EXPECT_EQ(kindsOf("// nothing\n/* also nothing */\n"), expected);
}

TEST(LexerErrors, ColumnKeepsCountingAfterAnUnknownCharacter) {
  std::vector<Token> tokens = Lexer("$ a").tokenize();
  ASSERT_EQ(tokens.size(), 3u);
  EXPECT_EQ(tokens[1].loc.column, 3);
}