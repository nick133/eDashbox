#!/bin/bash

[[ -x pgm2c && pgm2c -nt pgm2c.nim ]] || nim c --verbosity:0 pgm2c.nim
./pgm2c -h:bitmaps.h -c:bitmaps.c -p:../../Bitmaps/PGM
