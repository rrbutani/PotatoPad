#!/bin/python

inputFile="WeAreTheChampions.txt"
outputFile="QUEEN.aud"
dacBits=11

###############################################################################

import sys
import struct

inputArray = []
medianVal  = 2 ** (dacBits - 1)
multiplier = 1

# Get input values
with open(inputFile, 'r') as f:
    for line in f:
        inputArray = ([n for n in line.strip().split(',')])

# Fix first and last values
inputArray[0] = (inputArray[0].split('{')[1])
inputArray[len(inputArray)-1] = (inputArray[len(inputArray)-1].split('}')[0])

# Convert to ints
inputArray = [int(n) for n in inputArray]

# Find min/max, adjusted to middle
bDiff = medianVal - min(inputArray)
tDiff = max(inputArray) - medianVal

print("Min is {} lower, Max is {} higher than {}".format(bDiff, tDiff, medianVal))

# Find multiplier:
if tDiff > medianVal or bDiff < 0:
	print("Error; we don't know how to handle this data.")
	sys.exit()

multiplier = medianVal / max(tDiff, bDiff)
print("Using {} as multiplier".format(multiplier))

# Scale Data:
inputArray = [int(multiplier * (n - medianVal) + medianVal) for n in inputArray]

print(min(inputArray))
print(max(inputArray))

# Output Data:
with open(outputFile, 'wb') as output:
	# Output size
	# print(len(inputArray))
	output.write(struct.pack('<I', len(inputArray)))

	# Everything else
	output.write(struct.pack('<%sH' % len(inputArray), *inputArray))