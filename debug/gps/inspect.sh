#!/bin/sh

# $ ./inspect.sh ./data7/*.bin > results.dat 

if [ $# -eq 0 ] then
 echo "No arguments supplied"
 exit 1
fi

for i in `ls -v $*`; do ruby inspect.rb $i; done;
