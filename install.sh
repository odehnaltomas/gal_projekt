#!/bin/bash
ogdfDownloadUrl="https://ogdf.uos.de/wp-content/uploads/2020/02/ogdf.v2020.02.zip";
buildDir="build";
ogdfdDir="ogdf";

mkdir -p $ogdfdDir;
cd $ogdfdDir;
FILE="./ogdf.v2020.02.zip";
if [ ! -f "$FILE" ]; then
  wget $ogdfDownloadUrl;
fi

unzip ogdf.v2020.02.zip
cp -r OGDF/* ./
rm -rf OGDF

cd ..;
mkdir -p $buildDir;
cd $buildDir;
cmake ../$ogdfdDir;
make all;

cd ..;

cmake .;
make