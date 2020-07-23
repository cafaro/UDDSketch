#!/bin/sh

cmake . -B build;
cd build;
make;
make install;
mv install ../install
cd ..;
cp build/uddsketch-test/uddsketch-test install
