#!/bin/sh
#

function build {
    NAME=$1
    shift
    FILES=$*
    CONFIG=bbmachine.cfg
    echo "*** Building $NAME from $FILES"
    ca65 -l $NAME.lst $FILES && \
    ld65 $NAME.o -o $NAME.bin -m $NAME.map -C $CONFIG
}


build helloworld helloworld.s


