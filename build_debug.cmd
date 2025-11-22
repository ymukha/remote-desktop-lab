rmdir /S /Q build
cmake --preset windows-vcpkg
cd build
cmake --build . --config Debug
cd ..