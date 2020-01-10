#
#    Copyright (C) 2017 Milan Kupcevic
#
#    You can redistribute and/or modify this software under the
#    terms of the GNU General Public License version 3, or any later
#    version as published by the Free Software Foundation.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#    GPLv3+
#

MCU = atmega328p
F_CPU = 16000000

AVR_DEBUG_FLAG = -gdwarf-2

CPPFLAGS = -pthread
LDFLAGS = -pthread
ADD_LIBS = -lutil

BINS = simulator

all: simulator

OBJS_simulator = simulator.o uart_stdinout.o

OBJS = $(OBJS_simulator) $(TARGETS)

simulator: $(OBJS_simulator)

.SUFFIXES:
.SUFFIXES: .c .o .avro .axf .hex

ifndef LIBS
LIBS = $(shell pkg-config --libs simavr)
endif

ifndef CFLAGS
CFLAGS = $(shell pkg-config --cflags simavr)
endif
CFLAGS += -I. -I../parts

ifndef AVR_LIBS
AVR_LIBS = $(shell pkg-config --libs simavr-avr)
endif

ifndef AVR_CFLAGS
AVR_CFLAGS = $(shell pkg-config --cflags simavr-avr)
endif
AVR_CFLAGS += -I. -I../shared -DF_CPU=$(F_CPU)

ifndef AVR_CC
AVR_CC = avr-gcc -Os -mmcu=$(MCU) $(AVR_DEBUG_FLAG) $(AVR_CC_FLAG)
endif

ifndef AVR_OBJCOPY
AVR_OBJCOPY = avr-objcopy
endif

ifndef STRIP
STRIP = strip
endif

ifndef AVR_STRIP
AVR_STRIP = avr-strip
endif

strip: bins-strip avr-bins-strip

bins-strip: $(BINS)
	$(STRIP) $^

avr-bins-strip: $(AVR_BINS)
	$(AVR_STRIP) $^

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BINS):
	$(CC) -o $@ $(LDFLAGS) $^ $(LIBS) $(ADD_LIBS)

.c.avro:
	$(AVR_CC) $(AVR_CPPFLAGS) $(AVR_CFLAGS) -c -o $@ $<

.axf.hex:
	$(AVR_OBJCOPY) -j .text -j .data -j .eeporm -O ihex $< $@

$(AVR_BINS):
	$(AVR_CC) -o $@ $(AVR_LDFLAGS) $^ $(AVR_LIBS) $(AVR_ADD_LIBS)

clean:
	rm -f -- $(BINS) $(AVR_BINS) $(OBJS)

