#!/bin/bash
#TODO: Somehow merge into CMakeLists

if [ ! -d ./build ] ; then
	echo "Error: The compiler does not seem to be built."
	exit 1
fi

cd build

./quikc ../stdlib/test.qk -c --lib

gcc -m32 -c ../stdlib/stdio.c
gcc -m32 -c ../stdlib/test.c -o test2.o

gcc -shared -m32 test.o test2.o stdio.o -o libqkstdlib.so

cd ..

echo "Done"
