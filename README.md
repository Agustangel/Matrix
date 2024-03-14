### Linux
```sh
mkdir build-debug
cmake -B build-debug/ -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug/
# Build in release preset
cd build-debug/
make
```

## 2. How to run tests
```sh
# Navigate to build directory and run
cd build-debug/
ctest
```
