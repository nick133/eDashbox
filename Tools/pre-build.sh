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
SRC_TOPDIRS="Core Drivers FATFS Middlewares"
UNUSED_FILES="Core/Src/freertos.c Makefile startup_stm32l432xx.s"
#BUILD_TARGETS="debug"
BUILD_TARGETS="release debug"

# Preserve custom FatFs config from overwriting by CubeMX code generation
OVERRIDE_SOURCES=FATFS/Target/ffconf.h

################################################################################

cd $(dirname $0)/..

MAIN_C=Core/Src/main.c
if grep -q '^#include "cmsis_os.h"' ${MAIN_C}
then
    echo "==> Patching main.c to disable CMSIS-RTOS code"
    sed -i \
    -e 's@^#include "cmsis_os.h"@/* & */@;' \
    -e 's@^ \+MX_FREERTOS_Init();@/* & */@' \
    -e 's@^ \+osKernelStart();@/* & */@' \
    -e 's@^ \+osKernelInitialize();@/* & */@' \
    -e 's@^void MX_FREERTOS_Init(void);@/* & */@' ${MAIN_C}
fi

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

for t in $BUILD_TARGETS
do
    if [[ ! -d "build-$t" ]]
    then
        echo "==> Creating build target: $t"
        tup variant configs/${t}.config
    fi
done

#tup
