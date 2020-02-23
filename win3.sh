#!/bin/bash

cat bt.tmp | sed 's/\x00//g' > infob.tmp

two=$(cat infob.tmp | sed 's/\x00//g' | grep -n '(gdb)' | awk -F ':' '{print $1}' | tail -n 2 | head -n 1)
three=$(cat infob.tmp | sed 's/\x00//g' |grep -n '(gdb)' | awk -F ':' '{print $1}' | tail -n 1)

cat infob.tmp | sed 's/\x00//g' | tail -n $((three-two+1))
