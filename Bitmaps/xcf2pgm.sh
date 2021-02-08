#!/bin/bash

xcf_file="/home/nick/STM32Cube/Projects/evAnalyst/Bitmaps/oled-gui-screens.xcf"

# https://gist.github.com/danstuken/3625841
gimp -i -b - << EOF
(let*
  (
    (xcfFilename "$xcf_file")
    (pgmFilename (string-append (substring xcfFilename 0 (- (string-length xcfFilename) 4) ) ".pgm"))
    (xcfImage (car (gimp-file-load RUN-NONINTERACTIVE xcfFilename xcfFilename)))
    (xcfDrawable (car (gimp-image-flatten xcfImage)))
  )
  (file-pgm-save 1 xcfImage xcfDrawable pgmFilename pgmFilename FALSE)
)
(gimp-quit TRUE)
EOF
