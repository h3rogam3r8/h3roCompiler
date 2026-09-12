#include "hero/SourceFile.h"

#include <gtest/gtest.h>

using namespace hero;

// Pretty cookie cutter tests

TEST(SourceFile, KeepsNameAndText) {
  SourceFile f("thing.hero", "abc");
  EXPECT_EQ(f.name(), "thing.hero");
  EXPECT_EQ(f.text(), "abc");
}

TEST(SourceFile, SingleLine) {
  SourceFile f("t.hero", "one line");
  EXPECT_EQ(f.line(1), "one line");
}

TEST(SourceFile, SplitsOnNewlines) {
  SourceFile f("t.hero", "first\nsecond\nthird");
  EXPECT_EQ(f.line(1), "first");
  EXPECT_EQ(f.line(2), "second");
  EXPECT_EQ(f.line(3), "third");
}

TEST(SourceFile, NewlineIsNotPartOfTheLine) {
  SourceFile f("t.hero", "a\nb\n");
  EXPECT_EQ(f.line(1), "a");
  EXPECT_EQ(f.line(2), "b");
}

TEST(SourceFile, OutOfRangeGivesEmpty) {
  SourceFile f("t.hero", "only");
  EXPECT_TRUE(f.line(0).empty());
  EXPECT_TRUE(f.line(99).empty());
}

TEST(SourceFile, StripsCarriageReturn) {
  SourceFile f("t.hero", "windows\r\nline");
  EXPECT_EQ(f.line(1), "windows");
}