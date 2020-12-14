#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Illegal number of arguments!"
	exit 1
fi

if [ "$1" = "left-right" ]; then
	>&2 echo "Testing LEFT-RIGHT planarity algorithm:"
	ALG="left-right"
elif [ "$1" = "hopcroft-tarjan" ]; then
	>&2 echo "Testing HOPCROGT-TARJAN planarity algorithm:"
  ALG="hopcroft-tarjan"
else
	>&2 echo "Bad argument! 'left-right' or 'hopcroft-tarjan' is correct!"
	exit 1
fi

# Check if directory Graph exists.
if [ ! -d "Graphs/" ]; then
	mkdir Graphs/
fi

# Check if directory Graph/tmp exist.
# Generated graphs will be temporalily stored in directory Graphs/tmp/
if [ ! -d "Graphs/tmp/" ]; then
  mkdir Graphs/tmp/
fi

if [ "$(ls -A Graphs/tmp/)" ]; then
  >&2 echo "Directory Graphs/tmp/ is not empty. Removing files."
	rm -rf Graphs/tmp/*
fi

for ((i=10;i<=1000;i = i + 10));
do
  ./gal_projekt -g -p -n $i -m 9999999 -o Graphs/tmp/planar_${i}.gml >&2
  >&2 printf "Testing graph..."

  for ((j=0;j<100;j++));
  do
    OUTPUT=$(./gal_projekt -a ${ALG} -t -f Graphs/tmp/planar_${i}.gml)
    if [ $j -eq 0 ]; then
      printf "%s" "${OUTPUT}" # Stdout
    else
      printf ";%s" "${OUTPUT}" # Stdout
    fi
    #>&2 printf "\t%s - %s\n" "$j" "${OUTPUT}"
  done

  printf "\n" # Stdout

  GREEN="\e[92m"
  NORMAL="\e[0m"
  rm Graphs/tmp/planar_${i}.gml
  >&2 printf " %bDONE%b\n" "${GREEN}" "${NORMAL}"
done