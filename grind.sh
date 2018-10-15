#!/bin/bash
rm -f grind.log && \
 make clean && \
 make && \
 valgrind \
 --leak-check=yes \
 --track-origins=yes \
 ./combine $1 $2
# --log-file="grind.log" \

