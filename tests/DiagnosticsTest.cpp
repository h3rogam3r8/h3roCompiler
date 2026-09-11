#include "hero/Diagnostics.h"
#include "hero/SourceFile.h"

#include <gtest/gtest.h>

#include <sstream>

using namespace hero;

TEST(Diagnostics, StartsEmpty) {
  SourceFile f("t.hero", "x");
  Diagnostics diags(f);
  EXPECT_FALSE(diags.hasErrors());
  EXPECT_EQ(diags.count(), 0u);
}

TEST(Diagnostics, WarningIsNotAnError) {
  SourceFile f("t.hero", "x");
  Diagnostics diags(f);
  diags.warning(SourceLoc{1, 1}, "just saying");
  EXPECT_FALSE(diags.hasErrors());
  EXPECT_EQ(diags.count(), 1u);
}

TEST(Diagnostics, CaretLandsUnderTheColumn) {
  SourceFile f("bad.hero", "let a = 1.0 a");
  Diagnostics diags(f);
  diags.error(SourceLoc{1, 13}, "expected ;");

  std::ostringstream out;
  diags.print(out);

  EXPECT_EQ(out.str(),
            "bad.hero:1:13: error: expected ;\n"
            "let a = 1.0 a\n"
            "            ^\n");
}

TEST(Diagnostics, PicksTheRightLine) {
  SourceFile f("t.hero", "line one\nline two\nline three");
  Diagnostics diags(f);
  diags.error(SourceLoc{2, 6}, "something");

  std::ostringstream out;
  diags.print(out);

  EXPECT_EQ(out.str(),
            "t.hero:2:6: error: something\n"
            "line two\n"
            "     ^\n");
}

// Padding is copied from the line so tabs don't throw it off
TEST(Diagnostics, TabsGetCopiedIntoThePadding) {
  SourceFile f("t.hero", "\t\tlet x = 1;");
  Diagnostics diags(f);
  diags.error(SourceLoc{1, 3}, "nope");

  std::ostringstream out;
  diags.print(out);

  EXPECT_EQ(out.str(),
            "t.hero:1:3: error: nope\n"
            "\t\tlet x = 1;\n"
            "\t\t^\n");
}

TEST(Diagnostics, PrintsAllOfThemInOrder) {
  SourceFile f("t.hero", "a\nb");
  Diagnostics diags(f);
  diags.error(SourceLoc{1, 1}, "first");
  diags.error(SourceLoc{2, 1}, "second");

  std::ostringstream out;
  diags.print(out);
  EXPECT_EQ(out.str(),
            "t.hero:1:1: error: first\n"
            "a\n"
            "^\n"
            "t.hero:2:1: error: second\n"
            "b\n"
            "^\n");
}