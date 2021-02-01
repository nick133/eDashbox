#!/bin/bash

image_pgm=logo.pgm
step=4

x=$step
n=1
while [[ "$x" -lt 100 ]]
do
    convert -compress none -resize ${x}%x${x}% $image_pgm pgm:- | \
        gawk '{ if (NR>5) { gsub(/ /, "\n") }; print }' > $image_pgm.$n.pgm

    x=$(($x+$step))
    n=$(($n+1))
done
