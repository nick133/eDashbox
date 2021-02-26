#!/bin/env bash
################################################################################
# pre-build.sh - Run before the first build or after CubeMX code generation
################################################################################
###
##  Config
#
TUPOUT=configs/Auto.tup
PGM2C_DIR=Tools/pgm2c
PGM2C=$PGM2C_DIR/bin/pgm2c
SRC_TOPDIRS="Core Drivers FATFS Middlewares/Third_Party"
DEBUG_SRC_TOPDIRS="Middlewares/Debug"
UNUSED_FILES="Makefile startup_stm32l432xx.s"
BUILD_TARGETS="debug"
#BUILD_TARGETS="release debug"

# Preserve custom FatFs config from overwriting by CubeMX code generation
OVERRIDE_SOURCES=FATFS/Target/ffconf.h

################################################################################

cd $(dirname $0)/..

echo "==> Patching sources.."
sed -i -e \
    's@^/\* \+\(#define USE_FULL_ASSERT \+1U\) \+\*/@#ifdef DEBUG\n\1\n#endif\n@' \
    Core/Inc/stm32l4xx_hal_conf.h

echo "==> Cleaning after CubeMX: $UNUSED_FILES"
rm -f $UNUSED_FILES

for to in ${OVERRIDE_SOURCES}
do
    from="`dirname $to`/override-`basename $to`"
    if ! cmp -s $from $to
    then
        echo "==> Recovering $to after CubeMX"
        cp -f $from $to
    fi
done

echo "==> Building $TUPOUT"
echo -e "# build.sh auto-generated TUP-file, do not edit!\n" > $TUPOUT

find $SRC_TOPDIRS -type f -name "*.c" -printf 'srcs += %h/*.c\n' | sort -u >> $TUPOUT
echo >> $TUPOUT
find $SRC_TOPDIRS -type f -name "*.h" -printf 'incs += -I%h\n' | sort -u >> $TUPOUT

if [[ "$DEBUG_SRC_TOPDIRS" ]]
then
    echo -e "\n\nifdef DEBUG" >> $TUPOUT
    find $DEBUG_SRC_TOPDIRS -type f -name "*.c" -printf 'srcs += %h/*.c\n' | sort -u >> $TUPOUT
    echo >> $TUPOUT
    find $DEBUG_SRC_TOPDIRS -type f -name "*.h" -printf 'incs += -I%h\n' | sort -u >> $TUPOUT
    echo -e "endif\n" >> $TUPOUT
fi

for t in $BUILD_TARGETS
do
    if [[ ! -d "build-$t" ]]
    then
        echo "==> Creating build target: $t"
        tup variant configs/${t}.config
    fi
done

#tup
