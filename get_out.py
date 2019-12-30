#!/usr/bin/python3
import sys

if len(sys.argv) != 2:
	print("Error: Insufficient arguments.")
	sys.exit(1)

path = sys.argv[1]

expected = ""
in_test = False

with open(path, "r") as reader:
	for line in reader:
		ln = line.strip()
		
		if len(ln) == 0:
			continue
		
		if ln == "#TEST":
			in_test = True
			continue
			
		if in_test:
			expected += str(ln[1:])
			expected += "\n"
			
expected = expected.strip()
print(expected)
