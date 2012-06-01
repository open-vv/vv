#! /bin/bash +x 

source `dirname $0`/midp_common.sh


############# main 

if echo $* | grep "\-h"; then
  echo Usage: calculate_motion_amplitude.sh { REFERENCE_IMAGE RTSTRUCT_FILE [ RTSTRUCT_ROI | --gui ] } | --gui 
  echo "  RTSTRUCT_REF_IMAGE: mhd of the reference image used in the contour delineation"
  echo "  RTSTRUCT_FILE: dicom with contours"
  echo "  RTSTRUCT_ROI: number of the contour whose motion to analyze"
  echo "  --gui: use GUI to select files"
  exit 0
fi


rtstruct_roi=

if echo $* | grep "\-\-gui" > /dev/null 2>&1; then
  gui_mode=1
  if [ $# = 1 ]; then
    rtstruct_ref_image=`zenity --file-selection --title="Select Reference Image."`
    rtstruct_file=`zenity --file-selection --title="Select RT Struct file."`
    select_roi
  elif [ $# = 3 ]; then
    rtstruct_ref_image=$1
    rtstruct_file=$2
    select_roi
  else
    echo Invalid arguments. Type \'`basename $0` -h\' for help
    exit -1
  fi

else
  gui_mode=0
  rtstruct_ref_image=$1
  rtstruct_file=$2
  if [ $# = 2 ]; then
    select_roi
  elif [ $# = 3 ]; then
    rtstruct_roi=$3
    rtstruct_roi_name=$rtstruct_roi
  else
    echo Invalid arguments. Type \'`basename $0` -h\' for help
    exit -1
  fi

fi


#echo "Processing \"${rtstruct_roi_list[$roi_struct]}\"..."
# create ROI mask from rtstruct
roi_mask=roi_${rtstruct_roi_name}.mhd
clitkDicomRTStruct2Image -i ${rtstruct_file} -o ${roi_mask} -j ${rtstruct_ref_image} -r ${rtstruct_roi} 2> /tmp/err.txt
if cat /tmp/err.txt | grep -q "No ROI"; then
  echo Invalid contour number.
  exit -3
fi

# guarantees the same sampling for roi mask and vector image
roi_mask2=resampled_${roi_mask}
clitkResampleImage -i ${roi_mask} -o ${roi_mask2} --like ${rtstruct_ref_image}

# calculate stats
clitkImageStatistics -i ${rtstruct_ref_image} -m ${roi_mask2} --verbose 2> /dev/null | tail -n 8 > /tmp/result.txt

if [ $gui_mode = 1 ]; then
  cat /tmp/result.txt | zenity --text-info --title "Restuls for \"${rtstruct_roi_name}\""
else
  echo "Restuls for \"${rtstruct_roi_name}\""
  cat /tmp/result.txt 
fi

rm `basename $roi_mask .mhd`.{mhd,raw}
rm `basename $roi_mask2 .mhd`.{mhd,raw}
rm /tmp/err.txt
rm /tmp/result.txt
