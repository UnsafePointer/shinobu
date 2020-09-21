#!/usr/bin/env bash
set -x
set -e

rm -rf build shinobu.debug
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -Bbuild -DSENTRY=ON
cmake --build build --parallel
objcopy --only-keep-debug build/shinobu shinobu.debug
sentry-cli upload-dif -o shinobu -p shinobu shinobu.debug
