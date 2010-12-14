#!/bin/bash

# Cleanup directory structure
./maintainer-clean

# Make all necessary dirs
if [ ! -d build-aux ]
then
        mkdir build-aux
fi

if [ ! -d build-aux/m4 ]
then
        mkdir build-aux/m4
fi

if [ ! -d src/common ]
then
        mkdir src/common
fi

# Generate build scripts
autoreconf --install --force
