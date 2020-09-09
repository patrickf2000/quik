#!/bin/bash

INSTALL_PATH="/usr/local/include/quik"

if [ ! -d $INSTALL_PATH ] ; then
	sudo mkdir -p $INSTALL_PATH
fi

if [ ! -f ./build/libqkstdlib.so ] ; then
	sudo cp -v ./stdlib/headers/*.qk $INSTALL_PATH
	echo "Error: The standard library does not seem to be built."
	echo "Plese build it, and try again."
	exit 1
fi

sudo cp -v ./build/libqkstdlib.so /usr/lib
sudo cp -v ./build/libqkstdlib32.so /usr/lib
sudo ldconfig

echo "Done!"
