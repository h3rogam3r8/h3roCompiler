#ifndef HERO_DIAGNOSTICS_H
#define HERO_DIAGNOSTICS_H

#include "hero/SourceFile.h"

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

namespace hero {

enum class Severity {
  Error,
  Warning,
  Note,
};

struct Diagnostic {
  Severity severity = Severity::Error;
  SourceLoc loc;
  std::string message;
};
// The spec has error codes (E001 and friends) but those are mostly about
// types and everything here is syntax so far. Adding them once sema is
// a thing.
class Diagnostics {
public:
  explicit Diagnostics(const SourceFile &file) : file_(&file) {}

  void error(SourceLoc loc, std::string message);
  void warning(SourceLoc loc, std::string message);

  bool hasErrors() const;
  size_t count() const { return diags_.size(); }
  const std::vector<Diagnostic> &all() const { return diags_; }

  void print(std::ostream &os) const;

  // One diagnostic on its own. Exposed so tests can check the formatting
  // without going through a stream.
  std::string render(const Diagnostic &diag) const;

private:
  const SourceFile *file_;
  std::vector<Diagnostic> diags_;
};

}  // namespace hero

#endif