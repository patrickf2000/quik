#!/bin/bash

for entry in "./test/error"/*.qk
do
	entry=`basename $entry`
	echo "$entry"
	o=`build/quikc "./test/error/$entry" -m x86_64`
	
	if [ $? == 0 ] ; then
		echo "Fail"
		exit 1
	else
		echo "Pass"
	fi
	
	echo ""
done

echo ""
echo "Done"
echo ""
