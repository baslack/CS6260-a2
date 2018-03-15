#!/usr/bin/env bash
NAME="results.csv";
echo "Compiling results from batch";
cat a2*.pbs.o* >> $NAME;
echo "Cleaning up batch";
rm a2*.pbs.o*;