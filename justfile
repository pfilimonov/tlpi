build *args:
  rm -rf build/
  cmake -S . -B build {{args}}
  cmake --build build

run *args:
  build/src/solution {{args}}
