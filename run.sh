#!/bin/sh
set -e

FILE="./MAIN.o"

if [ -f ${FILE} ]; then
    make clean
fi

clear
make
./MAIN > out.txt

echo ""
echo "-------------------------------------------------"
echo "              RESULTS:"
echo "-------------------------------------------------"

valgrind -v --leak-check=full --log-file=memraw.log ./MAIN

echo "-------------------------------------------------\n"
