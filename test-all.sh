#!/bin/bash

for entry in "./test"/*.qk
do
	entry=`basename $entry`
	echo "$entry"
	./test.sh $entry
	
	if [ $? == 1 ] ; then
		exit 1
	fi
	
	echo ""
done

echo ""
echo "Cleaning up..."

rm /tmp/*.asm
rm /tmp/*.o

echo "Done"
