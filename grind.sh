#!/bin/bash
rm -f grind.log && \
 make clean && \
 make && \
 valgrind --leak-check=yes --track-origins=yes --log-file="grind.log" \
 ./combine $1 $2

