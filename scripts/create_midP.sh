#!/bin/sh -x

#################################################################
# create_MidP 	arguments : CT_4D.mhd ref_phase spacing 	#
#################################################################
source `dirname $0`/common.sh

extract_patient()
{
  echo "$image_name -> Extracting patient..."
  $CLITK/clitkExtractPatient -i $image_name -o MASK/patient_$image_name --noAutoCrop
  $CLITK/clitkBinarizeImage -i MASK/patient_$image_name -o MASK/patient_$image_name -l 1 -u 1 --mode=BG
  $CLITK/clitkSetBackground -i $image_name -o MASK/patient_only_$image_name --mask MASK/patient_$image_name --outsideValue -1000
}

extract_bones()
{
  echo "$image_name -> Extracting bones..."
  $CLITK/clitkImageConvert -i $image_name -o MASK/float_$image_name -t float
  $CLITK/clitkExtractBones -i MASK/float_$image_name -o MASK/bones_$image_name --lower1 120 --upper1 2000 --lower2 70 --upper2 2000 --smooth --time 0.0625 --noAutoCrop
  $CLITK/clitkMorphoMath -i MASK/bones_$image_name -o MASK/bones_$image_name --type 2 --radius 4,4,2
}

resample()
{
  echo "$image_name -> Resampling..."
  $CLITK/clitkResampleImage -i MASK/patient_$image_name -o MASK/patient_$image_name --spacing $spacing
  $CLITK/clitkResampleImage -i MASK/patient_only_$image_name -o MASK/patient_only_$image_name --spacing $spacing
  #$CLITK/clitkResampleImage -i MASK/bones_$image_name -o MASK/bones_$image_name --like MASK/patient_only_$image_name
}

compute_motion_mask()
{
#   $CLITK/clitkMotionMask -i MASK/patient_only_$image_name -o MASK/mm_$image_name --featureBones=MASK/bones_$image_name --upperThresholdLungs -400 --fillingLevel 94 --offsetDetect 0,-5,0 --pad --writeFeature=MASK/feature_$image_name --writeEllips=MASK/inital_ellipse_$image_name --writeGrownEllips=MASK/growing_ellipse_$image_name;
$CLITK/clitkMotionMask -i MASK/patient_only_$image_name -o MASK/mm_$image_name --upperThresholdLungs -400 --fillingLevel 94 --offsetDetect 0,-5,0 --pad --writeFeature=MASK/feature_$image_name --writeEllips=MASK/inital_ellipse_$image_name --writeGrownEllips=MASK/growing_ellipse_$image_name;
}

set_background()
{
  echo "$image_name -> Setting Background..."
  $CLITK/clitkSetBackground -i MASK/patient_only_$image_name -o MASK/inside_$image_name --mask MASK/mm_$image_name --outsideValue -1200
  $CLITK/clitkSetBackground -i MASK/patient_only_$image_name -o MASK/outside_$image_name --mask MASK/mm_$image_name --outsideValue -1200 --fg
}

create_registration_masks()
{
  echo "$image_name -> Creating registration masks..."
  $CLITK/clitkMorphoMath -i MASK/mm_$image_name -o MASK/regmask_in_$image_name --type 1 --radius 8
  $CLITK/clitkExtractPatient -i MASK/outside_$image_name -o MASK/regmask_out_$image_name --noAutoCrop
  $CLITK/clitkMorphoMath -i MASK/regmask_out_$image_name -o MASK/regmask_out_$image_name --type 1 --radius 8
}

remove_files()
{
  echo "Removing temporary files..."
  image_name_base=`echo $image_name | sed 's/mhd//'`
  case $1 in
    1)
      rm MASK/float_$image_name_base*;;
    2)
      rm MASK/bones_$image_name_base*;;
    3)
      rm MASK/patient_only_$image_name_base*;;
    4)
      #rm MASK/patient_$image_name_base*
      #rm MASK/mm_$image_name_base*
      rm -f $vf_dir/vf_tmp_in_${ref}_${phase}.*
      rm -f $vf_dir/vf_tmp_out_${ref}_${phase}.*
      #rm MASK/regmask_in_$image_name_base*
      #rm MASK/regmask_out_$image_name_base*
      ;;
    5)
#       rm -f coeff_*
      #rm $vf_dir/vf_in_*
      #rm $vf_dir/vf_out_*
#       rm MASK/regmask_*
#       rm MASK/mm_*
      ;;
    6)
      ;;
      #rm -f $vf_dir/_4D.*
      #rm -f $vf_dir/vf_MIDP_${ref}.*
      #rm -f $vf_dir/vf_${ref}_MIDP.*;;
    7)
      ;; #rm $vf_dir/vf_MIDP_${phase}.*;;
    8)
      rm $vf_dir/*.txt
      rm $vf_dir/*.log
  esac
}

mm_preprocessing()
{
  extract_patient
  #extract_bones
  remove_files 1
  resample
}

mm_postprocessing()
{
  remove_files 2
  set_background
  remove_files 3
  create_registration_masks
}

# mm_workflow()
# {
#   extract_patient
#   extract_bones
#   remove_files 1
#   resample
#   echo "$image_name -> Computing motion mask..."
#   compute_motion_mask >> LOG/motion_mask_$image_name.log
#   remove_files 2
#   set_background
#   remove_files 3
#   create_registration_masks
# }

motion_mask()
{
  echo
  echo "------------ Motion mask ------------"
  start=`date`
  echo "start: $start"
  echo
  mkdir -p "MASK"
  rm -f "LOG/motion_mask*.log"
  regmask_in_list=""
  regmask_out_list=""
  reg_in_list=""
  reg_out_list=""

  # multi-threaded pre-processing for motion mask calcs
  for phase in $phase_list
  do
    image_name=`echo $phase | sed 's/raw/mhd/'`
    check_threads $MAX_THREADS
    #mm_preprocessing &
  done

  # single-threaded motion mask calc
  check_threads 1
  for phase in $phase_list
  do
    image_name=`echo $phase | sed 's/raw/mhd/'`

    echo "$image_name -> Computing motion mask..."
    #compute_motion_mask >> LOG/motion_mask_$image_name.log
  done

  # multi-threaded post-processing of motion mask calcs
  for phase in $phase_list
  do
    image_name=`echo $phase | sed 's/raw/mhd/'`
    check_threads $MAX_THREADS 
    #mm_postprocessing &
    regmask_in_list="$regmask_in_list regmask_in_$image_name"
    regmask_out_list="$regmask_out_list regmask_out_$image_name"
    reg_in_list="$reg_in_list inside_$image_name"
    reg_out_list="$reg_out_list outside_$image_name"
  done

  wait
  echo
  echo "-------- Motion mask done ! ---------"
  end=`date`
  echo "start: $start"
  echo "end: $end"
  echo
}

compute_BLUTDIR()
{
  ########## register in ##########
  for reg_in in $reg_in_list
  do
    if [ ! -z `echo $reg_in | grep "$phase"` ]
    then
      target_in=$reg_in
    fi
  done
  echo "Computing BLUTDIR $reference_in -> $target_in ..."
  #$CLITK/clitkBLUTDIR --reference="MASK/$reference_in" --target="MASK/$target_in" --output="MASK/reg_$target_in" --referenceMask="MASK/$reference_mask_in" --vf="$vf_dir/vf_in_${ref}_${phase}.mhd" $coeff_in_ini --coeff="$coeff_dir/coeff_in_${ref}_${phase}.mhd" $registration_parameters_BLUTDIR >> LOG/registration_${ref}_${phase}.log
  $CLITK/clitkBLUTDIR --reference="MASK/$reference_in" --target="MASK/$target_in" --output="MASK/reg_$target_in" --referenceMask="MASK/$reference_mask_in" --vf="$vf_dir/vf_in_${ref}_${phase}.mhd" --coeff="$coeff_dir/coeff_in_${ref}_${phase}.mhd" $registration_parameters_BLUTDIR >> LOG/registration_${ref}_${phase}.log
  coeff_in_ini="--initCoeff=$coeff_dir/coeff_in_${ref}_${phase}.mhd"
  ########## register out ##########
  for reg_out in $reg_out_list
  do
    if [ ! -z `echo $reg_out | grep "$phase"` ]
    then
      target_out=$reg_out
    fi
  done
  echo "Computing BLUTDIR $reference_out -> $target_out ..."
  #$CLITK/clitkBLUTDIR --reference="MASK/$reference_out" --target="MASK/$target_out" --output="MASK/reg_$target_out" --referenceMask="MASK/$reference_mask_out" --vf="$vf_dir/vf_out_${ref}_${phase}.mhd" $coeff_out_ini --coeff="$coeff_dir/coeff_out_${ref}_${phase}.mhd" $registration_parameters_BLUTDIR >> LOG/registration_${ref}_${phase}.log
  $CLITK/clitkBLUTDIR --reference="MASK/$reference_out" --target="MASK/$target_out" --output="MASK/reg_$target_out" --referenceMask="MASK/$reference_mask_out" --vf="$vf_dir/vf_out_${ref}_${phase}.mhd" --coeff="$coeff_dir/coeff_out_${ref}_${phase}.mhd" $registration_parameters_BLUTDIR >> LOG/registration_${ref}_${phase}.log
  coeff_out_ini="--initCoeff=$coeff_dir/coeff_out_${ref}_${phase}.mhd"
  ##################################
  $CLITK/clitkCombineImage -i $vf_dir/vf_in_${ref}_${phase}.mhd -j $vf_dir/vf_out_${ref}_${phase}.mhd -m MASK/mm_$image_name -o $vf_dir/vf_${ref}_${phase}.mhd
  $CLITK/clitkZeroVF -i $vf_dir/vf_${ref}_${phase}.mhd -o $vf_dir/vf_${ref}_${ref}.mhd
  $CLITK/clitkCombineImage -i $vf_dir/vf_${ref}_${phase}.mhd -j $vf_dir/vf_${ref}_${ref}.mhd -m MASK/patient_$image_name -o $vf_dir/vf_${ref}_${phase}.mhd
  remove_files 4
}

compute_DEMONSDIR()
{
  ########## register in ##########
  for reg_in in $reg_in_list
  do
    if [ ! -z `echo $reg_in | grep "_$phase"` ]
    then
      target_in=$reg_in
    fi
  done
  echo "Computing DEMONSDIR $reference_in -> $target_in ..."
  $CLITK/clitkDemonsDeformableRegistration --reference="MASK/$reference_in" --target="MASK/$target_in" --output="MASK/reg_$target_in" --vf="$vf_dir/vf_in_${ref}_${phase}.mhd" $vf_in_ini $registration_parameters_DEMONSDIR #&>> LOG/registration_${ref}_${phase}.log
  vf_in_ini="--init=$vf_dir/vf_in_${ref}_${phase}.mhd"
  ########## register out ##########
  for reg_out in $reg_out_list
  do
    if [ ! -z `echo $reg_out | grep "_$phase"` ]
    then
      target_out=$reg_out
    fi
  done
  echo "Computing DEMONSDIR $reference_out -> $target_out ..."
  $CLITK/clitkDemonsDeformableRegistration --reference="MASK/$reference_out" --target="MASK/$target_out" --output="MASK/reg_$target_out" --vf="$vf_dir/vf_out_${ref}_${phase}.mhd" $vf_out_ini $registration_parameters_DEMONSDIR #&>> LOG/registration_${ref}_${phase}.log
  vf_out_ini="--init=$vf_dir/vf_out_${ref}_${phase}.mhd"
  ##################################
  $CLITK/clitkCombineImage -i $vf_dir/vf_in_${ref}_${phase}.mhd -j $vf_dir/vf_out_${ref}_${phase}.mhd -m MASK/mm_$image_name -o $vf_dir/vf_${ref}_${phase}.mhd
  $CLITK/clitkZeroVF -i $vf_dir/vf_${ref}_${phase}.mhd -o $vf_dir/vf_${ref}_${ref}.mhd
  $CLITK/clitkCombineImage -i $vf_dir/vf_${ref}_${phase}.mhd -j $vf_dir/vf_${ref}_${ref}.mhd -m MASK/patient_$image_name -o $vf_dir/vf_${ref}_${phase}.mhd
  remove_files 4
}

compute_ELASTIX()
{
  ########## register in ##########
  for reg_in in $reg_in_list
  do
    if [ ! -z `echo $reg_in | grep "_$phase"` ]
    then
      target_in=$reg_in
    fi
  done
  echo "Computing ELASTIX $reference_in -> $target_in ..."
  exec_dir=`which elastix`
  exec_dir=`dirname $exec_dir`
  cat $exec_dir/params_BSpline.txt | sed -e "s+<NbIterations>+500+" \
                              -e "s+<LabelsFile>++" \
                              -e "s+<HistBins>+25+" \
                              -e "s+<Levels>+3+" \
                              -e "s+<NbSamples>+2000+" \
                              -e "s+<SamplerType>+Random+" \
                              -e "s+<Spacing>+32+" > params_BSpline.txt 
  elastix -f "MASK/$reference_in" -m "MASK/$target_in" -fMask "MASK/$reference_mask_in" -out $vf_dir -p params_BSpline.txt > /dev/null
  transformix -tp $vf_dir/TransformParameters.0.txt -out $vf_dir -def all > /dev/null  
  mv $vf_dir/deformationField.mhd $vf_dir/vf_in_${ref}_${phase}.mhd
  mv $vf_dir/deformationField.raw $vf_dir/vf_in_${ref}_${phase}.raw
  sed -i "s:deformationField:vf_in_${ref}_${phase}:" $vf_dir/vf_in_${ref}_${phase}.mhd
  mv $vf_dir/result.0.mhd MASK/reg_$target_in
  targetraw=`echo reg_$target_in | sed 's:mhd:raw:'`
  sed -i "s:result.0.mhd:$targetraw" MASK/reg_$target_in
  mv $vf_dir/result.0.raw MASK/$targetraw
  remove_files 8

  ########## register out ##########
  for reg_out in $reg_out_list
  do
    if [ ! -z `echo $reg_out | grep "_$phase"` ]
    then
      target_out=$reg_out
    fi
  done
  echo "Computing ELASTIX $reference_out -> $target_out ..."
  elastix -f "MASK/$reference_out" -m "MASK/$target_out" -fMask "MASK/$reference_mask_out" -out $vf_dir -p params_BSpline.txt > /dev/null
  transformix -tp $vf_dir/TransformParameters.0.txt -out $vf_dir -def all > /dev/null  
  mv $vf_dir/deformationField.mhd $vf_dir/vf_out_${ref}_${phase}.mhd
  mv $vf_dir/deformationField.raw $vf_dir/vf_out_${ref}_${phase}.raw
  sed -i "s:deformationField:vf_out_${ref}_${phase}:" $vf_dir/vf_out_${ref}_${phase}.mhd
  mv $vf_dir/result.0.mhd MASK/reg_$target_out
  targetraw=`echo reg_$target_out | sed 's:mhd:raw:'`
  sed -i "s:result.0.mhd:$targetraw" MASK/reg_$target_out
  mv $vf_dir/result.0.raw MASK/$targetraw
  remove_files 8

  ##################################
  $CLITK/clitkCombineImage -i $vf_dir/vf_in_${ref}_${phase}.mhd -j $vf_dir/vf_out_${ref}_${phase}.mhd -m MASK/mm_$image_name -o $vf_dir/vf_${ref}_${phase}.mhd
  $CLITK/clitkZeroVF -i $vf_dir/vf_${ref}_${phase}.mhd -o $vf_dir/vf_${ref}_${ref}.mhd
  $CLITK/clitkCombineImage -i $vf_dir/vf_${ref}_${phase}.mhd -j $vf_dir/vf_${ref}_${ref}.mhd -m MASK/patient_$image_name -o $vf_dir/vf_${ref}_${phase}.mhd
  remove_files 4
}

registration()
{
  echo
  echo "----------- Registration ------------"
  start=`date`
  echo "start: $start"
  echo

  rm -f "LOG/registration*.log"

  # wait any unfinished threads
  check_threads 1

  for reg_in in $reg_in_list
  do
    if [ ! -z `echo $reg_in | grep "$ref"` ]
    then
      reference_in=$reg_in
    fi
  done
  for reg_out in $reg_out_list
  do
    if [ ! -z `echo $reg_out | grep "$ref"` ]
    then
      reference_out=$reg_out
    fi
  done
  for regmask_in in $regmask_in_list
  do
    if [ ! -z `echo $regmask_in | grep "$ref"` ]
    then
      reference_mask_in=$regmask_in
    fi
  done
  for regmask_out in $regmask_out_list
  do
    if [ ! -z `echo $regmask_out | grep "$ref"` ]
    then
      reference_mask_out=$regmask_out
    fi
  done

  registration_parameters_BLUTDIR="--spacing=32,32,32 --interp=2 --metric=11 --bins=25 --samples=1 --levels=3 --verbose " #--coeffEveryN 5"
  registration_parameters_DEMONSDIR="--demons=3 --levels=1"
  registration_parameters_ELASTIX="--demons=3 --levels=1"

  coeff_in_ini=""
  coeff_out_ini=""
  vf_in_ini=""
  vf_out_ini=""
  
  for phase in $list_phases
  do
    for img in $phase_list
    do
      if [ ! -z `echo $img | grep "$phase" | grep -v "[0-9]$phase"` ]
      then
	image_name=`echo $img | sed 's/raw/mhd/'`
      fi
    done
    if [ $method = 1 ]
    then
      compute_BLUTDIR
    elif [ $method = 2 ]
    then
      compute_DEMONSDIR
    elif [ $method = 3 ]
    then
      compute_ELASTIX
    fi
  done
  remove_files 5

  echo
  echo "-------- Registration done ! --------"
  end=`date`
  echo "start: $start"
  echo "end: $end"
  echo
}

calculate_vf_MIDP_REF()
{
  echo "Calculating vf_REF_MIDP.mhd..."
  remove_files 6
  create_mhd_4D.sh $vf_dir #"vf_4D.mhd"
  $CLITK/clitkAverageTemporalDimension -i $vf_dir/_4D.mhd -o $vf_dir/vf_${ref}_MIDP.mhd
  $CLITK/clitkInvertVF -i $vf_dir/vf_${ref}_MIDP.mhd -o $vf_dir/vf_MIDP_${ref}.mhd
}

calculate_CT_MIDP_REF()
{
  reference=`ls *.mhd | grep $ref #| grep -v "[0-9]$ref.mhd"`
  echo "Calculating CT_MIDP_REF.mhd '$reference'..."
  $CLITK/clitkWarpImage -i $reference -o CT_MIDP_REF.mhd --vf=$vf_dir/vf_MIDP_${ref}.mhd -s 1
}

calculate_CT_MIDP_PHASE()
{
  echo "Calculating CT_MIDP_${phase}.mhd..."
  $CLITK/clitkComposeVF -i $vf_dir/vf_MIDP_${ref}.mhd -j $vf_dir/vf_${ref}_${phase}.mhd -o $vf_dir/vf_MIDP_${phase}.mhd
  phase_img=`ls *.mhd | grep "${phase}" # | grep -v "[0-9]$ref.mhd"`
  $CLITK/clitkWarpImage -i $phase_img -o MIDP/CT_MIDP_${phase}.mhd --vf=$vf_dir/vf_MIDP_${phase}.mhd -s 1
  $CLITK/clitkImageConvert -i MIDP/CT_MIDP_${phase}.mhd -o MIDP/CT_MIDP_${phase}.mhd -t float
  remove_files 7
}

prepare_MIDP_images()
{
  echo "Preparing MIDP images..."
  cp CT_MIDP_REF.mhd MIDP/CT_MIDP_${ref}.mhd
  cp CT_MIDP_REF.raw MIDP/CT_MIDP_${ref}.raw
  cat MIDP/CT_MIDP_${ref}.mhd | sed "s/ElementDataFile = .*/ElementDataFile = CT\_MIDP\_${ref}\.raw/" > MIDP/CT_MIDP_${ref}_tmp.mhd
  rm MIDP/CT_MIDP_${ref}.mhd
  mv MIDP/CT_MIDP_${ref}_tmp.mhd MIDP/CT_MIDP_${ref}.mhd
  $CLITK/clitkImageConvert -i MIDP/CT_MIDP_${ref}.mhd -o MIDP/CT_MIDP_${ref}.mhd -t float
  create_mhd_4D.sh MIDP #"CT_MIDP_4D.mhd"
}

calculate_CT_MIDP_MOY()
{
  echo "Calculating CT_MIDP_MOY.mhd..."
  $CLITK/clitkAverageTemporalDimension -i MIDP/_4D.mhd -o CT_MIDP_MOY.mhd
}

calculate_CT_MIDP_MED()
{
  echo "Calculating CT_MIDP_MED.mhd..."
  $CLITK/clitkMedianTemporalDimension -i MIDP/_4D.mhd -o CT_MIDP_MED.mhd
}

calculate_CT_MIDP_MIP()
{
  echo "Calculating CT_MIDP_MIP.mhd..."
  $CLITK/clitkMIP -i MIDP/_4D.mhd -o CT_MIDP_MIP.mhd -d 3
}

mid_position()
{
  echo
  echo "----------- Mid-position ------------"
  start=`date`
  echo "start: $start"
  echo
  
  mkdir -p "MIDP"

  calculate_vf_MIDP_REF
  calculate_CT_MIDP_REF
  for phase in $list_phases
  do  
    check_threads $MAX_THREADS
    calculate_CT_MIDP_PHASE &
  done
  wait
  prepare_MIDP_images
  calculate_CT_MIDP_MED &
  calculate_CT_MIDP_MIP &
  calculate_CT_MIDP_MOY &
  wait
      
  echo
  echo "-------- Mid-position done ! --------"
  end=`date`
  echo "start: $start"
  echo "end: $end"
  echo
}

#################
#	main	#
#################

if [ $# -lt 1 ]
then
    echo "Usage: create_midP.sh CT_4D.mhd ref_phase computation_spacing(mm) method(1:BSPLINE, 2:DEMONS)"
    exit 1
fi
CLITK=~/creatis/clitk3/build/bin
CT_4D_path=$1
CT_4D=`basename $CT_4D_path`
work_dir=`dirname $CT_4D_path`
cd $work_dir
ref=$2
spacing=$3
method=$4

vf_dir="VF"
coeff_dir="$vf_dir/coeff"

mkdir -p $vf_dir
mkdir -p $coeff_dir

echo
echo "--------------- START ---------------"
begining=`date`
echo "start: $begining"
echo

mkdir -p "LOG"
phase_list=`grep ".raw" $CT_4D`
echo "phases -> " $phase_list
nb_phases=`grep ".raw" $CT_4D | wc -l`
gt_ref=""
lt_ref=""

phase_files=( `cat $CT_4D | grep ".raw" | sed 's:.raw:.mhd:'` )
echo "Phase files are ${phase_files[@]}"

phase_nbs=( `echo ${phase_files[@]} | grep -o '[[:alpha:][:punct:]][0-9]\{1,2\}[[:punct:]]' | grep -o '[0-9]\{1,2\}'` )
#phase_nbs=( `echo ${phase_files[@]} | grep -o '[0-9]\{1,2\}'` )  
echo "Phase numbers are ${phase_nbs[@]}"


for ph in $phase_list
do
  #ph_nb=`echo $ph | grep -o "[0-9][0-9]*\.raw" | sed -e 's/\.raw//'`
  ph_nb=`echo $ph | grep -o "[0-9][0-9]"`  
  if [ $ph_nb -gt $ref ]
  then
    gt_ref="$gt_ref $ph_nb"
  elif [ $ph_nb -lt $ref ]
  then
    lt_ref="$lt_ref $ph_nb"
  fi
done
list_phases="$gt_ref $lt_ref"
echo list_phases $list_phases

motion_mask
registration
mid_position

echo
echo "---------------- END ----------------"
terminating=`date`
echo "start: $begining"
echo "end: $terminating"
echo
