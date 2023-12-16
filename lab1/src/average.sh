#!/bin/bash
count=0
sum=0
cat $1 | 
    (while read line; do
        let count++
        sum=$(($sum+$line))
    done
echo $"Quantity=$count"
echo $"Summ=$sum"
echo $"Average=$((sum/count))"
)
