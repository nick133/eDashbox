#!/bin/bash

xcf_file="$1"
pgm_file="$2"
map_file="$3"

if [[ -z "$xcf_file" || -z "$pgm_file" || ! -r "$xcf_file" ]]
then
    echo -e "Usage: $(basename $0) input-image.xcf output-image.pgm colormap.pgm"
    exit 1
fi

xcf_file=$(realpath $xcf_file)
pgm_file=$(realpath $pgm_file)

# https://gist.github.com/danstuken/3625841
gimp -i -b - >/dev/null 2>&1 << EOF
(let*
  (
    (xcfFilename "$xcf_file")
    (pgmFilename "$pgm_file")
    (xcfImage (car (gimp-file-load RUN-NONINTERACTIVE xcfFilename xcfFilename)))
    (xcfDrawable (car (gimp-image-flatten xcfImage)))
  )
  (file-pgm-save 1 xcfImage xcfDrawable pgmFilename pgmFilename FALSE)
)
(gimp-quit TRUE)
EOF

if [[ ! -f "$pgm_file" ]]
then
    echo -e "GIMP ERROR: can not convert image!"
    exit 1
fi

# Convert pixmap to greymap
if [[ "$(head -c 2 $pgm_file)" == "P3" ]]
then
    pgmcopy="${pgm_file}-orig.pgm"
    mv -f $pgm_file ${pgmcopy}
    convert ${pgmcopy} -remap $map_file -compress none pgm:- | sed 's/ $//;6,$s/ /\n/g' > ${pgm_file}
    rm -f ${pgmcopy}
fi

exit 0
