HEX=fw.hex
OUT=fw.obj

MMCU=atmega32u2
F_CPU = 8000000

CFLAGS=-mmcu=$(MMCU) -Wall -Os -DF_CPU=$(F_CPU) -std=c99 --combine -fwhole-program  -mcall-prologues

$(HEX): $(OUT)
	avr-objcopy -O ihex $(OUT) $(HEX)
	avr-size --mcu=$(MMCU) --format=avr $(OUT)

#$(OUT): $(OBJECTS) *.c *.h sd-reader/*.c sd-reader/*.h
$(OUT): main.c leds.c leds.h encoder.c encoder.h serial.c serial.h
	avr-gcc $(CFLAGS) -o $(OUT) main.c leds.c encoder.c serial.c #*.c sd-reader/*.c

clean:
	rm $(OUT) $(HEX)
################################################################################

program: programmed
	touch programmed
	avrdude -p $(MMCU) -c usbtiny -U flash:w:$(HEX)

fuse:
	avrdude -p $(MMCU) -c usbtiny -U lfuse:w:0xde:m -U hfuse:w:0xd9:m -U efuse:w:0xf4:m

programmed: $(HEX)
