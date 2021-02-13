#!/bin/env bash
#

project=evAnalyst
target=debug

firmware=${project}.bin
build_dir="$(realpath $(dirname $0)/../build-${target})"

mcu=STM32L432KC
jlinkexe="/opt/SEGGER/JLink/JLinkExe"

jscript_file="/tmp/firmware-flash.jlink"
jscript_data="si SWD\n\
speed 4000\n\
r\n\
h\n\
loadbin $firmware, 0x08000000\n\
verifybin $firmware, 0x08000000\n\
r\n\
go\n\
qc\n"

if [[ ! -r "$build_dir/$firmware" ]]
then
    echo "Error: Can not read firmware '$build_dir/$firmware', build it first."
    exit 1
fi

cd $build_dir

echo -e "$jscript_data" > $jscript_file

$jlinkexe -device $mcu -CommanderScript $jscript_file

rm -f $jscript_file
