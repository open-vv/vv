#! /bin/sh

###############################################################################
#
# FILE: pts_to_landmarks
# AUTHOR: Vivien Delmon
#
# Conversion from landmarks in the format used in clitkCalculateTRE (.pts) 
# to the format used in VV (.txt).
#
###############################################################################

if [ $# -ne 2 ]; then
  echo "Usage: $0 input.pts output.txt" 1>&2
fi

to_append=/tmp/$RANDOM
to_prepend=/tmp/$RANDOM

for i in $(seq 0 $((`cat $1 | wc -l` - 1)));
do
  echo 0' '0 >> $to_append
  echo $i >> $to_prepend
done

echo "LANDMARKS1" > $2
paste -d ' ' $to_prepend $1 $to_append >> $2
