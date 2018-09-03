#!/bin/sh
# This is adapter from Rob Pike.

filter()
{
   read p || exit
   echo $p
   while read x; do
        if [ `expr $x % $p` != 0 ]; then
            echo $x
        fi
    done | filter
}

filter
# seq 2 100 | filter
