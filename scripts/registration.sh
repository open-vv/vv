#! /bin/sh

###############################################################################
#
# FILE: registration.sh
# AUTHOR: Rômulo Pinho 05/08/2011
#
# Helper file with registration functions using different methods.
# Each function receives a set of parameters that overall apply to any
# registration algorithm, as follows:
# 
# reference=$1 : reference (fixed) image
# target=$2 : target (moving) image
# mask_ref=$3 : mask for the reference image
# mask_targ=$4 : mask for the moving image
# vf=$5 : output vector field representing the registration
# result=$6 : result image after applying the vector field
# nb_iter=$7 : maximum number of iterations
# nb_samples=$8 : number of image samples used in the metric calulcation
# sampling_algo=$9 : algorithm used in the selection of image samples
# hist_bins=${10} : number of histogram bins used in the metric calculation
# nb_levels=${11} : number of image resolutions
# spacing=${12} : spacing of the b-spline grid of the fines resolution
# metric=${13} : metric algorithm
# optimizer=${14} : optimizer
# interpolator=${15} : image interpolator 
# log=${16} : log file
#
# New registration functions may be added to this file at any moment, 
# respecting the interface defined above.
#
###############################################################################

source `dirname $0`/midp_common.sh


################# BLUTDIR #####################
registration_blutdir()
{
  local reference=$1
  local target=$2
  local mask_ref=$3
  local mask_targ=$4
  local vf=$5
  local result=$6
  local nb_iter=$7
  local nb_samples=$8
  local sampling_algo=$9
  local hist_bins=${10}
  local nb_levels=${11}
  local spacing=${12}
  local metric=${13}
  local optimizer=${14}
  local interpolator=${15}
  local log=${16}
  local coeff=${17}
  local init_coeff=${18}

  echo "Computing BLUTDIR $reference -> $target ..."
  blutdir_params="--levels $nb_levels  --metric $metric --optimizer $optimizer --samples $nb_samples --spacing $spacing,$spacing,$spacing --bins $hist_bins --maxIt $nb_iter --interp $interpolator --centre --verbose"

  if [ -n "$coeff" ]; then
    coeff="--coeff $coeff"
  fi
  
  if [ -n "$init_coeff" ]; then
    init_coeff="--initCoeff $init_coeff"
  fi
  
  cmd="clitkBLUTDIR -r $reference -t $target -m $mask_ref --targetMask $mask_targ --vf $vf $coeff $init_coeff -o $result $blutdir_params"
  $cmd > $log

  abort_on_error registration_blutdir $? clean_up_registration
}

################# ELASTIX #####################
registration_elastix()
{
  local reference=$1
  local target=$2
  local mask_ref=$3
  local mask_targ=$4
  local vf=$5
  local result=$6
  local nb_iter=$7
  local nb_samples=$8
  local sampling_algo=$9
  local hist_bins=${10}
  local nb_levels=${11}
  local spacing=${12}
  local metric=${13}
  local optimizer=${14}
  local interpolator=${15}
  local log=${16}
  local coeff=${17}
  local init_coeff=${18}

# RP 25/06/2012: unused variables (for now at elast)  
#   local log=${16}
#   local coeff=${17}
#   local init_coeff=${18}
  
  ########## register in ##########
  for reg_in in $reg_in_list
  do
    if [ ! -z `echo $reg_in | grep "_$phase"` ]
    then
      target_in=$reg_in
    fi
  done
  echo "Computing ELASTIX $reference -> $target ..."
  exec_dir=`which elastix`
  exec_dir=`dirname $exec_dir`
  suffix=${nb_samples}_${nb_iter}_${nb_levels}
  cat $exec_dir/params_elastix_template.txt | sed -e "s+<NbIterations>+$nb_iter+" \
                              -e "s+<HistBins>+$hist_bins+" \
                              -e "s+<Levels>+$nb_levels+" \
                              -e "s+<PctSamples>+$nb_samples+" \
                              -e "s+<SamplerType>+$sampling_algo+" \
                              -e "s+<Metric>+$metric+" \
                              -e "s+<Optimizer>+$optimizer+" \
                              -e "s+<Interpolator>+$interpolator+" \
                              -e "s+<Spacing>+$spacing+" > params_elastix_${suffix}.txt 

  # set elastix to write out the coefficient images and corresponding DVFs
  echo "(WriteCoefficientImage \"true\")" >> params_elastix_${suffix}.txt 
  echo "(WriteDVFFromCoeff \"true\")" >> params_elastix_${suffix}.txt 

  vf_dir=`dirname $vf`
  vf_base=`basename $vf .mhd`
  coeff_dir=`dirname $coeff`
  coeff_base=`basename $coeff .mhd`
  result_dir=`dirname $result`
  result_base=`basename $result .mhd`

  # image registration
  cmd="elastix -f $reference -m $target -fMask $mask_ref -mMask $mask_targ -out $result_dir -p params_elastix_${suffix}.txt"
  $cmd  > /dev/null
  abort_on_error registration_elastix $? clean_up_registration

  # generate vector field
  # cmd="transformix -tp $result_dir/TransformParameters.0.txt -out $vf_dir -def all"
  # $cmd  > /dev/null
  # abort_on_error registration_elastix $? clean_up_registration

  # post-processing
  local level=$(($nb_levels - 1))
  mv $result_dir/dvf.0.R$level.mhd $vf
  mv $result_dir/dvf.0.R$level.raw `echo $vf | sed 's/mhd/raw/'`
  sed -i "s+dvf.0.R$level+$vf_base+" $vf

  mv $result_dir/coeff.0.R$level.mhd $coeff_dir/${coeff_base}_0.mhd
  mv $result_dir/coeff.0.R$level.raw $coeff_dir/${coeff_base}_0.raw
  sed -i "s+coeff.0.R$level+$coeff_base+" $coeff_dir/${coeff_base}_0.mhd

  mv $result_dir/result.0.mhd $result
  mv $result_dir/result.0.raw `echo $result | sed 's/mhd/raw/'`
  sed -i "s+result.0+$result_base+" $result

  mv $result_dir/elastix.log $log
  mv $result_dir/TransformParameters.0.txt $result_dir/${result_base}_TransformParameters.0.txt
}