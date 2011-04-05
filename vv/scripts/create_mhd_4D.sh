#!/bin/sh


#################################################
# create_mhd_4D 	argument : repertoire	#
#################################################
if [ $# -lt 1 ]
then
  echo "Usage: create_mhd_4D.sh DIRECTORY"
  exit 1
fi

nb_phase_file=`find $1 -iname "*[0-99].[0-9].mhd" | wc -l`
if [ $nb_phase_file = 0 ]
then
  echo "Error: no phase found"
  exit 1
fi

list_pattern=""
list_phase_file=`find $1 -iname "*[0-99].[0-9].mhd"`
for phase_file in $list_phase_file
do
  phase_file_name=`basename $phase_file`
  if [[ ! -z `echo "$phase_file_name" | grep ","` ]]
  then
    preffix=`echo $phase_file_name | sed "s/,_.*/,_/"`
  else
    preffix="NONE"
  fi
  if [[ -z `echo "$list_pattern" | grep "$preffix"` ]]
  then
    list_pattern="$list_pattern $preffix"
  fi
done


for pattern in $list_pattern
do

  if [ "$pattern" = "NONE" ]
  then
    pattern=""
  fi

  nbph0=`find $1 -iname "${pattern}[0-9].[0-9].mhd" | wc -l`
  orig0=`find $1 -iname "${pattern}[0-9].[0-9].mhd" | sort | head -n 1`
  listph0=`find $1 -iname "${pattern}[0-9].[0-9].raw" | sort`

  nbph1=`find $1 -iname "${pattern}[0-9][0-9].[0-9].mhd" | wc -l`
  orig1=`find $1 -iname "${pattern}[0-9][0-9].[0-9].mhd" | sort | head -n 1`
  listph1=`find $1 -iname "${pattern}[0-9][0-9].[0-9].raw" | sort`

  nbph=$(( nbph0+nbph1 ))
  orig=`echo -e "$orig0\n$orig1" | head -n 1`
  listph="$listph0 $listph1"

  cat $orig | sed "s/NDims = .*/NDims = 4/
		  s/TransformMatrix = .*/TransformMatrix = 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1/
		  /Offset/ s/.*/& 0/
		  /CenterOfRotation/ s/.*/& 0/
		  s/AnatomicalOrientation = .*/AnatomicalOrientation = ????/
		  /ElementSpacing/ s/.*/& 1/
		  /DimSize/ s/.*/& $nbph/
		  s/ElementDataFile = .*/ElementDataFile = LIST/" > "$1/${pattern}_4D.mhd"
  
  for ph in $listph
  do
    ph=`basename $ph`
    echo "$ph" >> "$1/${pattern}_4D.mhd"
  done

done
