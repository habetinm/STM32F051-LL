TARGET = f051-ll

CUBE = $(HOME)/STM32/STM32CubeF0
CMSIS = $(CUBE)/Drivers/CMSIS
DEVICE = $(CMSIS)/Device/ST/STM32F0xx
LL = $(CUBE)/Drivers/STM32F0xx_HAL_Driver

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CPU = -mcpu=cortex-m0 -mthumb

CFLAGS = $(CPU) \
         -DSTM32F051x8 \
         -DUSE_FULL_LL_DRIVER \
         -O0 -g3 \
         -Wall -Wextra \
         -ffunction-sections \
         -fdata-sections \
         -std=c11

INCLUDES = \
    -I$(CMSIS)/Include \
    -I$(DEVICE)/Include \
    -I$(LL)/Inc

LDFLAGS = $(CPU) \
          -Tstm32f051x8.ld \
          -Wl,--gc-sections \
          -Wl,-Map=build/$(TARGET).map \
          --specs=nosys.specs

SOURCES = \
    src/main.c \
    $(DEVICE)/Source/Templates/system_stm32f0xx.c \
    $(DEVICE)/Source/Templates/gcc/startup_stm32f051x8.s

OBJECTS = $(patsubst %.c,build/%.o,$(filter %.c,$(SOURCES))) \
          build/startup_stm32f051x8.o

all: build/$(TARGET).elf build/$(TARGET).bin

build/$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@
	$(SIZE) $@

build/$(TARGET).bin: build/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

build/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

build/startup_stm32f051x8.o: $(DEVICE)/Source/Templates/gcc/startup_stm32f051x8.s
	mkdir -p build
	$(CC) $(CPU) -c $< -o $@

load:
	openocd -f interface/stlink.cfg -f target/stm32f0x.cfg -c "program build/f051-ll.elf verify reset exit"

clean:
	rm -rf build

.PHONY: all clean
