#!/bin/bash

TARGET="arm-softmmu"

if [ ! -e build/ ]; then mkdir build; fi
pushd build

../configure --target-list=${TARGET} \
	--disable-capstone \
	--enable-debug

make -j6
popd
