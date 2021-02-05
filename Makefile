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
BUILD_ROOT = build
ifeq ($(DEBUG),1)
OPT = -Og
BUILD_DIR = $(BUILD_ROOT)/debug
else
BUILD_DIR = $(BUILD_ROOT)/release
OPT = -Ofast
#OPT = -O3
endif

#######################################
# dependencies
#######################################
-include $(shell find $(BUILD_DIR) -name "*.d")

######################################
# source
######################################
# C sources
SOURCE_DIRS :=  Core Drivers FATFS Middlewares
C_SOURCES_EXCLUDE := Core/Src/freertos.c
# Preserve files not to be overwriten by CubeMX
OVERRIDE_C_SOURCES := FATFS/Target/ffconf.h

C_SOURCES := $(filter-out $(C_SOURCES_EXCLUDE),$(shell find $(SOURCE_DIRS) -name "*.c"))

# ASM sources
ASM_SOURCES := startup/startup_stm32l432xx.s

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
# mcu
MCU := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# AS defines
AS_DEFS = 

# C defines
C_DEFS = -DUSE_HAL_DRIVER -DSTM32L432xx

# AS includes
AS_INCLUDES := -ICore/Inc

# C includes (remove trailing slash)
C_INCLUDES := $(addprefix -I,\
	$(subst $(PWD)/,,$(realpath $(sort $(dir \
	$(shell find $(SOURCE_DIRS) -name "*.h"))))))

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
LDSCRIPT := STM32L432KCUx_FLASH.ld

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
OBJECTS = $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o))
vpath %.c $(sort $(dir $(C_SOURCES)))

# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

OBJECTS_PATTERN := $(addsuffix %.o,$(dir $(OBJECTS)))

$(OBJECTS_PATTERN): %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(<:.c=.lst) $< -o $@

$(OBJECTS_PATTERN): %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile $(LDSCRIPT)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(LDSCRIPT): TrueSTUDIO/$(TARGET)/STM32L432KC_FLASH.ld
	@cp -f $< $@

$(BUILD_DIR):
	@mkdir -p $(sort $(dir $(OBJECTS)))

# After build
$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@$(HEX) $< $@
	@stat -c "%n: %s bytes" $(BUILD_DIR)/$(TARGET).hex
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@$(BIN) $< $@	
	@stat -c "%n: %s bytes" $(BUILD_DIR)/$(TARGET).bin

rebuild: clean all

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_ROOT)

#######################################
# pre/post-build actions
#######################################

# 1. Comment CMSIS-RTOS related code as we prefer raw FreeRTOS
# 2. Preserve files not to be overwriten by CubeMX
pre-build: MAIN_C := Core/Src/main.c
pre-build: $(PGM2C)
	@grep -q '^#include "cmsis_os.h"' $(MAIN_C) && sed -i \
		-e 's@^#include "cmsis_os.h"@/* & */@;' \
		-e 's@^ \+MX_FREERTOS_Init();@/* & */@' \
		-e 's@^ \+osKernelStart();@/* & */@' \
		-e 's@^ \+osKernelInitialize();@/* & */@' \
		-e 's@^void MX_FREERTOS_Init(void);@/* & */@' $(MAIN_C) || true
	@for to in $(OVERRIDE_C_SOURCES); do \
		from="`dirname $$to`/override-`basename $$to`";\
		cmp -s $$from $$to || cp -f $$from $$to; done

$(PGM2C): $(PGM2C_PATH)/src/pgm2c.nim
	@cd $(PGM2C_PATH) && nimble build

post-build:

gflash:	all
	@jflash $(BUILD_ROOT)/debug/$(TARGET).bin

flash: DEBUG := 0
flash: all
	@jflash $(BUILD_ROOT)/release/$(TARGET).bin

.PHONY: all clean pre-build post-build flash gflash rebuild
#.ONESHELL:

