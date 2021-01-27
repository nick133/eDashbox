#!/bin/bash
#
# Flash the latest firmware from Debug/Release targets
# Run from project root
#

firmware="$1"
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

build_target=$([[ "Debug/$firmware" -nt "Release/$firmware" ]] && printf Debug || printf Release)

cd $build_target

if [[ -z "$firmware" ]]
then
    echo "Usage: $(basename $0) [firmware.bin]"
    exit 0
elif [[ ! -r "$firmware" ]]
then
    echo "ERROR: Can not read firmware file '$firmware'"
    exit 1
fi

echo -e "$jscript_data" > $jscript_file

$jlinkexe -device $mcu -CommanderScript $jscript_file

rm -f $jscript_file

echo "==> Flashing firmware $build_target/$firmware completed."
