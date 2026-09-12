// The compiler driver. Reads a .hero file and prints the tree. Argument
// handling is the same shape as the one in Alpha, just smaller.

#include "hero/ASTPrinter.h"
#include "hero/Parser.h"
#include "hero/SourceFile.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

void usage() {
  std::cerr << "usage: heroc <file.hero> [--emit=ast]\n";
}

}  // namespace

int main(int argc, char **argv) {
  std::string path;
  std::string emit = "ast";  // the only thing it can do right now

  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];

    // No starts_with until C++20 so this is the rfind trick. Not gonna use C++20 cause of LLVM issues.
    if (arg.rfind("--emit=", 0) == 0) {
      emit = arg.substr(7);
    } else if (arg == "-h" || arg == "--help") {
      usage();
      return 0;
    } else if (!arg.empty() && arg[0] == '-') {
      std::cerr << "heroc: unknown option " << arg << "\n";
      return 1;
    } else {
      path = arg;
    }
  }

  if (path.empty()) {
    usage();
    return 1;
  }

  if (emit != "ast") {
    std::cerr << "heroc: only --emit=ast works so far\n";
    return 1;
  }

  std::ifstream in(path);
  if (!in) {
    std::cerr << "heroc: could not open " << path << "\n";
    return 1;
  }

  std::stringstream buffer;
  buffer << in.rdbuf();
  std::string source = buffer.str();

  hero::SourceFile file(path, source);
  hero::Parser parser(file);

  auto program = parser.parse();
  if (!program) {
    parser.diags().print(std::cerr);
    return 1;
  }

  hero::printProgram(*program, std::cout);
  return 0;
}