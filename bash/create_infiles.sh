#!/bin/bash

inputFile=$1
input_dir=$2
numFilesPerDirectory=$3

if [[ "$#" -ne 3 ]]; then                                    
    echo "There must be exactly 3 arguments in the command line!"
    exit 1
fi

if [[ "$numFilesPerDirectory" -lt 1 ]]; then
    echo "Invalid input for number of files per directory!"
    exit 1
fi

if [ -d "$input_dir" ]; then
    echo "Directory already exists!"
    exit 1
else
    `mkdir $input_dir`
    echo "Directory created!"
fi

while read line; do
    COUNTRY=$(echo $line | cut -d' ' -f 4)
    if [ -d "$input_dir/$COUNTRY" ]; then
        echo "Subdirectory already exists!"
    else
        `mkdir $input_dir/$COUNTRY`
        for (( i=1; i<=numFilesPerDirectory; i++ ))
        do
            touch $input_dir/$COUNTRY/$COUNTRY-$i.txt
        done
        echo "Subdirectory created!"
    fi
    
    check=0;
    max=$( wc -l <"$input_dir/$COUNTRY/$COUNTRY-1.txt" );
    for (( i=2; i<=numFilesPerDirectory; i++ ))
    do
        counter=$( wc -l <"$input_dir/$COUNTRY/$COUNTRY-$i.txt" );
        if [[ max -gt counter ]]; then
            echo $line >> "$input_dir/$COUNTRY/$COUNTRY-$i.txt"
            check=1;
            break;
        fi
    done
    
    if [[ check -eq 0 ]]; then
        echo $line >> "$input_dir/$COUNTRY/$COUNTRY-1.txt"
    fi
    
done < $inputFile