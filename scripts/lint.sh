#!/bin/bash -e

PROJ_DIR="/home/cgev/Projects/CLionProjects/active/shamans"
find "${PROJ_DIR}/src" -type f \
  \( -name \*.c -o -name \*.cc -o -name \*.cpp -o -name *.h \) \
  -print0 \
  | xargs -0 "${PROJ_DIR}/third_party/cpplint.py" \
  --filter='-legal,-runtime/references,-build/include,-build/c++11'

