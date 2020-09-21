#!/usr/bin/env bash
set -x
set -e

rm -rf build
cmake -GNinja -DCMAKE_BUILD_TYPE=RelWithDebInfo -Bbuild -H. -DSENTRY=ON -DCMAKE_TOOLCHAIN_FILE=third_party/sentry-native/toolchains/msys2.cmake
ninja -C build
cp build/third_party/sentry-native/libsentry.dll* build/
cp build/third_party/sentry-native/crashpad_build/handler/crashpad_handler.exe ./build
sentry-cli upload-dif -o shinobu -p shinobu build/shinobu.pdb
