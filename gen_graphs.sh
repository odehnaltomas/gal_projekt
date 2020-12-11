#!/bin/bash

if [ $# -ne 2 ]; then
	echo "Illegal number of arguments!"
	exit 1
fi

if [ "$1" = "non_planar" ]; then
	echo "Generating NON_PLANAR graphs:"
	FILE_PREFIX="non_planar"
elif [ "$1" = "planar" ]; then
	echo "Generating PLANAR graphs:"
	FILE_PREFIX="planar"
else
	echo "Bad argument! 'non_planar' or 'planar' is correct!"
	exit 1
fi

if ! [[ "$2" =~ ^[0-9]+$ ]]; then
	echo "Second argument must be number and greater than 4!"
	exit 1
fi
if [ "$2" -lt 5 ] && [ $FILE_PREFIX = "non_planar" ]; then
	echo "Second argument must be number greater than 4!"
	exit 1
fi

if [ ! -d "Graphs/" ]; then
	mkdir Graphs/
fi

if [ ! -d "Graphs/$FILE_PREFIX" ]; then
	mkdir Graphs/$FILE_PREFIX/
fi

if [ $FILE_PREFIX = "non_planar" ]; then
	if [ "$(ls -A Graphs/${FILE_PREFIX})" ]; then
		echo "Directory Graphs/${FILE_PREFIX} is not empty."
		rm Graphs/${FILE_PREFIX}/*
	fi

	for ((i=5;i<=$2;i++));
	do
		./gal_projekt -g -n $i -o Graphs/${FILE_PREFIX}/${FILE_PREFIX}_${i}.gml
	done
elif [ $FILE_PREFIX = "planar" ]; then
        if [ "$(ls -A Graphs/${FILE_PREFIX})" ]; then
                echo "Directory Graphs/${FILE_PREFIX} is not empty."
                rm Graphs/${FILE_PREFIX}/*
        fi

        for ((i=1;i<=$2;i++));
        do
                ./gal_projekt -g -p -n $i -m 9999 -o Graphs/${FILE_PREFIX}/${FILE_PREFIX}_${i}.gml
        done

fi
