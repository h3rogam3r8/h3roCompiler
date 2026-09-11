#ifndef HERO_ASTPRINTER_H
#define HERO_ASTPRINTER_H

#include "hero/AST.h"

#include <iosfwd> // More optimized iostream
#include <string>

namespace hero {

// Writes the tree out indented. Only meant for reading by eye and for
// tests, nothing reads this format back in.
void printProgram(const Program &program, std::ostream &os);

// tensor<[B, 768], f16>, or just f32 for a scalar. 
std::string typeToString(const Type &type);

}  // namespace hero

#endif  // HERO_ASTPRINTER_H