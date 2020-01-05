#!/bin/bash

arch=$1

if [[ $arch == "i386" ]] ; then
	for entry in "./test"/*.qk
	do
		entry=`basename $entry`
		echo "$entry"
		./test.sh "test/$entry" i386
		
		if [ $? == 1 ] ; then
			exit 1
		fi
		
		echo ""
	done
elif [[ $arch == "x64" ]] ; then
	for entry in "./test"/*.qk
	do
		entry=`basename $entry`
		echo "$entry"
		./test.sh "test/$entry" x64
		
		if [ $? == 1 ] ; then
			exit 1
		fi
		
		echo ""
	done
	
	#Test stuff specific to x86_64
	for entry in "./test/x64"/*.qk
	do
		entry=`basename $entry`
		echo "$entry"
		./test.sh "test/x64/$entry" x64
		
		if [ $? == 1 ] ; then
			exit 1
		fi
		
		echo ""
	done
else
	echo "Error: Unknown or unsupported architecture."
	echo "The choices are i386 (32-bit) or x64 (64-bit)."
	exit 1
fi

echo ""
echo "Cleaning up..."

rm /tmp/*.asm
rm /tmp/*.o

echo "Done"
