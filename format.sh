#!/bin/bash

set -eux

# '-style=file' loads config from .clang-format
CLANG_FORMAT_OPTIONS="-i -style=file"

# header files
find -name '*.h' -exec clang-format $CLANG_FORMAT_OPTIONS {} \;

# c files
find -name '*.c' -exec clang-format $CLANG_FORMAT_OPTIONS {} \;
