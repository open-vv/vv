#! /bin/bash +x 

############# main 

if echo $* | grep "\-h"; then
  echo Usage: calculate_motion_amplitude.sh VECTOR_FILE RTSTRUCT_FILE RTSTRUCT_ROI_NUMBER RTSTRUCT_REF_IMAGE
  echo "  VECTOR_FILE: mhd of the vector field from where to extract motion information (may also be 4D)"
  echo "  RTSTRUCT_FILE: dicom with contours"
  echo "  RTSTRUCT_ROI: number of the contour whose motion to analyze"
  echo "  RTSTRUCT_REF_IMAGE: mhd of the reference image used in the contour delineation"

  exit 0
fi

if [ $# != 4 ]; then
  echo Invalid arguments. Type \'`basename $0` -h\' for help
  exit -1
fi

vector_file=$1
rtstruct_file=$2
rtstruct_roi=$3
rtstruct_ref_image=$4

if ! cat $vector_file | grep -q "Channels = 3"; then
  echo Vector file must have 3 channels.
  exit -2
fi

# create ROI mask from rtstruct
roi_mask=roi_${rtstruct_roi}.mhd
clitkDicomRTStruct2BinaryImage -i ${rtstruct_file} -o ${roi_mask} -j ${rtstruct_ref_image} -r ${rtstruct_roi} 2> /tmp/err.txt
if cat /tmp/err.txt | grep -q "No ROI"; then
  echo Invalid contour number.
  exit -3
fi

# guarantees the same sampling for roi mask and vector image
roi_mask2=resampled_${roi_mask}
clitkResampleImage -i ${roi_mask} -o ${roi_mask2} --like ${vector_file}

# calculate motion amplitudes along the 3 image axes
dir=( sagittal coronal axial )
for i in 0 1 2; do
  echo Motion along ${dir[$i]} direction
  clitkImageStatistics -i ${vector_file} -m ${roi_mask2} -c $i --verbose 2> /dev/null | tail -n 8 | head -n 6 > /tmp/res.txt
  min=`tail -n 2 /tmp/res.txt | head -n 1 | cut -f 2 -d ':'`
  max=`tail -n 1 /tmp/res.txt | cut -f 2 -d ':'`
  amp=`echo $max-$min | bc`
  cat /tmp/res.txt
  echo "Amplitude: $amp"
  echo
done

rm `basename $roi_mask .mhd`.{mhd,raw}
rm `basename $roi_mask2 .mhd`.{mhd,raw}
rm /tmp/err.txt
rm /tmp/res.txt
