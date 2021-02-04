#.ONESHELL:

######################################
# target
######################################
TARGET = evAnalyst

######################################
# building variables
######################################
# debug build?
DEBUG = 1

# optimization, build path
ifeq ($(DEBUG),1)
OPT = -Og
BUILD_DIR = build/debug
else
BUILD_DIR = build/release
OPT = -O2
endif

######################################
# source
######################################
# C sources
C_SOURCES = \
$(wildcard Core/Src/*.c) \
Drivers/SH1122_Driver/Src/sh1122.c \
$(wildcard Drivers/SH1122_Driver/Src/fonts/*.c) \
$(wildcard Drivers/DS18B20_HAL_Driver/Src/*.c) \
FATFS/App/fatfs.c \
FATFS/Target/user_diskio.c \
FATFS/Target/fatfs_sd.c \
$(wildcard Middlewares/Third_Party/FatFs/src/*.c) \
$(wildcard Middlewares/Third_Party/FatFs/src/option/*.c) \
$(wildcard Middlewares/Third_Party/FreeRTOS/Source/*.c) \
Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c \
Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c \
Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c \
$(wildcard Drivers/STM32L4xx_HAL_Driver/Src/*.c) \
Core/Src/system_stm32l4xx.c

# Exclude
C_SRC_EXCLUDE = \
Core/Src/freertos.c
C_SOURCES := $(sort $(filter-out $(C_SRC_EXCLUDE),$(C_SOURCES)))

# ASM sources
ASM_SOURCES = startup_stm32l432xx.s


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -v -O ihex
BIN = $(CP) -v -O binary -S

PGM2C_PATH = Tools/pgm2c
PGM2C = $(PGM2C_PATH)/bin/pgm2c

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32L432xx

# AS includes
AS_INCLUDES =  \
-ICore/Inc

# C includes
C_INCLUDES =  \
-ICore/Inc \
-IDrivers/SH1122_Driver/Inc \
-IDrivers/SH1122_Driver/Inc/fonts \
-IDrivers/DS18B20_HAL_Driver/Inc \
-IDrivers/STM32L4xx_HAL_Driver/Inc \
-IDrivers/STM32L4xx_HAL_Driver/Inc/Legacy \
-IFATFS/App \
-IFATFS/Target \
-IMiddlewares/Third_Party/FatFs/src \
-IMiddlewares/Third_Party/FreeRTOS/Source/include \
-IMiddlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 \
-IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F \
-IDrivers/CMSIS/Device/ST/STM32L4xx/Include \
-IDrivers/CMSIS/Include

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32L432KCUx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
# float support for sprintf(), adds ~2Kb to firmware size vs gcvt()
#LDFLAGS += -u _printf_float

# default action: build all
all: pre-build $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin post-build

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile $(LDSCRIPT)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(LDSCRIPT): TrueSTUDIO/$(TARGET)/STM32L432KC_FLASH.ld
	@cp -f $< $@

$(BUILD_DIR):
	mkdir -p $@

# After build
$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@$(HEX) $< $@
	@stat -c "%n: %s bytes" $(BUILD_DIR)/$(TARGET).hex
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@$(BIN) $< $@	
	@stat -c "%n: %s bytes" $(BUILD_DIR)/$(TARGET).bin

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

#######################################
# pre/post-build actions
#######################################

# Comment CMSIS-RTOS related code as we prefer raw FreeRTOS
pre-build: MAIN_C := Core/Src/main.c
pre-build: $(PGM2C)
	@grep -q '^#include "cmsis_os.h"' $(MAIN_C) && sed -i \
		-e 's@^#include "cmsis_os.h"@/* & */@;' \
		-e 's@^ \+MX_FREERTOS_Init();@/* & */@' \
		-e 's@^ \+osKernelStart();@/* & */@' \
		-e 's@^ \+osKernelInitialize();@/* & */@' \
		-e 's@^void MX_FREERTOS_Init(void);@/* & */@' $(MAIN_C) || true

$(PGM2C): $(PGM2C_PATH)/src/pgm2c.nim
	@cd $(PGM2C_PATH) && nimble build

post-build:

gflash:
	@jflash build/debug/evAnalyst.bin

flash:
	@jflash build/release/evAnalyst.bin

a:
	@echo $(C_SOURCES)|most
