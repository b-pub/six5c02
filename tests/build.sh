#!/bin/sh
#

function build {
    NAME=$1
    CONFIG=config.cfg
    if [ $# = 2 ] ; then
        CONFIG=$2
    fi
    echo "*** Building $NAME"
    ca65 -l $NAME.lst $NAME.s && \
    ld65 $NAME.o -o $NAME.bin -m $NAME.map -C $CONFIG
}

build functional_test
build decimal_test
