#!/bin/bash
###
# Requires Linux, Bash
###

#get name of current xterm emulator
XTERM=$(ps -p $(cat /proc/$PPID/stat | awk -F ' ' '{print $4}') | awk -F ' ' 'NR==2 {print $4}')
#gnome-terminal shows up as 'gnome-terminal-' for whatever reason
XTERM=${XTERM%-}
winName=${0#./}
winName=${winName%.*}
tmp=${1#./}
tmp=${tmp%.*}
winName="${winName}_${tmp}"
succeed=1
#gnome-terminal is deprecating -e
if [ "$XTERM" = "gnome-terminal" ]; then
    $XTERM -- screen -S $winName gdb $1 2>&1 > /dev/null &&\
    sleep 1 &&\
    screen -S $winName -p 0 -X split -v
    succeed=$?
else
    $XTERM -e "screen -S $winName gdb $1" 2>&1 > /dev/null &&\
    sleep 1 &&\
    screen -S $winName -p 0 -X split -v
    succeed=$?
fi

if [ "$succeed" = "0" ]; then
    screen -S $winName -p 0 -X split &&\
    screen -S $winName -p 0 -X resize 90% &&\
    screen -S $winName -p 0 -X focus down &&\
    screen -S $winName -p 0 -X screen top &&\
    screen -S $winName -p 0 -X focus right &&\
    screen -S $winName -p 0 -X split &&\
    screen -S $winName -p 0 -X screen gdb &&\
    screen -S $winName -p 0 -X resize 70% &&\
    screen -S $winName -p 0 -X focus down &&\
    screen -S $winName -p 0 -X screen gdb
else
    echo "failed for some reason"
fi
