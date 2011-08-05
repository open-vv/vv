#! /bin/sh +x

###############################################################################
#
# FILE: common.sh
# AUTHOR: Rômulo Pinho 05/08/2011
#
# Helper file with many functions used in the midP scripts.
#
###############################################################################


# block execution untill the number of threads (jobs) launched by the
# current process is below the given number of threads. 
MAX_THREADS=2
check_threads()
{
  nbth=$1
  while [[ $(jobs -p | wc -l) -ge $nbth ]]; do
      jobs
      sleep 10
  done
}

#
# receive a 4D file and and extract the corresponding phase numbers
# export the variables containing each of the extracted data
#
extract_4d_phase_numbers()
{
  mhd4d=$1

  nb_phases=${#phase_files[@]}

  # get everything except numbers and punctuation
  cat $mhd4d | grep ".raw" | sed 's:.raw:.mhd:' | sed 's/.mhd//' | grep -o "[^0-9[:punct:]]*" | sort -u > /tmp/patterns.txt

  # find which patterns have the phases connected to it
  patterns=`cat /tmp/patterns.txt`
  if [ -z "$patterns" ]; then
    phase_nbs=( `cat $mhd4d | grep ".raw" | sed 's:.raw:.mhd:' | sed 's/.mhd//' | grep "[0-9]\+"` )
  else
    for i in $patterns; do 

      # check if the pattern appears before the phase number
      nb_phases_found=`cat $mhd4d | grep ".raw" | sed 's:.raw:.mhd:' | sed 's/.mhd//' | grep -o "$i[0-9[:punct:]]\+" | sort -u | wc -l`
      if [ $nb_phases_found == $nb_phases ]; then
        # keep only what identifies the phase number
        phase_nbs=( `cat $mhd4d | grep ".raw" | sed 's:.raw:.mhd:' | sed 's/.mhd//' | grep -o "$i[0-9[:punct:]]\+" | grep -o "[^${i}]\+" | grep -o "[0-9]\+[[:punct:]]*[0-9]*" | grep -o "[0-9]*[[:punct:]]*[0-9]\+"` ) 
        break
      fi
    
      # check if the pattern appears after the phase number
      nb_phases_found=`cat $mhd4d | grep ".raw" | sed 's:.raw:.mhd:' | sed 's/.mhd//' | grep -o "[0-9[:punct:]]\+$i" | sort -u | wc -l`
      if [ $nb_phases_found == $nb_phases ]; then
        # keep only what identifies the phase number
        phase_nbs=( `cat $mhd4d | grep ".raw" | sed 's:.raw:.mhd:' | sed 's/.mhd//' | grep -o "[0-9[:punct:]]\+$i" | grep -o "[^${i}]\+" | grep -o "[0-9]\+[[:punct:]]*[0-9]*" | grep -o "[0-9]*[[:punct:]]*[0-9]\+"` ) 
        break
      fi

    done
  fi

  echo "Phase numbers are ${phase_nbs[@]}"
  rm /tmp/patterns.txt
}

#
# receive a 4D file and extract the corresponding phase files, 
# and phase numbers.
# export the variables containing each of the extracted data
#
extract_4d_phases()
{
  mhd4d=$1

  echo "4D file is $mhd4d"

  # array of phase files
  phase_files=( `cat $mhd4d | grep ".raw" | sed 's:.raw:.mhd:'` )
  echo "Phase files are ${phase_files[@]}"

  extract_4d_phase_numbers $mhd4d 
}


#
# receive a 4D file and the reference phase number as input 
# and extract the corresponding phase files, phase numbers, 
# and reference phase file. 
#
# export the variables containing each of the extracted data
#
extract_4d_phases_ref()
{
  extract_4d_phases $1

  # reference phase file
  ref_phase_file=`cat $mhd4d | grep ".raw" | sed 's:.raw:.mhd:' | grep $2`
  echo "Reference phase is $ref_phase_file"

  # reference phase number
  for i in $( seq 0 $((${#phase_nbs[@]} - 1))); do
    ref_phase_nb=`echo ${phase_nbs[$i]} | grep $2`
    if [ -n "$ref_phase_nb" ]; then
      echo "Reference phase number is $ref_phase_nb"
      break
    fi
  done
}
