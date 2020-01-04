#!/bin/bash
# TODO: Merge with main test file

./test_x64.sh hello.qk
./test_x64.sh int1.qk

if [ $? == 1 ] ; then
	exit 1
fi

echo "Done"
