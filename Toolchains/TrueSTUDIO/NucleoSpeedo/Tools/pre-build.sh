#!/bin/bash
#
# Executed in Release/Debug directory as:
# pre-build.sh ${BuildArtifactFileBaseName}
#

echo "==> Pre-Build script entered for $1"

cd ../Tools/pgm2c
nimble build
