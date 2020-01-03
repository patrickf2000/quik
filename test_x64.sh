#!/bin/bash

INPUT="test/$1"
expected=`./get_out.py $INPUT`

build/quikc $INPUT -o build/out.bin
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

