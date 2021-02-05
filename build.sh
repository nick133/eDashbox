#!/bin/env bash
#

# Comment CMSIS-RTOS related code as we prefer raw FreeRTOS
MAIN_C=Core/Src/main.c
grep -q '^#include "cmsis_os.h"' ${MAIN_C} && sed -i \
    -e 's@^#include "cmsis_os.h"@/* & */@;' \
    -e 's@^ \+MX_FREERTOS_Init();@/* & */@' \
    -e 's@^ \+osKernelStart();@/* & */@' \
    -e 's@^ \+osKernelInitialize();@/* & */@' \
    -e 's@^void MX_FREERTOS_Init(void);@/* & */@' ${MAIN_C}

# Preserve custom FatFs config from overwriting by CubeMX code generation
OVERRIDE_SOURCES=FATFS/Target/ffconf.h
for to in ${OVERRIDE_SOURCES}
do
    from="`dirname $to`/override-`basename $to`"
    cmp -s $from $to || cp -f $from $to
done

tup
