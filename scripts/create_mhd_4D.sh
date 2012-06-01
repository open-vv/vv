#!/bin/sh +x


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
  
  for ph in ${listph[@]}
  do
    ph=`basename $ph`
    echo "$ph" >> "$1/$file_name_4D"
  done
  echo $1/$file_name_4D
}

#################################################
# create_mhd_4D 	argument : repertoire	#
#################################################
if [ $# -lt 1 ]
then
  echo "Usage: create_mhd_4D.sh DIRECTORY"
  exit 1
fi

nb_phase_file=`find $1 -maxdepth 1 -iname "*[0-9]*.mhd" | wc -l`
if [ $nb_phase_file = 0 ]
then
  echo "No phase found in $1"
  exit 1
fi


list_prefix=""
list_phase_file=`find $1 -maxdepth 1 -iname "*[0-9]*.mhd"`
for phase_file in $list_phase_file
do
  if grep -q "NDims = 4" $phase_file 2> /dev/null
  then
    #echo $phase_file is 4D
    continue;
  fi;

  phase_file_name=`basename $phase_file .mhd`
#   if [[ ! -z `echo "$phase_file_name" | grep "__[0-9]"` ]]
#   then
#     prefix=`echo $phase_file_name | sed "s/__[0-9].*/__/"`
#   else
#     if [[ ! -z `echo "$phase_file_name" | grep "[0-9]-.*\]"` ]] 
#     then
#       if [[ ! -z `echo "$phase_file_name" | grep "[0-9][0-9]-.*\]"` ]]
#       then
# 	prefix=`echo $phase_file_name | sed "s/[0-9][0-9]-.*//"`
#       else
# 	prefix=`echo $phase_file_name | sed "s/[0-9]-.*//"`
#       fi
#     else
#       prefix="NONE"
#     fi
#   fi

  # preffix: grep sequence of characters followed by sequence of numbers and remove sequence of numbers
  # prefix=`echo $phase_file_name | grep -o "\(^[[:alpha:][:punct:]]*\)\([[:digit:]\.\_\-]\+\)" | sed 's/\(^[[:alpha:][:punct:]]*\)\([[:digit:]\.\_\-]\+\)/\1/'`
  prefix=`echo $phase_file_name | grep -o "\(^[[:alpha:][:punct:]]*\)\([[:digit:]]\+\)\([\.\_\-]*\)"`
  prefix=`echo $prefix | sed 's/^\-\+//' | sed 's/\(^[[:alpha:][:punct:]]*\)\([[:digit:]]\+\)\([\.\_\-]*\)/\1/'`
  if [[ -z $prefix ]]
  then 
    prefix="NONE"
  fi

  # register new preffix if not yet done
  if [[ -z `echo "$list_prefix" | grep -w -- "$prefix"` ]]
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
    if grep -q "NDims = 4" $phase_file_prefix  2> /dev/null
    then
      #echo $phase_file_prefix is 4D
      continue;
    fi;
    phase_file_prefix_name=`basename $phase_file_prefix .mhd`
#     if [[ ! -z `echo "$phase_file_prefix_name" | grep "__[0-9]"` ]]
#     then
#       suffix="NONE"
#     else
#       if [[ ! -z `echo "$phase_file_prefix_name" | grep "[0-9]-.*\]"` ]]
#       then
# 	suffix=`echo $phase_file_prefix_name | sed "s/.*[0-9]-//;s/_\.mhd//;s/\.mhd//"`
#       else
# 	suffix="NONE"
#       fi
#     fi
# 

    # suffix: grep sequence of numbers followed by sequence of characters and remove sequence of numbers
    # suffix=`echo $phase_file_prefix_name | grep -o "\([[:digit:]\.\_\-]\+\)\([[:alpha:][:punct:]]*$\)" | sed 's/\([[:digit:]\.\_\-]\+\)\([[:alpha:][:punct:]]*$\)/\2/'`
    suffix=`echo $phase_file_prefix_name | grep -o "\([\.\_\-]*\)\([[:digit:]]\+\)\([[:alpha:][:punct:]]*$\)"`
    if ! echo $suffix | grep -qo "^\-\+"; then
      suffix=`echo $suffix | sed 's/\([\.\_\-]*\)\([[:digit:]]\+\)\([[:alpha:][:punct:]]*$\)/\3/'`
    fi
    suffix=`echo $suffix | sed 's/[\.\_\-]\+$//'`
    if [[ -z $suffix ]]
    then 
      suffix="NONE"
    fi

    # register new suffix if not yet done
    if [[ -z `echo "$list_suffix" | grep -w -- "$suffix"` ]]
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
    orig=`find $1 -maxdepth 1 -iname "${prefix}[0-9]*${suffix}*.mhd" | grep "${prefix}[[:digit:][:punct:]]\+${suffix}[\.\_\-]*.mhd" | sort | head -n 1`
    listph=( `find $1 -maxdepth 1 -iname "${prefix}[0-9]*${suffix}*.*raw" | grep "${prefix}[[:digit:][:punct:]]\+${suffix}[\.\_\-]*.z*raw" | sort` )
    nbph=${#listph[@]}

    # only create 4D file if potential number of phases is > 1
    if [ $nbph -gt 1 ]; then
      file_name_4D="${prefix}4D${suffix}.mhd"
      write_mhd_4D $1
    fi

  done

done

