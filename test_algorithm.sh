#!/bin/bash

if [ $# -ne 2 ]; then
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

if [ "$2" = "planar" ]; then
	PLAN="planar"
elif [ "$2" = "non-planar" ]; then
  PLAN="non-planar"
else
	>&2 echo "Bad argument! 'planar' or 'non-planar' is correct!"
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

GREEN="\e[0;32m"
NORMAL="\e[0m"
RED="\e[0;31m"

for ((i=10;i<=1000;i = i + 10));
do
  if [ "${PLAN}" == "planar" ]; then
    ./gal_projekt -g -p -n $i -m 9999999 -o Graphs/tmp/planar_${i}.gml >&2
  elif [ "${PLAN}" == "non-planar" ]; then
    ./gal_projekt -g -n $i -o Graphs/tmp/planar_${i}.gml >&2
  fi

  >&2 printf "Testing graph...\n"

  OUTPUT=$(./gal_projekt -a ${ALG} -f Graphs/tmp/planar_${i}.gml)

  if [ "${OUTPUT}" == "Is planar? Yes" ] && [ "${PLAN}" == "planar" ]; then
    >&2 printf "%bALG. WORKING OK%b\n" "${GREEN}" "${NORMAL}";
  elif [ "${OUTPUT}" == "Is planar? No" ] && [ "${PLAN}" == "non-planar" ]; then
    >&2 printf "%bALG. WORKING OK%b\n" "${GREEN}" "${NORMAL}";
  else
    >&2 printf "%bALG. NOT WORKING OK%b\n" "${RED}" "${NORMAL}";
  fi

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

  rm Graphs/tmp/planar_${i}.gml
  >&2 printf "%bDONE%b\n" "${GREEN}" "${NORMAL}"
done