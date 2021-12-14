#
# Makefile for sample bbmachine program/ROM.
#
# The CC65 toolchain should be in $PATH.
#
CA=ca65
LD=ld65
CONFIG=bbmachine.cfg

helloworld: zeropage.o rom.o helloworld.o
	$(LD) $^ -o $@.bin -m $@.map -C $(CONFIG)

.s.o:
	$(CA) -l $(basename $<).lst $<

clean:
	rm -f *.o *.bin *.map *.lst *~
