#!/bin/bash
set -euf -o pipefail

BUILD_DIR="build"

# debug mode?
dirname=$BUILD_DIR
if [ "$1" ] && [ "$1" = "debug" ] ; then
    debug=true
    dirname="${dirname}_debug"
fi

# create a directory for build
cd "$(dirname "${0}")" || exit 1
if [ ! -e "$dirname" ] ; then
    mkdir "$dirname"
fi
cd $dirname || exit

# build
if [ "$debug" ] ; then
    cmake -DCMAKE_BUILD_TYPE=Debug \
        -DBUILD_TESTS=ON \
        -DBUILD_EXAMPLES=ON \
        ..
else
    cmake -DCMAKE_BUILD_TYPE=Release ..
fi
