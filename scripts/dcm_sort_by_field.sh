#!/bin/bash
if [ $# -lt 1 ]
then
    echo Usage: dcm_sort_by_field.sh \"field name\"
    exit
fi
path_dir=`pwd`
dir_name=`basename "$path_dir"`
[ -d "../${dir_name}_sorted" ] && rm -r "../${dir_name}_sorted"
mkdir "../${dir_name}_sorted"
find . -type f -iname "*.dcm" > ../${dir_name}_sorted/dicom_files
finished=0
total=$(wc -l ../${dir_name}_sorted/dicom_files)
cat ../${dir_name}_sorted/dicom_files | while :
do
    jobrunning=0
    conc_jobs=50
    while [ $jobrunning -lt $conc_jobs ]
    do
        read i || { wait; break 2; }
        {
            name="$(clitkDicomInfo "$i" | grep "$1" | head -n 1 | sed "s/.*\[//;s/.$//;s/ /_/g")"
	    [ -z "$name" ] && echo "Warning: key not found in file $i" 1>&2 && mkdir -p "../${dir_name}_unsorted" && basename=$(basename "$i") && cp -l "$i" "../${dir_name}_unsorted/$basename" && exit 1 #copy to unsorted directory if dicom key not found
            #[ -z "$name" ] && echo "Warning: key not found in file $i" 1>&2 && exit 1 #don't do anything if dicom key not found
            name=`echo "$name" | sed 's/\//_/g'`
	    name=`echo "$name" | sed 's/\\\/_/g'`
	    name=`echo "$name" | sed 's/\*/_/g'`
	    [ -d "../${dir_name}_sorted/$name" ] || mkdir "../${dir_name}_sorted/$name" 2>>/dev/null
            basename=$(basename "$i")
            cp -l "$i" "../${dir_name}_sorted/$name/$basename"
        }&
        jobrunning=$(( $jobrunning + 1 ))
    done
    finished=$(( $finished + $conc_jobs  ))
    echo -e -n "( $finished / $total )\r"
    wait
done
rm ../${dir_name}_sorted/dicom_files
