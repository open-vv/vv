#! /bin/sh -x

###############################################################################
#
# FILE: midp_common.sh
# AUTHOR: Rômulo Pinho 05/08/2011
#
# Helper file with many functions used in the midP scripts.
#
###############################################################################

#
# check return value passed and abort if it represents an error (ie, ret != 0)
# optionally, a function can be passed as a 3rd parameter, to be called just
# before exiting. this is useful for cleaning up, for example.
#
abort_on_error()
{
  if [ $2 != 0 ]; then
    echo Aborted at $1 with code $2
    if [ $# = 3 ]; then
      eval $3
    fi

    exit $2
  fi
}

#
# wait for all processes in the list and return their exit codes
# in the ret_codes variable.
#
# OBS: this function must always be called in the same shell
# that launched the processes.
#
wait_pids()
{
  local pids=$*
  local ret=
  local rets=
#   echo PIDS: $pids
  for p in $pids; do
#     echo waiting $p
    wait $p > /dev/null 2> /dev/null
    ret=$?
    if [ ret != 127 ]; then
      rets=$rets" "$ret
    else
      rets=$rets" "0
    fi
      
  done

  ret_codes=$rets
}

#
# clean-up functions for maks, registration, and midp
#
clean_up_masks()
{
  rm -fr $mask_dir_tmp
}

clean_up_midp()
{
  rm -fr $midp_dir
}

clean_up_registration()
{
  rm -fr $vf_dir
  rm -fr $output_dir
}


#
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

#
# replacement for clitkCombineImage
combine_image()
{
#  eg: -i $result_in -j $result_out -o $out_result -m $motion_mask

  clitkSetBackground -i $1 -o temp1.mhd -m $4
  clitkSetBackground -i $2 -o temp2.mhd -m $4 --fg

  clitkImageArithm -i temp1.mhd -j temp2.mhd -o $3
  rm temp?.*
}

# 
# replacement for clitkAverageTemporalDimension
average_temporal_dimension()
{
  # eg: -i $midp_dir/midp_4D.mhd -o $midp_dir/midp_avg.mhd

  local tot=tot.mhd

  local dir=`dirname $1` 
  local first=`grep raw $1 | sed 's/raw/mhd/g' | head -n 1`
  clitkImageArithm -i $dir/$first -o $tot -t 1 -s 0

  local nbphases=`grep raw $1 | sed 's/raw/mhd/g' | wc -l`
  for i in $(grep raw $1 | sed 's/raw/mhd/g'); do
    clitkImageArithm -i $dir/$i -j $tot -o $tot
  done

  clitkImageArithm -i $tot -o $2 -t 11 -s $nbphases
  rm tot.*
}
