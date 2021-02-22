#!/bin/bash

if [[ -z "$4" ]]
then
    echo "Usage: $(basename $0) input-image.pgm colormap.pgm XxY output-basename|output.pgm"
    exit 1
fi

image=$1
colormap=$2
cropsize=$3
outbase=$4
outfile=${outbase}
pgmglob=${outbase}

# multiple output
if [[ $cropsize =~ .+\@$ ]]
then
    outfile="${outbase}%d.pgm"
    pgmglob="${outbase}*.pgm"
fi

convert "$image" -compress none -crop "${cropsize}" -remap $colormap ${outfile}

for i in $pgmglob
do
    sed -i 's/ $//;6,$s/ /\n/g' $i
done
