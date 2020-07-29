#!/bin/bash

set -eux

CLANG_FORMAT_OPTIONS="-i -style=LLVM"

# header files
find -name '*.h' -exec clang-format $CLANG_FORMAT_OPTIONS {} \;

# c files
find -name '*.c' -exec clang-format $CLANG_FORMAT_OPTIONS {} \;
