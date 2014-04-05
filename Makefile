MCU = atmega32u4
ARCH = AVR8
F_CPU = 8000000
F_USB = $(F_CPU)
OPTIMIZATION = s
TARGET = fw
SRC = encoder.c leds.c main.c mp3.c player.c sd.c serial.c tenths.c \
	  sd-reader/byteordering.c sd-reader/fat.c sd-reader/partition.c \
	  sd-reader/sd_raw.c sd-reader/sd_manager.c \
	  mass-storage/descriptors.c mass-storage/mass_storage.c \
	  mass-storage/scsi.c $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS)

LUFA_PATH = lufa/LUFA
CC_FLAGS = -DUSE_LUFA_CONFIG_HEADER -Imass-storage

all:

include $(LUFA_PATH)/Build/lufa_core.mk
include $(LUFA_PATH)/Build/lufa_sources.mk
include $(LUFA_PATH)/Build/lufa_build.mk

dfu: all
	dfu-programmer $(MCU) erase
	dfu-programmer $(MCU) flash --suppress-bootloader-mem $(TARGET).hex

fuse:
	avrdude -p $(MCU) -c usbtiny -U lfuse:w:0xde:m -U hfuse:w:0xd9:m -U efuse:w:0xfb:m
