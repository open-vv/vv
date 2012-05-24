#! /bin/bash -x
  
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
  clitkExtractLung -i $phase_file -o $mask_dir_tmp/lungs_$phase_nb.mhd -a $afdb_file --noAutoCrop --doNotSeparateLungs --type 1
}



resample()
{
  echo "$phase_file -> Resampling..."
  clitkResampleImage -i $mask_dir_tmp/patient_$phase_nb.mhd -o $mask_dir_tmp/patient_$phase_nb.mhd --spacing $resample_spacing --interp $resample_algo
  clitkResampleImage -i $mask_dir_tmp/patient_mask_$phase_nb.mhd -o $mask_dir_tmp/patient_mask_$phase_nb.mhd --spacing $resample_spacing --interp $resample_algo
  if [ "$mask_type" != "patient" ]; then
    clitkResampleImage -i $mask_dir_tmp/lungs_$phase_nb.mhd -o $mask_dir_tmp/lungs_$phase_nb.mhd --like $mask_dir_tmp/patient_$phase_nb.mhd
  fi
  if [ "$mask_type" == "mm" ]; then
    clitkResampleImage -i $mask_dir_tmp/bones_$phase_nb.mhd -o $mask_dir_tmp/bones_$phase_nb.mhd --like $mask_dir_tmp/patient_$phase_nb.mhd
  fi
}

compute_motion_mask()
{
  if [ x = x$MotionMaskOffsetDetect ]; then
    MotionMaskOffsetDetect="0,-5,0"
  fi
  if [ x = x$FillingLevel ]; then
    FillingLevel=94
  fi

  clitkMotionMask -i $mask_dir_tmp/patient_$phase_nb.mhd -o $mask_dir_tmp/mm_$phase_nb.mhd --featureLungs $mask_dir_tmp/lungs_$phase_nb.mhd --upperThresholdLungs -400 --featureBones $mask_dir_tmp/bones_$phase_nb.mhd --fillingLevel $FillingLevel --offsetDetect $MotionMaskOffsetDetect --pad --writeFeature $mask_dir_tmp/feature_$phase_nb.mhd $MotionMaskExtra  
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
  combine_image $input_dir/short_band1_$input_base $input $output $input_dir/band1_$input_base
  combine_image $input_dir/short_band2_$input_base $output $output $input_dir/band2_$input_base

  # clean-up
  rm `echo $input_dir/extra?_$input_base | sed 's:.mhd:.*:g'`
  rm `echo $input_dir/band?_$input_base | sed 's:.mhd:.*:g'`
  rm `echo $input_dir/short_band?_$input_base | sed 's:.mhd:.*:g'`
}

create_registration_masks()
{
  # extract inside and outside regions from the patient image, 
  # using the motion mask computed previously
  echo "$phase_file -> Setting Background..."
  clitkSetBackground -i $mask_dir_tmp/patient_$phase_nb.mhd -o $mask_dir_tmp/inside_$phase_nb.mhd --mask $mask_dir_tmp/${mask_type}_$phase_nb.mhd --outsideValue -1200
  clitkSetBackground -i $mask_dir_tmp/patient_$phase_nb.mhd -o $mask_dir_tmp/outside_$phase_nb.mhd --mask $mask_dir_tmp/${mask_type}_$phase_nb.mhd --outsideValue -1200 --fg

  # the registration masks for inside (and outside) region correspond
  # to the motion mask (and its complement) plus extra grey value bands,
  # obtained with morphological dilations.
  # 
  echo "$phase_file -> Creating registration masks..."
  # inside
  clitkMorphoMath -i $mask_dir_tmp/${mask_type}_$phase_nb.mhd -o $mask_dir_tmp/mask_inside_$phase_nb.mhd --type 1 --radius 8
  create_banded_mask $mask_dir_tmp/inside_$phase_nb.mhd $mask_dir_tmp/${mask_type}_$phase_nb.mhd $mask_dir_tmp/banded_inside_$phase_nb.mhd $mask_dir_tmp/banded_mask_inside_$phase_nb.mhd 4
  # outside 
  clitkBinarizeImage -i $mask_dir_tmp/outside_$phase_nb.mhd -o $mask_dir_tmp/${mask_type}_outside_$phase_nb.mhd -l -999 -u 4000 --mode both 
  #clitkExtractPatient -i $mask_dir_tmp/outside_$phase_nb.mhd -o $mask_dir_tmp/${mask_type}_outside_$phase_nb.mhd --noAutoCrop
  clitkMorphoMath -i $mask_dir_tmp/${mask_type}_outside_$phase_nb.mhd -o $mask_dir_tmp/mask_outside_$phase_nb.mhd --type 1 --radius 8
  create_banded_mask $mask_dir_tmp/outside_$phase_nb.mhd $mask_dir_tmp/${mask_type}_outside_$phase_nb.mhd $mask_dir_tmp/banded_outside_$phase_nb.mhd $mask_dir_tmp/banded_mask_outside_$phase_nb.mhd 4
}

mm_preprocessing()
{
  extract_patient

  if [ "$mask_type" != "patient" ]; then
    extract_lungs
  fi

  if [ "$mask_type" == "mm" ]; then
    extract_bones
  fi

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
#   if [ "$mask_type" == "mm" ]; then
#     create_registration_motion_masks
#   elif [ "$mask_type" == "lungs" ]; then
#     create_registration_lung_masks
#   fi
}

wait_mm_creation()
{
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
  do_mm=1
  if [ -e $mask_dir ]; then
    # check that the final mask dir exists and that it contains all files it needs.
    # the check assumes that the inside and outside masks are the key files to exist.
    do_mm=0
    nb_phases=${#phase_nbs[@]}
    if [ "$mask_type" == "patient" ]; then
      nb_masks=`ls $mask_dir/lungs_*.mhd | wc -l`
      if [ $nb_masks != $nb_phases ]; then
        # if the mask dir is invalid, remove it and recreate all masks, just in case.
        rm -fr $mask_dir 2> /dev/null
        do_mm=1
      fi
    else
      nb_mm_masks=`ls $mask_dir/${mask_type}_outside*.mhd | wc -l`
      nb_in_masks=`ls $mask_dir/mask_in*.mhd | wc -l`
      nb_out_masks=`ls $mask_dir/mask_out*.mhd | wc -l`
      if [ $nb_mm_masks != $nb_phases -o $nb_in_masks != $nb_phases -o $nb_out_masks != $nb_phases ]; then
        # if the mask dir is invalid, remove it and recreate all masks, just in case.
        rm -fr $mask_dir 2> /dev/null
        do_mm=1
      fi
    fi
  fi
  
  if [ $do_mm = 1 ]; then
    if ! mkdir $mask_dir_tmp 2> /dev/null; then
      if [ ! -e $mask_dir_tmp ]; then
        # if the temp dir couldn't be created, but it doesn't exist, abort
        abort_on_error wait_mm_creation $? clean_up_masks
      else
        # assumes another process is creating the maks in the temp dir.
        # now we need to wait until the masks are complete or until the
        # time limit is reached. 
        interval=10
        sleeping=0
        max_wait=3600 # one hour
        nb_files0=`ls $mask_dir_tmp/* | wc -l`
        while [ ! -e $mask_dir -a $sleeping -le $max_wait ]; do
          echo "waiting creation of motion masks..."
          sleep $interval
          sleeping=$(( $sleeping + $interval ))
          nb_files1=`ls $mask_dir_tmp/* | wc -l`
          if [ $nb_files1 != $nb_files0 ]; then
            nb_files0=$nb_files1
            sleeping=0
          fi  
        done

        if [ $sleeping -gt $max_wait ]; then
          abort_on_error wait_mm_creation -1 clean_up_masks
        else
          echo "finished waiting"
          do_mm=0
        fi
      fi
    fi  
  fi
}

motion_mask()
{
  #set cmd line variables
  local mhd4d=`basename $1`
  mask_type=$2
  if [ $# -eq 4 ] ; then
    resample_spacing=$3
    resample_algo=$4
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

  wait_mm_creation

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

    if [ "$mask_type" == "mm" ]; then
      # single-threaded motion mask calc
      for i in $( seq 0 $((${#phase_nbs[@]} - 1))); do
        phase_nb=${phase_nbs[$i]}
        phase_file=${phase_files[$i]}

        check_threads 1
        echo "$phase_file -> Computing motion mask..."
        compute_motion_mask > $mask_log_dir/motion_mask_$phase_file.log
        abort_on_error compute_motion_mask $? clean_up_masks
      done
    fi

    # multi-threaded post-processing of motion mask calcs
    if [ "$mask_type" != "patient" ]; then
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
    fi

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

if [ $# -ne 4 -a $# -ne 2 -a $# -ne 1 ]; then
  echo "Usage: $0 CT_4D TYPE [RESAMPLE_SPACING RESAMPLE_ALGORITHM]"
  echo "  TYPE: \"mm\" (traditional motion masks); \"lungs\" (lung masks); \"patient\" (patient mask only)"
  exit -1
fi

#
# variables exported in this scope
#
# mask_dir: directory where all masks are kept
#

if [ $1 != "using-as-lib" ]; then
  if [ $# -eq 4 ] ; then
    motion_mask $1 $2 $3 $4
  elif [ $# -eq 2 ] ; then
    motion_mask $1 $2
  else
    motion_mask $1 all
  fi
fi
