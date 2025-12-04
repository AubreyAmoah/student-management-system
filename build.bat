@echo off

REM Create build directory
mkdir build
cd build

REM Configure
cmake -G "Visual Studio 17 2022" -A x64 ^
      -DBUILD_TESTS=ON ^
      -DUSE_SYSTEM_SQLITE=OFF ^
      ..

REM Build
cmake --build . --config Release --parallel 4

REM Run tests
ctest --output-on-failure -C Release