#!/bin/bash
touch numbers.txt
for ((i=1; i<=150; i++)); do
    echo $(od -A n -t d -N 1 /dev/urandom) >> numbers.txt
done