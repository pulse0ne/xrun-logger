#!/bin/bash
HERE=$(pwd)
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

mkdir $DIR/build
cd $DIR/build
cmake ..
make
mv xrun-logger ..
cd ..
rm -rf ./build

cd $HERE
echo "done."
