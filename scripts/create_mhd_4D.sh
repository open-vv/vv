#!/bin/sh


write_mhd_4D()
{
  cat $orig | sed "s/NDims = .*/NDims = 4/
		  s/TransformMatrix = .*/TransformMatrix = 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1/
		  /Offset/ s/.*/& 0/
		  /CenterOfRotation/ s/.*/& 0/
		  s/AnatomicalOrientation = .*/AnatomicalOrientation = ????/
		  /ElementSpacing/ s/.*/& 1/
		  /DimSize/ s/.*/& $nbph/
		  s/ElementDataFile = .*/ElementDataFile = LIST/" > "$1/$file_name_4D"
  
  for ph in $listph
  do
    ph=`basename $ph`
    echo "$ph" >> "$1/$file_name_4D"
  done
}

#################################################
# create_mhd_4D 	argument : repertoire	#
#################################################
if [ $# -lt 1 ]
then
  echo "Usage: create_mhd_4D.sh DIRECTORY"
  exit 1
fi

nb_phase_file=`find $1 -maxdepth 1 -iname "*[0-9]*.mhd" -o -iname "*[0-9]*\]*.mhd" | wc -l`
if [ $nb_phase_file = 0 ]
then
  echo "Error: no phase found"
  exit 1
fi


list_prefix=""
list_phase_file=`find $1 -maxdepth 1 -iname "*[0-9]*.mhd"`
for phase_file in $list_phase_file
do
  phase_file_name=`basename $phase_file`
  if [[ ! -z `echo "$phase_file_name" | grep "__[0-9]"` ]]
  then
    prefix=`echo $phase_file_name | sed "s/__[0-9].*/__/"`
  else
    if [[ ! -z `echo "$phase_file_name" | grep "[0-9]-.*\]"` ]]
    then
      if [[ ! -z `echo "$phase_file_name" | grep "[0-9][0-9]-.*\]"` ]]
      then
	prefix=`echo $phase_file_name | sed "s/[0-9][0-9]-.*//"`
      else
	prefix=`echo $phase_file_name | sed "s/[0-9]-.*//"`
      fi
    else
      prefix="NONE"
    fi
  fi

  if [[ -z `echo "$list_prefix" | grep "$prefix"` ]]
  then
    list_prefix="$list_prefix $prefix"
  fi
done


for prefix in $list_prefix
do
  if [ "$prefix" = "NONE" ]
  then
    prefix=""
  fi

  list_suffix=""
  list_phase_file_prefix=`find $1 -maxdepth 1 -iname "${prefix}[0-9]*.mhd"`
  for phase_file_prefix in $list_phase_file_prefix
  do
    phase_file_prefix_name=`basename $phase_file_prefix`
    if [[ ! -z `echo "$phase_file_prefix_name" | grep "__[0-9]"` ]]
    then
      suffix="NONE"
    else
      if [[ ! -z `echo "$phase_file_prefix_name" | grep "[0-9]-.*\]"` ]]
      then
	suffix=`echo $phase_file_prefix_name | sed "s/.*[0-9]-//;s/_\.mhd//;s/\.mhd//"`
      else
	suffix="NONE"
      fi
    fi

    if [[ -z `echo "$list_suffix" | grep "$suffix"` ]]
    then
      list_suffix="$list_suffix $suffix"
    fi
  done

  for suffix in $list_suffix
  do
    if [ "$suffix" = "NONE" ]
    then
      suffix=""
    fi
    nbph=`find $1 -maxdepth 1 -iname "*${prefix}*[0-9]*${suffix}*.mhd" | wc -l`
    orig=`find $1 -maxdepth 1 -iname "*${prefix}*[0-9]*${suffix}*.mhd" | sort | head -n 1`
    listph=`find $1 -maxdepth 1 -iname "*${prefix}*[0-9]*${suffix}*.raw" | sort`

    file_name_4D="${prefix}4D${suffix}.mhd"
    
    write_mhd_4D $1

  done

done

