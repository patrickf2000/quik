#!/bin/bash

if [ ! -f ./build/libqkstdlib.so ] ; then
	echo "Error: The standard library does not seem to be built."
	echo "Plese build it, and try again."
	exit 1
fi

sudo cp -v ./build/libqkstdlib.so /usr/local/lib
sudo ldconfig

INSTALL_PATH="/usr/local/include/quik"

if [ ! -d $INSTALL_PATH ] ; then
	sudo mkdir -p $INSTALL_PATH
fi

sudo cp -v ./stdlib/headers/*.qk $INSTALL_PATH

echo "Done!"
