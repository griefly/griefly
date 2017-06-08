#!/bin/bash

# Mac:
#  If you installed qt using brew you need to adjust your enviorment according to `brew info qt`
#  for example
# export PATH="/usr/local/opt/qt/bin:$PATH"
# export LDFLAGS="-L/usr/local/opt/qt/lib"
# export CPPFLAGS="-I/usr/local/opt/qt/include"

if [[ $# -eq 0 ]] ; then
    ARGS="-DBUILD_TESTS=OFF -DBUILD_COVER=OFF -DCMAKE_BUILD_TYPE=Release"
else
    ARGS="$@"
fi

mkdir -p build
cd build
cmake ../ $ARGS
make && make install
