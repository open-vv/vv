#!/bin/bash
#Create a sequence of mhd images from the various UNTAGGED directories
if [ $# -lt 1 ]
then
    echo "Usage: create_sequence.sh prefix"
    exit 1
fi
for i in $1*
do
    filename=CT_$1_$(echo "$i" | sed "s/.*,_//;s/\..*//").mhd
    echo $filename
    find "$i" -iname "*.dcm" | clitkDicom2Image --focal_origin -o "$filename" --std_input
done

create_mhd_4D.sh . "CT_4D.mhd"
