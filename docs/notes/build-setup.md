# Build setup notes

Notes to myself about why the build looks the way it does.

## No LLVM yet

The whole point of this project is MLIR and LLVM, so it feels weird not to
have them in the build. However, the lexer and parser are just plain C++ that
turns text into structs. Nothing in them needs LLVM.

Installing LLVM is a few gigabytes and getting CMake to find MLIR looks
like its own learning curve, so I'm putting it off (lol).

The part I'm unsure about: LLVM has its own string and container types
(`StringRef`, `SmallVector`) and its own diagnostics machinery, and real
LLVM projects use them everywhere instead of the standard library. So when
MLIR shows up I'll probably have to go back and convert the frontend. No
idea how bad that's gonna be.

## googletest through FetchContent

FetchContent downloads and builds googletest during the configure step, so
there's nothing to install by hand and the version is pinned in the
CMakeLists where I can see it. Costs a minute on the first configure and
needs internet.

The alternative was `brew install googletest`, which is faster, but then
the version is whatever Homebrew has that week and it wouldn't help me on
my Windows laptop.

## No lit or FileCheck yet

Those are LLVM's own testing tools and they come with LLVM, so same story
as above. My guess is they only start mattering once there's IR to check,
since what they really do is match generated text against patterns.
googletest is enough for a lexer.

## Layout

    include/hero/   headers
    src/            implementation
    tools/heroc/    the driver binary
    tests/          googletest files

The include/src split is what LLVM does. It's obviously overkill for two
files right now but I'd rather set it up than move everything later.