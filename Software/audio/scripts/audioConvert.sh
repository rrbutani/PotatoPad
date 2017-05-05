#!/bin/bash
# Script to take .txt file output from Dr. Y's WavConv.m script, cut it down
# to the given size, amplifiy the audio, and generate an audio file for use
# with the TM4C (in the directory of the input file)
#
# WARNING: Will overwrite output file if it already exists.
#
# Call with input .txt file (from WavConv) as first audio input, and number
# of DAC bits (1 to 16) as second argument.
# Ex: ./audioConvert.sh potato.txt 11
#
# This script will not alter the sampling rate in any way, though it's worth
# noting that the version of WavConv in the folder this script is in is
# configured to downsample to 39.025 KHz.
#
# The output file format is as follows:
# {
#	uint32_t sizeOfAudioFile :: number of audio values in this file
#	uint16_t audioArray[sizeOfAudioFile] :: array of 16-bit audio values
# }
#
# Author: Rahul Butani
# Last modified: May 1st, 2017

# Global Variables:
output="true"

inputFile=""
dacBits=""

outputFile=""
inputArray=""
multiplier=""
arraySize=""
middleVal=""


# Colours:
BOLD='\033[0;1m' #(OR USE 31)
CYAN='\033[0;36m'
PURPLE='\033[0;35m'
GREEN='\033[0;32m'
BROWN='\033[0;33m'
RED='\033[1;31m'
NC='\033[0m' # No Color

# Helper Functions:
function print
{
    if [[ "$output" != "true" ]]; then return; fi

    N=0
    n="-e"

    if [[ "$*" == *" -n"* ]]; then
        N=1
        n="-ne"
    fi

    if [ "$#" -eq $((1 + $N)) ]; then
        echo $n $1
    elif [ "$#" -eq $((2 + $N)) ]; then
        printf ${2} && echo $n $1 && printf ${NC}
    else
        #printf ${RED} && echo "Error in print. Received: $*" && printf ${NC}
        printf ${RED} && echo "Received: $*" && printf ${NC}
    fi
}

function checkInputs
{
	# Make sure we have exactly 2 arguments
	# and that our DAC bits value is [1, 16]
	# and that our input file is a .txt file
    if [[ "$#" != "2" || ("$2" > 16 || "$2" < 1) ]]; then
    	print "Invalid arguments." $RED
    	print "$0 <path to input .txt file> <number of DAC bits; 1-16, inclusive>" $BOLD
    	exit 1
    fi

    # Check that the input file actually exists
    if [ -e $1 ]; then
    	inputFile="$1"
    else
    	print "Specified input file does not exist; please try again." $RED
    	exit 1
    fi

    dacBits=$2
}

function processInputs
{
	# Sets/creates output file...
	outputFile="$(dirname ${inputFile})/$(basename ${inpuFile} .txt).aud" && \
	touch ${outputFile} && \

	# ...sets median value based on DAC Bits...
	middleVal=$(( 2**(${dacBits}-1) )) && \

	# and grabs data from input file...
	IFS=',' read -ra inputArray < "${inputFile}" && \
	
	# ...finds the size of the array...
	arraySize=(${inputArray[0]//[/ }) && \
	arraySize=(${arraySize[3]//]/ }) && \
	
	# ...and fixes the first and last elements.
	temp=(${inputArray[0]//\{/ }) && \
	inputArray[0]=${temp[4]} && \

	temp=(${inputArray[arraySize-1]//\}/ }) && \
	inputArray[arraySize-1]=${temp[0]}

	return $?
}

function findMultiplier
{
	# Find max value:
	max=${inputArray[0]}
	for i in ${inputArray[@]}; do
		#max=$([ "$i" -gt "$max" ] && echo "$i" || echo "$max")
		if [ $i -gt $max ]; then
			max=$i
		fi
	done
	print "Max value is ${max}" $PURPLE

	# Find min value:
	min=${inputArray[0]}
	for i in ${inputArray[@]}; do
		#min=$([ "$i" -lt "$min" ] && echo "$i" || echo "$min")
		if [ "$i" -lt "$min" ]; then
			min=$i
		fi
	done
	print "Min value is ${min}" $PURPLE

	# Normalize to median:
	max=$(( ${max} - ${middleVal} ))
	min=$(( ${middleVal} - ${min} ))

	# Throw an error if negative:
	if [ "$max" -lt 0 ] || [ "$min" -lt 0 ]; then
		print "Error; we don't know how to handle this data." $RED
		exit 1
	fi

	# Find lower diff, compare to half of full range, and generate mutliplier
	# from there:
	# Multiplier = (1/2 range) / lower diff
	multiplier=$([ "$max" -lt "$min" ] && echo "$max" || echo "$min)"
	echo $multiplier
	multiplier=$(( ${middleVal} / ${multiplier} ))
	print "Using ${multiplier} as multiplier" $PURPLE

	return $?
}

function scaleData
{
	# Scales each value of the array:
	# newVal = multiplier * (oldVal - middleVal) + middleVal

	b=$(( $arraySize - 1 ))
	for i in `seq 0 $b`; do
		${inputArray[$i]}=$(( (${multiplier} * (${inputArray[$i]} - ${middleVal}) ) + ${middleVal} ))
	done

	return $?
}

function outputData
{
	print "Outputting data..." $PURPLE

	printf "0 : %.4x" ${arraySize} | xxd -r -g0 > ${outputFile}

	b=$(( $arraySize - 1 ))
	for i in `seq 0 $b`; do
		printf "0 : %.4x" ${inputArray[$i]} | xxd -r -g0 >> ${outputFile}
	done
}
	
function fin
{
	if [[ "$1" != 0 ]]; then
		print "Something went wrong. Good Luck!" $RED
		exit $1
	else
		print "Success! Output is ${outputFile}" $CYAN
		exit 0
	fi
}

checkInputs "${@}" && \
processInputs && \
findMultiplier && \
scaleData && \
outputData

fin $?
