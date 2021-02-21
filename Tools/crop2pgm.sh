#!/bin/bash

if [[ -z "$1" ]]
then
    echo "Usage: $(basename $0) input-image.pgm colormap.pgm output-basename"
    exit 1
fi

image=$1
colormap=$2
outbase=$3

convert "$image" -compress none +repage -crop '10x1@' -remap $colormap ${outbase}%d.pgm

for i in ${outbase}*.pgm
do
    sed -i 's/ $//;6,$s/ /\n/g' $i
done
