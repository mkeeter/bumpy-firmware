HEX=fw.hex
OUT=fw.obj

MMCU=atmega32u4
F_CPU = 8000000

CFLAGS=-mmcu=$(MMCU) -Wall -Os -DF_CPU=$(F_CPU) -std=c99 --combine -fwhole-program  -mcall-prologues

$(HEX): $(OUT)
	avr-objcopy -O ihex $(OUT) $(HEX)
	avr-size --mcu=$(MMCU) --format=avr $(OUT)

$(OUT): $(OBJECTS) *.c *.h sd-reader/*.c sd-reader/*.h
	avr-gcc $(CFLAGS) -o $(OUT) *.c sd-reader/*.c

clean:
	rm $(OUT) $(HEX)
################################################################################

program: programmed
	touch programmed
	avrdude -p $(MMCU) -c usbtiny -U flash:w:$(HEX)

fuse:
	avrdude -p $(MMCU) -c usbtiny -U lfuse:w:0xDE:m -U hfuse:w:0xD9:m

programmed: $(HEX)
