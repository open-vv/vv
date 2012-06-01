#! /bin/bash +x 

source `dirname $0`/midp_common.sh


calculate_motion()
{
  # calculate motion amplitudes along the 3 image axes
  rm /tmp/result.txt 2> /dev/null
  dir=( "\"left-right\"" "\"anterior-posterior\"" "\"cranio-caudal\"" )
  pct=( 0 33 67 )
  for i in 0 1 2; do
    if [ $gui_mode = 1 ]; then
      echo "${pct[$i]}"; echo "# Calculating motion along ${dir[$i]} direction.."
    else
      echo "Calculating motion along ${dir[$i]} direction.."
    fi

    echo "Motion along ${dir[$i]} direction" >> /tmp/result.txt
    clitkImageStatistics -i ${vector_file} -m ${roi_mask2} -c $i --verbose 2> /dev/null | tail -n 8 | head -n 6 >> /tmp/result.txt
    min=`tail -n 2 /tmp/result.txt | head -n 1 | cut -f 2 -d ':'`
    max=`tail -n 1 /tmp/result.txt | cut -f 2 -d ':'`
    amp=`echo $max-$min | bc`

    echo "Amplitude: $amp" >> /tmp/result.txt
    echo "" >> /tmp/result.txt
  done
}

############# main 

if echo $* | grep "\-h"; then
  echo "Usage: calculate_motion_amplitude.sh { VECTOR_FILE RTSTRUCT_REF_IMAGE RTSTRUCT_FILE [ RTSTRUCT_ROI_NUMBER | --gui ] } | --gui "
  echo "  VECTOR_FILE: mhd of the vector field from where to extract motion information (may also be 4D)"
  echo "  RTSTRUCT_REF_IMAGE: mhd of the reference image used in the contour delineation"
  echo "  RTSTRUCT_FILE: dicom with contours"
  echo "  RTSTRUCT_ROI: number of the contour whose motion to analyze"
  echo "  --gui: use GUI to select files"
  exit 0
fi

if echo $* | grep "\-\-gui" > /dev/null 2>&1; then
  gui_mode=1
  if [ $# = 1 ]; then
    vector_file=`zenity --file-selection --title="Select 4D Vector Field file."`
    rtstruct_ref_image=`zenity --file-selection --title="Select Reference Image."`
    rtstruct_file=`zenity --file-selection --title="Select RT Struct file."`
    select_roi
  elif [ $# = 4 ]; then
    vector_file=$1
    rtstruct_ref_image=$2
    rtstruct_file=$3
    select_roi
  else
    echo Invalid arguments. Type \'`basename $0` -h\' for help
    exit -1
  fi
else
  gui_mode=0
  vector_file=$1
  rtstruct_ref_image=$2
  rtstruct_file=$3
  if [ $# = 4 ]; then
    rtstruct_roi=$4
    rtstruct_roi_name=$rtstruct_roi
  elif [ $# = 3 ]; then
    select_roi
  else
    echo Invalid arguments. Type \'`basename $0` -h\' for help
    exit -1
  fi
fi

if ! cat $vector_file | grep -q "Channels = 3"; then
  echo Vector file must have 3 channels.
  exit -2
fi

# create ROI mask from rtstruct
roi_mask=roi_${rtstruct_roi}.mhd
clitkDicomRTStruct2Image -i ${rtstruct_file} -o ${roi_mask} -j ${rtstruct_ref_image} -r ${rtstruct_roi} 2> /tmp/err.txt
if cat /tmp/err.txt | grep -q "No ROI"; then
  echo Invalid contour number.
  exit -3
fi

# guarantees the same sampling for roi mask and vector image
roi_mask2=resampled_${roi_mask}
clitkResampleImage -i ${roi_mask} -o ${roi_mask2} --like ${vector_file}

if [ $gui_mode = 1 ]; then
  calculate_motion | zenity --progress --auto-close --percentage=0 --text=""
else
  calculate_motion
fi

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
