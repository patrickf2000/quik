#!/bin/bash

arch=$2
arch_cmd="-m i386"

if [[ $arch == "i386" ]] ; then
	arch_cmd="-m i386"
elif [[ $arch == "x64" ]] ; then
	arch_cmd="-m x86_64"
else
	echo "Error: Unknown architecture specified."
	exit 1
fi

INPUT="test/$1"
expected=`./get_out.py $INPUT`

build/quikc $INPUT -o build/out.bin $arch_cmd
actual=`build/out.bin`

if [[ $expected == $actual ]] ;then
	echo "Pass"
else
	echo "Fail"
	echo ""
	echo "Expected: $expected"
	echo "Actual: $actual"
	exit 1
fi

exit 0

