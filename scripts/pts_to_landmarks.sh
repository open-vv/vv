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
  exit 1
fi

to_append=`mktemp`
to_prepend=`mktemp`
pts_file_spaced=`mktemp`

for i in $(seq 0 $((`cat $1 | wc -l` - 1)));
do
  echo "0 0" >> $to_append
  echo $i >> $to_prepend
done

echo "LANDMARKS1" > $2
cat $1 | sed -e 's/\s/ /g' >> $pts_file_spaced  
paste -d ' ' $to_prepend $pts_file_spaced $to_append >> $2

rm $to_append $to_prepend $pts_file_spaced
