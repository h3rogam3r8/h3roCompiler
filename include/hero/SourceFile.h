#ifndef HERO_SOURCEFILE_H
#define HERO_SOURCEFILE_H

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace hero {

// Moved here out of Lexer.h. The lexer makes these but the diagnostics
// code needs them too, and having Diagnostics.h pull in the whole lexer
// felt backwards.
struct SourceLoc {
  int line = 1;
  int column = 1;
};

// Holds a file's text and remembers where every line starts, so an error
// can print the line it's complaining about.
class SourceFile {
public:
  SourceFile(std::string name, std::string text);

  const std::string &name() const { return name_; }
  const std::string &text() const { return text_; }

  int lineCount() const { return (int)lineStarts_.size(); }

  // Line 1 is the first line. Out of range gives back an empty view.
  //The newline itself is not included.
  std::string_view line(int lineNumber) const;

private:
  std::string name_;
  std::string text_;

  // Byte offset
  std::vector<size_t> lineStarts_;
};

}  // namespace hero

#endif  // HERO_SOURCEFILE_H