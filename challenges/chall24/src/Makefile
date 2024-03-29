LINTFLAGS := -sys-dirs /usr/include/sys/:/opt/local/avr/include/:/usr/lib/avr/include -unrecogcomments  -standard -isoreserved +charintliteral
DEVICE     = atmega328p
CLOCK      = 16000000
PROGRAMMER = -B 5 -c usbasp -F
LINTOBJECTS  = main.o uart.o

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) \
	 -ffunction-sections -Xlinker -gc-sections \
	 -fomit-frame-pointer -fno-inline -g \
	 -Wl,--defsym=__heap_end=0x8008ff



# symbolic targets:
all:	chall24.hex flag.hex debugversion.x86 chall24.bin

%.bin : %.elf
	avr-objcopy -O binary $< $@

%.hex : %.txt
	avr-objcopy --change-addresses +0x0810000 -I binary -O ihex $< $@

vpath %.c ../

debugversion.x86 : main.c
	gcc -Wall $< -o $@

.PHONY:
lint: $(LINTOBJECTS:.o=.c)
	splint -DF_CPU=$(CLOCK) -DAVR -D__AVR_ATmega328P__ -I /usr/lib/avr/include -I /opt/local/avr/include/  +posixlib $(LINTFLAGS) $^

.c.o:
	$(COMPILE) -c $< -o $@

.PHONY:
flash_%: %.hex
	avrdude -p m328p -P /dev/ttyACM0 -c arduino -U flash:w:$<

flash: flash_chall24

fuse:
	$(AVRDUDE) $(FUSES)

# Xcode uses the Makefile targets "", "clean" and "install"
install: flash fuse

clean:
	rm -f *.o *.hex *.elf *.bin *.x86


# file targets:
.SECONDARY:
%.elf : $(LINTOBJECTS)
	$(COMPILE) -o $@ $^

%.hex: %.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex $< tmp.hex
	avr-objcopy -I ihex -O ihex tmp.hex $@
	rm tmp.hex
	avr-size $<
