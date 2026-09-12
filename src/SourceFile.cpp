#include "hero/SourceFile.h"
#include <utility>

namespace hero {

SourceFile::SourceFile(std::string name, std::string text)
    : name_(std::move(name)), text_(std::move(text)) {
  lineStarts_.push_back(0);
  for (size_t i = 0; i < text_.size(); i++) {
    if (text_[i] == '\n')
      lineStarts_.push_back(i + 1);
  }
  // A file ending in a newline leaves a last entry pointing past the end.
  // line() handles it and dropping it would complicate the loop.
}

std::string_view SourceFile::line(int lineNumber) const {
  if (lineNumber < 1 || lineNumber > lineCount())
    return {};

  size_t start = lineStarts_[lineNumber - 1];
  if (start > text_.size())
    return {};

  size_t end = text_.find('\n', start);
  if (end == std::string::npos)
    end = text_.size();

  // Windows specific modification
  if (end > start && text_[end - 1] == '\r')
    end--;

  return std::string_view(text_).substr(start, end - start);
}

}  // namespace hero