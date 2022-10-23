#
# Makefile for sample bbmachine program/ROM.
#
# The CC65 toolchain should be in $PATH.
#
CA=ca65
LD=ld65
CONFIG=bbmachine.cfg

COMMONOBJS=zeropage.o rom.o

all: helloworld.bin math.bin

helloworld.bin: $(COMMONOBJS) helloworld.o
	$(LD) $^ -o $@ -m $(basename $@).map -C $(CONFIG)

math.bin: $(COMMONOBJS) math.o
	$(LD) $^ -o $@ -m $(basename $@).map -C $(CONFIG)

.s.o:
	$(CA) -l $(basename $<).lst $<

.PHONY:
clean:
	rm -f *.o *.bin *.map *.lst *~
