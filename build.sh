#/usr/bin/env bash

gcc main.c lib/db.c utils/hashmap.c -I lib/ -g -o ./build/main
./build/main