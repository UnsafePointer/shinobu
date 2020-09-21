rmdir /q /q build
cmake -Bbuild -G "MSYS Makefiles"
cmake --build build --parallel
