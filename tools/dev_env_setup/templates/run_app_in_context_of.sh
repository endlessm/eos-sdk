#!/bin/bash
PID="$(pgrep -o -f -u $(id -u) $1)"
[ -z "$PID" ] && echo "No such process" > /dev/stderr && exit 1

eval `strings /proc/$PID/environ | while read line; do
   KEY="$(echo $line | sed -n 's/\([[:upper:]_]*\)=.*/\1/p')"
   VALUE="$(echo $line | sed -n 's/[[:upper:]_]*=\([^"]*\)/\1/p')"
   [ -z "$KEY" ] && continue;
   [ -z "$VALUE" ] && continue;
   echo "export $KEY='$VALUE'"
done`

echo "Now in same session as $PID. Running $2"
$2
