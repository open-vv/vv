#! /bin/bash 
  
###############################################################################
#
# FILE: create_midP-2.0.sh
# AUTHOR: Rômulo Pinho 05/08/2011
#
# Version 2.0 of the create_midP_masks.sh script. The most relevant changes are:
#   * creation of bands around input and output image regions to try to improve 
#   the registration along lung boundaries (naturally, it depends on the quality
#   of motion mask generation).
#   * some steps are now in different modules, to facilitate re-use (see "includes" section).
#   * minor modifications on output file names.
#   * attempt to simplify the code a bit.
#
###############################################################################

source `dirname $0`/midp_common.sh

extract_patient()
{
  echo "$phase_file -> Extracting patient..."
  clitkExtractPatient -i $phase_file -o $mask_dir_tmp/patient_mask_$phase_nb.mhd --noAutoCrop -a $afdb_file $ExtractPatientExtra
#   abort_on_error clitkExtractPatient $?

  clitkSetBackground -i $phase_file -o $mask_dir_tmp/patient_$phase_nb.mhd --mask $mask_dir_tmp/patient_mask_$phase_nb.mhd --outsideValue -1000
#   abort_on_error clitkSetBackground $?
}

extract_bones()
{
  if [ x = x$ExtractBonesLower1 ]; then
    ExtractBonesLower1=120
  fi
  if [ x = x$ExtractBonesLower2 ]; then
    ExtractBonesLower2=80
  fi
  echo "$phase_file -> Extracting bones..."
  clitkImageConvert -i $phase_file -o $mask_dir_tmp/float_$phase_nb.mhd -t float
  clitkExtractBones -i $mask_dir_tmp/float_$phase_nb.mhd -o $mask_dir_tmp/bones_$phase_nb.mhd -a $afdb_file --lower1 $ExtractBonesLower1 --upper1 2000 --lower2 $ExtractBonesLower2 --upper2 2000 --smooth --time 0.0625 --noAutoCrop
}

extract_lungs()
{
  echo "$phase_file -> Extracting lungs..."  
  clitkExtractLung -i $phase_file -o $mask_dir_tmp/lungs_$phase_nb.mhd -a $afdb_file --noAutoCrop --doNotSeparateLungs
}



resample()
{
  echo "$phase_file -> Resampling..."
  clitkResampleImage -i $mask_dir_tmp/patient_$phase_nb.mhd -o $mask_dir_tmp/patient_$phase_nb.mhd --spacing $resample_spacing --interp $resample_algo
  clitkResampleImage -i $mask_dir_tmp/patient_mask_$phase_nb.mhd -o $mask_dir_tmp/patient_mask_$phase_nb.mhd --spacing $resample_spacing --interp $resample_algo
  clitkResampleImage -i $mask_dir_tmp/lungs_$phase_nb.mhd -o $mask_dir_tmp/lungs_$phase_nb.mhd --like $mask_dir_tmp/patient_$phase_nb.mhd
}

compute_motion_mask()
{
  if [ x = x$MotionMaskOffsetDetect ]; then
    MotionMaskOffsetDetect="0,-5,0"
  fi
  if [ x = x$FillingLevel ]; then
    FillingLevel=94
  fi

  clitkMotionMask -i $mask_dir_tmp/patient_$phase_nb.mhd -o $mask_dir_tmp/mm_$phase_nb.mhd --featureLungs $mask_dir_tmp/lungs_$phase_nb.mhd --upperThresholdLungs -400 --fillingLevel $FillingLevel --offsetDetect $MotionMaskOffsetDetect --pad --writeFeature=$mask_dir_tmp/feature_$phase_nb.mhd $MotionMaskExtra 
  #--monitor=$mask_dir_tmp/monitor_$phase_nb.mhd
}

create_banded_mask()
{
  input=$1
  input_mask=$2
  output=$3
  output_mask=$4
  radius=$5

  input_dir=`dirname $input`
  input_base=`basename $input`

  # first band
  clitkMorphoMath -i $input_mask -o $input_dir/extra1_$input_base --type 1 --radius $radius
  clitkImageArithm -i $input_dir/extra1_$input_base -j $input_mask -o $input_dir/band1_$input_base -t 7
  clitkBinarizeImage -i $input_dir/band1_$input_base -o $input_dir/band1_$input_base -l 1 -u 1 --fg 100 --mode both
  clitkImageConvert -i $input_dir/band1_$input_base -o $input_dir/short_band1_$input_base -t short
  
  # second band
  clitkMorphoMath -i $input_dir/extra1_$input_base -o $input_dir/extra2_$input_base --type 1 --radius $radius
  clitkImageArithm -i $input_dir/extra2_$input_base -j $input_dir/extra1_$input_base -o $input_dir/band2_$input_base -t 7
  clitkBinarizeImage -i $input_dir/band2_$input_base -o $input_dir/band2_$input_base -l 1 -u 1 --fg 200 --mode both
  clitkImageConvert -i $input_dir/band2_$input_base -o $input_dir/short_band2_$input_base -t short
  
  # combine bands with masks
  clitkImageArithm -i $input_mask -j $input_dir/band1_$input_base -o $output_mask -t 0
  clitkImageArithm -i $output_mask -j $input_dir/band2_$input_base -o $output_mask -t 0
  # combine bands with image
  clitkCombineImage -i $input_dir/short_band1_$input_base -j $input -m $input_dir/band1_$input_base -o $output
  clitkCombineImage -i $input_dir/short_band2_$input_base -j $output -m $input_dir/band2_$input_base -o $output

  # clean-up
  rm `echo $input_dir/extra?_$input_base | sed 's:.mhd:.*:g'`
  rm `echo $input_dir/band?_$input_base | sed 's:.mhd:.*:g'`
  rm `echo $input_dir/short_band?_$input_base | sed 's:.mhd:.*:g'`
}

create_registration_masks()
{
  # extract inside and outside lung regions from the patient image, 
  # using the motion mask computed previously
  echo "$phase_file -> Setting Background..."
  clitkSetBackground -i $mask_dir_tmp/patient_$phase_nb.mhd -o $mask_dir_tmp/inside_$phase_nb.mhd --mask $mask_dir_tmp/mm_$phase_nb.mhd --outsideValue -1200
  clitkSetBackground -i $mask_dir_tmp/patient_$phase_nb.mhd -o $mask_dir_tmp/outside_$phase_nb.mhd --mask $mask_dir_tmp/mm_$phase_nb.mhd --outsideValue -1200 --fg

  # the registration masks for inside (and outside) region correspond
  # to the motion mask (and its complement) plus extra grey value bands,
  # obtained with morphological dilations.
  # 
  echo "$phase_file -> Creating registration masks..."
  # inside
  create_banded_mask $mask_dir_tmp/inside_$phase_nb.mhd $mask_dir_tmp/mm_$phase_nb.mhd $mask_dir_tmp/banded_inside_$phase_nb.mhd $mask_dir_tmp/mask_inside_$phase_nb.mhd 4
  # outside 
  clitkExtractPatient -i $mask_dir_tmp/outside_$phase_nb.mhd -o $mask_dir_tmp/mm_outside_$phase_nb.mhd --noAutoCrop
  create_banded_mask $mask_dir_tmp/outside_$phase_nb.mhd $mask_dir_tmp/mm_outside_$phase_nb.mhd $mask_dir_tmp/banded_outside_$phase_nb.mhd $mask_dir_tmp/mask_outside_$phase_nb.mhd 4
}

mm_preprocessing()
{
  extract_patient
  # extract_bones
  extract_lungs
  # remove_tmp_masks 1
  if [ $resample_spacing -ne 0 ] ; then 
    resample
  fi
}

mm_postprocessing()
{
  # remove_tmp_masks 2
  # remove_tmp_masks 3
  create_registration_masks
}

motion_mask()
{
  #set cmd line variables
  mhd4d=`basename $1`
  if [ $# -eq 3 ] ; then
    resample_spacing=$2
    resample_algo=$3
  else
    resample_spacing=0
    resample_algo=0
  fi

  dir=`dirname $1`
  cd $dir
    
  # import variables specific to each patient
  if test -e ./variables; then
    source ./variables
  fi

  #set other global variables
  if [ $resample_spacing -ne 0 ] ; then
    mask_dir="MASK-${resample_spacing}mm-$resample_algo"
  else
    mask_dir="MASK"
  fi
  mask_dir_tmp="tmp.$mask_dir"
  extract_4d_phases $mhd4d

  echo
  echo "------------ Motion mask from create_midP_masks.sh ------------"
  start=`date`
  echo "start: $start"
  echo

  # the motion masks are first created in a tmp directory. this directory is 
  # later going to be renamed to the final motion mask directory. concurrent
  # executions trying to create the same set of masks will be blocked until
  # the first execution finishes. naturally, these other executions will not
  # recreate the masks. so first we try to create the tmp directory. 
  # if the creation fails, it means that another execution is
  # already creating the masks, so this execution will be blocked. the
  # execution is unblocked only when the creation of masks is finished and
  # the mask directory is renamed.
  #
  # ATTENTION: RP - 08/02/2011
  # robustness issue: tmp directory may exist but may be empty or 
  # incomplete. the solution is to check per file, but I'll leave it like 
  # this for the moment.
  do_mm=0
  if [ $(ls -d $mask_dir 2> /dev/null | wc -l) -eq 0 ]; then
    mkdir $mask_dir_tmp 2> /dev/null
    return_mkdir=$?
    if [ $return_mkdir == 0 ]; then
      do_mm=1 
    else
      while [[ $(ls -d $mask_dir 2> /dev/null | wc -l) -eq 0 ]]; do
        echo "waiting creation of motion masks..."
        sleep 2
      done
      echo "finished waiting"
    fi  
  fi
  
#   do_mm=1
#   mask_dir_tmp=$mask_dir
  if [ $do_mm == 1 ]; then
    mask_log_dir=$mask_dir_tmp/LOG
    mkdir -p $mask_log_dir

    # multi-threaded pre-processing for motion mask calcs
    pids=( )
    for i in $( seq 0 $((${#phase_nbs[@]} - 1))); do
      phase_nb=${phase_nbs[$i]}
      phase_file=${phase_files[$i]}
      afdb_file=`echo $phase_file | sed 's/mhd/afdb/'`

      check_threads $MAX_THREADS
      mm_preprocessing &
      pids=( "${pids[@]}" "$!" )
    done

    wait_pids ${pids[@]}
    for ret in $ret_codes; do
      abort_on_error mm_preprocessing $ret clean_up_masks
    done

    # single-threaded motion mask calc
    for i in $( seq 0 $((${#phase_nbs[@]} - 1))); do
      phase_nb=${phase_nbs[$i]}
      phase_file=${phase_files[$i]}

      check_threads 1
      echo "$phase_file -> Computing motion mask..."
      compute_motion_mask > $mask_log_dir/motion_mask_$phase_file.log
      abort_on_error compute_motion_mask $? clean_up_masks
    done

    # multi-threaded post-processing of motion mask calcs
    pids=( )
    for i in $( seq 0 $((${#phase_nbs[@]} - 1))); do
      phase_nb=${phase_nbs[$i]}
      phase_file=${phase_files[$i]}

      check_threads $MAX_THREADS 
      mm_postprocessing &
      pids=( "${pids[@]}" "$!" )
    done
  
    wait_pids ${pids[@]}
    for ret in $ret_codes; do
      abort_on_error mm_postprocessing $ret clean_up_masks
    done


    # rename tmp mask directory after mask creation
    check_threads 1
    mv -f $mask_dir_tmp $mask_dir
  fi

  echo
  echo "-------- Motion mask done ! ---------"
  end=`date`
  echo "start: $start"
  echo "end: $end"
  echo
}


#################
# main  #
#################

if [ $# -ne 3 -a $# -ne 1 ]; then
  echo "Usage: $0 CT_4D [RESAMPLE_SPACING RESAMPLE_ALGORITHM]"
  exit -1
fi

#
# variables exported in this scope
#
# mask_dir: directory where all masks are kept
#

if [ $1 != "using-as-lib" ]; then
  if [ $# -eq 3 ] ; then
    motion_mask $1 $2 $3
  else
    motion_mask $1
  fi
fi
