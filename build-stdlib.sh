#!/bin/bash
#TODO: Somehow merge into CMakeLists

if [ ! -d ./build ] ; then
	echo "Error: The compiler does not seem to be built."
	exit 1
fi

cd build

# Build for ARM
if [[ $1 == "armv7" ]] ; then
	gcc -fPIC -c ../stdlib/stdio.c
	gcc -fPIC -c ../stdlib/test.c -o test2.o
	gcc -shared test2.o stdio.o -o libqkstdlib.so
	
# Build for Intel
else
	#Build 32-bit
	./quikc ../stdlib/test.qk -c --lib -m i386

	gcc -m32 -c ../stdlib/stdio.c
	gcc -m32 -c ../stdlib/test.c -o test2.o

	gcc -shared -m32 test.o test2.o stdio.o -o libqkstdlib32.so

	#Build 64-bit
	gcc -fPIC -c ../stdlib/stdio.c
	gcc -fPIC -c ../stdlib/test.c -o test2.o

	gcc -shared test2.o stdio.o -o libqkstdlib.so
fi

cd ..

echo "Done"

