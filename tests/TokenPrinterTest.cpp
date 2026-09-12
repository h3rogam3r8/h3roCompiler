// Tests for the token dump behind heroc --emit=tokens.

#include "hero/Lexer.h"

#include <gtest/gtest.h>

#include <sstream>
#include <string>

using namespace hero;

namespace {

std::string dump(std::string_view source) {
  std::ostringstream out;
  printTokens(Lexer(source).tokenize(), out);
  return out.str();
}

}  // namespace

TEST(TokenPrinter, EmptyInputIsJustTheEofLine) {
  EXPECT_EQ(dump(""), "1:1     eof            \n");
}

TEST(TokenPrinter, ColumnsAndTextAreShown) {
  EXPECT_EQ(dump("let x"),
            "1:1     let            'let'\n"
            "1:5     identifier     'x'\n"
            "1:6     eof            \n");
}

TEST(TokenPrinter, SecondLineGetsItsOwnNumber) {
  EXPECT_EQ(dump("a\nb"),
            "1:1     identifier     'a'\n"
            "2:1     identifier     'b'\n"
            "2:2     eof            \n");
}

TEST(TokenPrinter, UnknownCharactersShowUpToo) {
  EXPECT_EQ(dump("$"),
            "1:1     unknown        '$'\n"
            "1:2     eof            \n");
}