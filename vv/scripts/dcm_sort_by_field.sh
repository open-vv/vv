#!/bin/bash
if [ $# -lt 1 ]
then
    echo Usage: dcm_sort_by_field.sh \"field name\"
    exit
fi
[ -d sorted ] && rm -r sorted
find . -type f -iname "*.dcm" > dicom_files
finished=0
total=$(wc -l dicom_files)
mkdir sorted
cat dicom_files | while :
do
    jobrunning=0
    conc_jobs=50
    while [ $jobrunning -lt $conc_jobs ]
    do
        read i || { wait; break 2; }
        {
            name="$(clitkDicomInfo "$i" | grep "$1" | head -n 1 | sed "s/.*\[//;s/.$//;s/ /_/g")"
            [ -z "$name" ] && echo "Warning: key not found in file $i" 1>&2 && exit 1 #don't do anything if dicom key not found
            name=`echo "$name" | sed 's/\//_/g'`
	    name=`echo "$name" | sed 's/\\\/_/g'`
	    name=`echo "$name" | sed 's/\*/_/g'`
	    [ -d "sorted/$name" ] || mkdir "sorted/$name" 2>>/dev/null
            basename=$(basename "$i")
            cp -l "$i" "sorted/$name/$basename"
        }&
        jobrunning=$(( $jobrunning + 1 ))
    done
    finished=$(( $finished + $conc_jobs  ))
    echo -e -n "( $finished / $total )\r"
    wait
done
rm dicom_files
