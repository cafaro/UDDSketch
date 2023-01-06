#!/bin/sh

#cmake . -B build -D CMAKE_BUILD_TYPE=Debug;
cmake . -B build
cd build;
make;
make install;
mv install ../install
cd ..;
cp build/uddsketch-test/uddsketch-test install
