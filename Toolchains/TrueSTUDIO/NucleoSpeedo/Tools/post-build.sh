#!/bin/bash
#
# Executed in Release/Debug directory as:
# post-build.sh ${BuildArtifactFileBaseName}
#

proj="$1"
elf="${proj}.elf"

echo "==> Creating firmware.."

arm-atollic-eabi-objcopy -v -O ihex "$elf" "${proj}.hex"
arm-atollic-eabi-objcopy -v -O binary "$elf" "${proj}.bin"

