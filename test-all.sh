#!/bin/bash

for entry in "./test"/*.qk
do
	entry=`basename $entry`
	echo "$entry"
	./test.sh $entry
	echo ""
done
