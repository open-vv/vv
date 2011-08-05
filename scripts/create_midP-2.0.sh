#!/bin/sh -x

###############################################################################
#
# FILE: create_midP-2.0.sh
# AUTHOR: Rômulo Pinho 05/08/2011
#
# Version 2.0 of the create_midP.sh script. The most relevant changes are:
#   * receives a .conf file as input, with variables related to the registration
#   parameters and to paramters of the script itself (see accompanying midp_template.conf)
#   for details.
#   * separates execution steps: it's now possible to choose which operation to execute
#   (mask, registration, midp, or all).
#   * some steps are now in different modules, to facilitate re-use (see "includes" section).
#   * minor modifications on output file names.
#   * attempt to simplify the code a bit.
#
###############################################################################

######################### includes

source `dirname $0`/create_midP_masks-2.0.sh using-as-lib 2 nn
source `dirname $0`/registration.sh
source `dirname $0`/common.sh

registration()
{
  echo
  echo "----------- Registration ------------"
  start=`date`
  echo "start: $start"
  echo

  mkdir -p $vf_dir
  mkdir -p $output_dir

  # banded images may be created as separate files,
  # with the specified preffix, which is interesting for debugging. 
  # if blank, it means that the original images (those without bands) 
  # will be used (see create_midP_masks-2.0.sh for details).
  banded="banded_"

  # params read from conf file
  params="$nb_iter $nb_samples $sampling_algo $nb_hist_bins $nb_levels $bspline_spacing $metric $optimizer $interpolator"

  # register all phases to the reference
  for i in $( seq 0 $((${#phase_files[@]} - 1))); do
    phase_file=${phase_files[$i]}
    phase_nb=${phase_nbs[$i]}
    
    if [ "$phase_nb" != "$ref_phase_nb" ]; then
      # inside params
      reference_in=$mask_dir/${banded}inside_$ref_phase_nb.mhd
      target_in=$mask_dir/${banded}inside_$phase_nb.mhd
      mask_ref_in=$mask_dir/mask_inside_$ref_phase_nb.mhd
      mask_targ_in=$mask_dir/mask_inside_$phase_nb.mhd
      vf_in=$vf_dir/vf_inside_${ref_phase_nb}_$phase_nb.mhd
      result_in=$output_dir/result_inside_${ref_phase_nb}_$phase_nb.mhd
      log_in=$log_dir/log_inside_${ref_phase_nb}_$phase_nb.log

      # outside params
      reference_out=$mask_dir/${banded}outside_$ref_phase_nb.mhd
      target_out=$mask_dir/${banded}outside_$phase_nb.mhd
      mask_ref_out=$mask_dir/mask_outside_$ref_phase_nb.mhd
      mask_targ_out=$mask_dir/mask_outside_$phase_nb.mhd
      vf_out=$vf_dir/vf_outside_$ref_phase_nb\_$phase_nb.mhd
      result_out=$output_dir/result_outside_$ref_phase_nb\_$phase_nb.mhd
      log_out=$log_dir/log_outside_${ref_phase_nb}_$phase_nb.log

      # registration
      if [ "$method" == "blutdir" ]; then
        registration_blutdir $reference_in $target_in $mask_ref_in $mask_targ_in $vf_in $result_in $params $log_in
        registration_blutdir $reference_out $target_out $mask_ref_out $mask_targ_out $vf_out $result_out $params $log_out
      elif [ "$method" == "elastix" ]; then
        registration_elastix $reference_in $target_in $mask_ref_in $mask_targ_in $vf_in $result_in $params $log_in
        registration_elastix $reference_out $target_out $mask_ref_out $mask_targ_out $vf_out $result_out $params $log_out
      fi

      # combine in and out vf
      motion_mask=$mask_dir/mm_$phase_nb.mhd
      vf_result=$vf_dir/vf_${ref_phase_nb}_$phase_nb.mhd
      clitkCombineImage -i $vf_in -j $vf_out -m $motion_mask -o $vf_result
      clitkZeroVF -i $vf_in -o vf_zero.mhd
      clitkCombineImage -i $vf_result -j vf_zero.mhd -m $patient_mask -o $vf_result
      rm vf_zero.*

      # save for later...
      vf_ref=$vf_in
    fi
  done

  # create (zero) vf from reference to reference
  clitkZeroVF -i $vf_ref -o $vf_dir/vf_${ref_phase_nb}_${ref_phase_nb}.mhd

  # create 4D vf
  create_mhd_4D_pattern.sh $vf_dir/vf_${ref_phase_nb}_

  echo
  echo "-------- Registration done ! --------"
  end=`date`
  echo "start: $start"
  echo "end: $end"
  echo
}

midp()
{
  echo
  echo "----------- Mid-position ------------"
  start=`date`
  echo "start: $start"
  echo

  mkdir -p $midp_dir

  ########### calculate the midp wrt the reference phase
  phase_nb=$ref_phase_nb
  echo "Calculating midp_$phase_nb.mhd..."
  vf_midp=$midp_dir/vf_$phase_nb\_midp.mhd
  midp=$midp_dir/midp_$phase_nb.mhd
  # average the vf's from reference phase to phase
  clitkAverageTemporalDimension -i $vf_dir/vf_${ref_phase_nb}_4D.mhd -o $vf_midp
  # invert the vf (why?)
  clitkInvertVF -i $vf_midp -o $vf_midp
  # create the midp by warping the reference phase with the reference vf
  clitkWarpImage -i $ref_phase_file -o $midp --vf=$vf_midp -s 1

  ref_vf_midp=$vf_midp
  ref_midp=$midp
  clitkImageConvert -i $ref_midp -o $ref_midp -t float

  ########### calculate the midp wrt the other phases
  for i in $( seq 0 $((${#phase_files[@]} - 1))); do
    phase_file=${phase_files[$i]}
    phase_nb=${phase_nbs[$i]}
    vf_midp=$midp_dir/vf_$phase_nb\_midp.mhd
    midp=$midp_dir/midp_$phase_nb.mhd

    if [ "$phase_nb" != "$ref_phase_nb" ]; then
      echo "Calculating midp_$phase_nb.mhd..."
      # calculate vf from phase to midp, using the vf from reference phase to midp (-i)
      # and the vf from reference phase to phase (-j)
      clitkComposeVF -i $ref_vf_midp -j $vf_dir/vf_$ref_phase_nb\_$phase_nb.mhd -o $vf_midp
      clitkWarpImage -i $phase_file -o $midp --vf=$vf_midp -s 1
      clitkImageConvert -i $midp -o $midp -t float
    fi
  done

  create_mhd_4D_pattern.sh $midp_dir/midp_
  echo "Calculating midp_avg.mhd..."
  clitkAverageTemporalDimension -i $midp_dir/midp_4D.mhd -o $midp_dir/midp_avg.mhd
  echo "Calculating midp_med.mhd..."
  clitkMedianTemporalDimension -i $midp_dir/midp_4D.mhd -o $midp_dir/midp_med.mhd

  # clean-up
  rm $midp_dir/vf_*
      
  echo
  echo "-------- Mid-position done ! --------"
  end=`date`
  echo "start: $start"
  echo "end: $end"
  echo
}



######################### main

if [ $# != 3 ]; then
  echo "Usage: create_midP-2.0.sh CT_4D REF_PHASE CONF_FILE"
  exit -1
fi

echo
echo "--------------- START ---------------"
begining=`date --rfc-3339=seconds`
# echo "beginning: $begining"
echo

# variable declarations
mhd4d=$1
ref_phase=$2
conf=$3
source $conf

mkdir -p $log_dir
mask_dir="MASK-${mask_interpolation_spacing}mm-$mask_interpolation_algorithm"

extract_4d_phases_ref $mhd4d $ref_phase

if [ "$step" == "mask" -o "$step" == "all" ]; then
  motion_mask $mhd4d $mask_interpolation_spacing $mask_interpolation_algorithm 
fi 

if [ "$step" == "registration" -o "$step" == "all" ]; then
  registration
fi 

if [ "$step" == "midp" -o "$step" == "all" ]; then
  midp
fi 

echo
echo "---------------- END ----------------"
terminating=`date --rfc-3339=seconds`
echo "beginning: $begining"
echo "terminating: $terminating"
echo
