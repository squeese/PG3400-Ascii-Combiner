#!/bin/bash
rm -f log && \
 make clean && \
 make && \
 valgrind --leak-check=yes --track-origins=yes --log-file="log" \
 ./combine wolf out.txt

