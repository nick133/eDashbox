#!/bin/bash
#
# Executed in Release/Debug directory as:
# pre-build.sh ${BuildArtifactFileBaseName}
#

echo "==> Pre-Build script entered for $1"

pgm2c_dir="../Tools/pgm2c"
pgm2c_bin="$pgm2c_dir/bin/pgm2c"

if [[ ! -x "$pgm2c_bin" || "$pgm2c_bin" -ot "$pgm2c_dir/src/pgm2c.nim" ]]
then
    cd $pgm2c_dir
    nimble build
fi
