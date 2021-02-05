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

BUILD_OBJDIR := $(BUILD_DIR)/obj
# Directory for auto-generated sources
BUILD_SRCDIR := $(BUILD_DIR)/src

PGM_DIR := Bitmaps/PGM

######################################
# source
######################################
# C sources
SOURCE_DIRS :=  Core Drivers FATFS Middlewares
C_SOURCES_EXCLUDE := Core/Src/freertos.c

# Preserve files not to be overwriten by CubeMX
OVERRIDE_C_SOURCES := FATFS/Target/ffconf.h

C_SOURCES = $(filter-out $(C_SOURCES_EXCLUDE),$(shell find $(SOURCE_DIRS) -name "*.c"))

# Auto generated sources
C_SOURCES += $(BUILD_SRCDIR)/bitmaps.c

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

PGM2C_SRCDIR = Tools/pgm2c
PGM2C = $(PGM2C_SRCDIR)/bin/pgm2c

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
C_INCLUDES = $(addprefix -I,\
	$(subst $(PWD)/,,$(realpath $(sort $(dir \
	$(shell find $(SOURCE_DIRS) -name "*.h"))))))

# Auto generated headers
C_INCLUDES += -I$(BUILD_SRCDIR)

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
# auto-generated sources and headers
$(BUILD_SRCDIR)/bitmaps.c: $(PGM2C) $(BUILD_DIR)
	$(PGM2C) -h:$(@:.c=.h) -c:$@ -p:$(PGM_DIR)

# list of objects
OBJECTS = $(addprefix $(BUILD_OBJDIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_OBJDIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_OBJDIR)/%.o: %.c %d Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_OBJDIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_OBJDIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

# $(LDSCRIPT)
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(LDSCRIPT): TrueSTUDIO/$(TARGET)/STM32L432KC_FLASH.ld
	@cp -f $< $@

$(BUILD_DIR):
	@mkdir -p $(BUILD_OBJDIR) $(BUILD_SRCDIR)

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
pre-build: $(BUILD_SRCDIR)/bitmaps.c
	@grep -q '^#include "cmsis_os.h"' $(MAIN_C) && sed -i \
		-e 's@^#include "cmsis_os.h"@/* & */@;' \
		-e 's@^ \+MX_FREERTOS_Init();@/* & */@' \
		-e 's@^ \+osKernelStart();@/* & */@' \
		-e 's@^ \+osKernelInitialize();@/* & */@' \
		-e 's@^void MX_FREERTOS_Init(void);@/* & */@' $(MAIN_C) || true
	@for to in $(OVERRIDE_C_SOURCES); do \
		from="`dirname $$to`/override-`basename $$to`";\
		cmp -s $$from $$to || cp -f $$from $$to; done

$(PGM2C): $(PGM2C_SRCDIR)/src/pgm2c.nim
	@cd $(PGM2C_SRCDIR) && nimble build

post-build:

gflash:	all
	@jflash $(BUILD_ROOT)/debug/$(TARGET).bin

flash: DEBUG := 0
flash: all
	@jflash $(BUILD_ROOT)/release/$(TARGET).bin

.PHONY: all clean pre-build post-build flash gflash rebuild
#.ONESHELL:
#$(BUILD_DIR)/Core/Src/bitmaps.c $(BUILD_DIR)/Core/Src/bitmaps.h


aa:
	@echo $(OBJECTS)
#######################################
# dependencies
#######################################
include $(wildcard $(BUILD_OBJDIR)/*.d)