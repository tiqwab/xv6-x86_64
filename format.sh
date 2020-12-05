#!/bin/bash

set -eux

# '-style=file' loads config from .clang-format
CLANG_FORMAT_OPTIONS="-i -style=file"

# header files
find -name '*.h' -not -path './net/lwip/*' -exec clang-format $CLANG_FORMAT_OPTIONS {} \;

# c files
find -name '*.c' -not -path './net/lwip/*' -exec clang-format $CLANG_FORMAT_OPTIONS {} \;
