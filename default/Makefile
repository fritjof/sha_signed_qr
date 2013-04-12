###############################################################################
# Makefile for the project lcd1
###############################################################################

## General Flags
PROJECT = lcd1
MCU = atmega328p
TARGET = qrauth.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99            -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=qrauth.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = qrauth.o qrframe.o qrencode.o i2c_phys.o sha204_comm.o sha204_comm_marshaling.o sha204_i2c.o timer_utilities.o base64_dec.o base64_enc.o qrv4l2.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) qrauth.hex qrauth.eep qrauth.lss size

## Compile
qrauth.o: ../qrauth.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

qrframe.o: ../qrframe.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

qrencode.o: ../qrencode.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

i2c_phys.o: ../i2c_phys.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sha204_comm.o: ../sha204_comm.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sha204_comm_marshaling.o: ../sha204_comm_marshaling.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sha204_i2c.o: ../sha204_i2c.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

timer_utilities.o: ../timer_utilities.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

base64_dec.o: ../base64_dec.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

base64_enc.o: ../base64_enc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

qrv4l2.o: ../qrv4l2.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) qrauth.elf dep/* qrauth.hex qrauth.eep qrauth.lss qrauth.map


## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

