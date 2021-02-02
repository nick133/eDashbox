# evAnalyst
Opensource electric motor vehicle analyst and datalogger.
Firmware for STM32-Nucleo32 board.

This is learning project in early stage, use it at your own risk.

## Build
Run 'make'

## Debug
* JLink debugger setup for VSCode https://wiki.segger.com/J-Link_Visual_Studio_Code

## IDE
VS code - it is fast and stable, especially with debugging.
Fix not found includes in VS code:
https://stackoverflow.com/questions/37522462/visual-studio-code-includepath

## Hardware
### DS18B20 temperature sensor
* Use 4.7 kOhm resistor from Vdd to Data pin, accordig to specs
* Tested with 3.3V Vdd

### SH1122 2.08" 256x64 16 colors OLED Display
https://www.aliexpress.com/item/4000866935030.html

## Integrated projects
* Tiny printf() for embedded systems:
https://github.com/mpaland/printf

* SH1122 OLED Display HAL driver:
https://github.com/mikhail-tsaryov/SH1122-STM32-HAL-Driver

* DS18B20 temperature sensor onewire HAL driver:
https://github.com/lamik/DS18B20_STM32_HAL
