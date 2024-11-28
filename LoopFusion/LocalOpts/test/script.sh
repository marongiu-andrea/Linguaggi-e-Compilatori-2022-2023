#!/bin/bash

output_file="output.txt"
output_file_opt="outputOpt.txt"

output_file_clean="clean_output.txt"
output_file_opt_clean="clean_outputOpt.txt"

declare -i iterations=100;

for ((i=1; i<=iterations; i++))
do
    echo "Running program iteration $i"
    ./measurePerfTime >> "$output_file"
    ./measurePerfTimeOpt >> "$output_file_opt"
done

cut -f2 -d" " $output_file > $output_file_clean
cut -f2 -d" " $output_file_opt > $output_file_opt_clean

rm -rf $output_file
rm -rf $output_file_opt

declare -i sum=0;
declare -i sumOpt=0;

while read -r line
do
    let "sum = sum + line"
done < "$output_file_clean"

let "average = sum / iterations"

echo "Media Loop non ottimizzato su $iterations iterzioni: $average"

while read -r line
do
    let "sumOpt = sumOpt + line"
done < "$output_file_opt_clean"

let "average = sumOpt / iterations"

echo "Media Loop ottimizzato su $iterations iterzioni: $average"

rm -rf $output_file_clean
rm -rf $output_file_opt_clean
