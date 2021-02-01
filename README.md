# XEVDO
eXtended Electric Vehicle Datalogger, Opensource.
Firmware for STM32-Nucleo32 board.

This is learning project in early stage, use it at your own risk.

## Build
Run 'make'

## Debug
* JLink debugger setup for VSCode https://wiki.segger.com/J-Link_Visual_Studio_Code

## IDE
Fix not found includes in VS code:
https://stackoverflow.com/questions/37522462/visual-studio-code-includepath

## Hardware
### DS18B20 temperature sensor
* Use 4.7 kOhm resistor from Vdd to Data pin, accordig to specs
* Tested with 3.3V Vdd
