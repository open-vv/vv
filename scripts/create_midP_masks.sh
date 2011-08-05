#! /bin/bash -x

source `dirname $0`/common.sh

extract_patient()
{
  echo "$image_name -> Extracting patient..."
  clitkExtractPatient -i $image_name -o $mask_dir_tmp/patient_$image_name --noAutoCrop -a $afdb_name $ExtractPatientExtra
  clitkBinarizeImage -i $mask_dir_tmp/patient_$image_name -o $mask_dir_tmp/patient_$image_name -l 1 -u 1 --mode=BG
  clitkSetBackground -i $image_name -o $mask_dir_tmp/patient_only_$image_name --mask $mask_dir_tmp/patient_$image_name --outsideValue -1000
}

extract_bones()
{
  if [ x = x$ExtractBonesLower1 ]; then
    ExtractBonesLower1=120
  fi
  if [ x = x$ExtractBonesLower2 ]; then
    ExtractBonesLower2=80
  fi
  echo "$image_name -> Extracting bones..."
  clitkImageConvert -i $image_name -o $mask_dir_tmp/float_$image_name -t float
  #clitkExtractBones -i $mask_dir_tmp/float_$image_name -o $mask_dir_tmp/bones_$image_name --lower1 120 --upper1 2000 --lower2 70 --upper2 2000 --smooth --time 0.0625 --noAutoCrop
  clitkExtractBones -i $mask_dir_tmp/float_$image_name -o $mask_dir_tmp/bones_$image_name -a $afdb_name --lower1 $ExtractBonesLower1 --upper1 2000 --lower2 $ExtractBonesLower2 --upper2 2000 --smooth --time 0.0625 --noAutoCrop
  #clitkMorphoMath -i $mask_dir_tmp/bones_$image_name -o $mask_dir_tmp/bones_$image_name --type 2 --radius 4,4,2
}

extract_lungs()
{
  echo "$image_name -> Extracting lungs..."
  clitkExtractLung -i $image_name -o $mask_dir_tmp/lungs_$image_name -a $afdb_name --noAutoCrop
}

resample()
{
  echo "$image_name -> Resampling..."
  clitkResampleImage -i $mask_dir_tmp/patient_$image_name -o $mask_dir_tmp/patient_$image_name --spacing $resample_spacing --interp $resample_algo
  clitkResampleImage -i $mask_dir_tmp/patient_only_$image_name -o $mask_dir_tmp/patient_only_$image_name --spacing $resample_spacing --interp $resample_algo
  clitkResampleImage -i $mask_dir_tmp/bones_$image_name -o $mask_dir_tmp/bones_$image_name --like $mask_dir_tmp/patient_only_$image_name --interp $resample_algo
  clitkResampleImage -i $mask_dir_tmp/lungs_$image_name -o $mask_dir_tmp/lungs_$image_name --like $mask_dir_tmp/patient_only_$image_name
}

compute_motion_mask()
{
  if [ x = x$MotionMaskOffsetDetect ]; then
    MotionMaskOffsetDetect="0,-5,0"
  fi
  if [ x = x$FillingLevel ]; then
    FillingLevel=94
  fi
  clitkMotionMask -i $mask_dir_tmp/patient_only_$image_name -o $mask_dir_tmp/mm_$image_name --featureBones=$mask_dir_tmp/bones_$image_name --featureLungs=$mask_dir_tmp/lungs_$image_name --upperThresholdLungs -400 --fillingLevel $FillingLevel --offsetDetect 0,-5,0 --pad --writeFeature=$mask_dir_tmp/feature_$image_name $MotionMaskExtra --monitor=$mask_dir_tmp/monitor_$image_name
}

set_background()
{
  echo "$image_name -> Setting Background..."
  clitkSetBackground -i $mask_dir_tmp/patient_only_$image_name -o $mask_dir_tmp/inside_$image_name --mask $mask_dir_tmp/mm_$image_name --outsideValue -1200
  clitkSetBackground -i $mask_dir_tmp/patient_only_$image_name -o $mask_dir_tmp/outside_$image_name --mask $mask_dir_tmp/mm_$image_name --outsideValue -1200 --fg
}

create_registration_masks()
{
  echo "$image_name -> Creating registration masks..."
  clitkMorphoMath -i $mask_dir_tmp/mm_$image_name -o $mask_dir_tmp/regmask_in_$image_name --type 1 --radius 8
  clitkExtractPatient -i $mask_dir_tmp/outside_$image_name -o $mask_dir_tmp/regmask_out_$image_name --noAutoCrop
  clitkMorphoMath -i $mask_dir_tmp/regmask_out_$image_name -o $mask_dir_tmp/regmask_out_$image_name --type 1 --radius 8
}

remove_tmp_masks()
{
  echo "Removing temporary files..."
  image_name_base=`echo $image_name | sed 's/mhd//'`
  case $1 in
  1)
    #rm $mask_dir_tmp/float_$image_name_base*
    ;;
  2)
    #rm $mask_dir_tmp/bones_$image_name_base*
    ;;
  3)
    #rm $mask_dir_tmp/patient_only_$image_name_base*
    ;;
  esac
}

mm_preprocessing()
{
  extract_patient
  extract_bones
  extract_lungs
  remove_tmp_masks 1
  resample
}

mm_postprocessing()
{
  remove_tmp_masks 2
  set_background
  remove_tmp_masks 3
  create_registration_masks
}

mm_workflow()
{
  extract_patient
  extract_bones
  remove_tmp_masks 1
  resample
  echo "$image_name -> Computing motion mask..."
  compute_motion_mask >> $mask_log_dir/motion_mask_$image_name.log
  remove_tmp_masks 2
  set_background
  remove_tmp_masks 3
  create_registration_masks
}

motion_mask()
{
  #set cmd line variables
  echo "4D CT -> "$1
  phases=`grep ".raw" $1`
  echo "mask phases -> " $phases

  dir=`dirname $1`
  cd $dir

  # import variables specific to each patient
  source variables

  resample_spacing=$2
  resample_algo=$3

    
  #set other global variables
  mask_dir="MASK-$resample_spacing""mm""-$resample_algo"
  mask_dir_tmp="tmp."$mask_dir

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

  regmask_in_list=""
  regmask_out_list=""
  reg_in_list=""
  reg_out_list=""

  # multi-threaded pre-processing for motion mask calcs
  if [ $do_mm == 1 ]; then
    mask_log_dir=$mask_dir_tmp/LOG
    mkdir -p $mask_log_dir

    for phase in $phases
    do
      image_name=`echo $phase | sed 's/raw/mhd/'`
      afdb_name=`echo $phase | sed 's/raw/afdb/'`
      check_threads $MAX_THREADS
      mm_preprocessing &
    done

    # single-threaded motion mask calc
    for phase in $phases
    do
      image_name=`echo $phase | sed 's/raw/mhd/'`

      check_threads 1
      echo "$image_name -> Computing motion mask..."
      compute_motion_mask >> $mask_log_dir/motion_mask_$image_name.log
    done
  fi

  # multi-threaded post-processing of motion mask calcs
  for phase in $phases
  do
    image_name=`echo $phase | sed 's/raw/mhd/'`
    if [ $do_mm = 1 ]; then
      check_threads $MAX_THREADS 
      mm_postprocessing &
    fi

    regmask_in_list="$regmask_in_list regmask_in_$image_name"
    regmask_out_list="$regmask_out_list regmask_out_$image_name"
    reg_in_list="$reg_in_list inside_$image_name"
    reg_out_list="$reg_out_list outside_$image_name"
  done

  # rename tmp mask directory after mask creation
  check_threads 1
  mv $mask_dir_tmp $mask_dir

  wait

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

if [ $# != 3 ]; then
  echo "Usage: create_midP_masks.sh 4D_CT.mhd resample_spacing resample_algorithm"
  exit -1
fi

#
# variables exported in this scope
#
# mask_dir: directory where all masks are kept
# regmask_in_list: list of registration mask files (inside lungs)
# regmask_out_list: list of registration mask files (outised lungs)
# reg_in_list: list of registration image files (inside lungs)
# reg_out_list: list of registration image files (outside lungs)
#

if [ $1 != "using-as-lib" ]; then
  motion_mask $1 $2 $3
fi
