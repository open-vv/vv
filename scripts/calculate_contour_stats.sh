#! /bin/bash +x 

select_contour_gui()
{
  local roi_list=$@
  roi=`zenity --list --title="Available Contours" --column="Please choose a contour:" $roi_list`
  case $? in
    0)
    if [ -z $roi ]
    then
      zenity --warning --text="You must choose one contour."
      select_contour $roi_list
    else
      rtstruct_roi_name=$roi
    fi;;
    1)
    if zenity --question --text="Do you really wish to quit?"
    then
      exit
    else
      select_contour $roi_list
    fi;;
    -1)
      zenity --error --text="Unexpected error. Please relaunch the application."
      exit;;
  esac
}

select_contour()
{
  local roi_list=$@
  echo "Available Contours:" 
  for r in $roi_list; do
    echo $r
  done

  echo "Please choose a contour number:"
  read rtstruct_roi_number
}

select_roi()
{
  rtstruct_roi_name_list=( `clitkDicomInfo ${rtstruct_file} | grep "3006|0026" | cut -d '[' -f 4 | sed 's/| V 3006|0026[LO] [ROI Name] \|]//'` )
  rtstruct_roi_number_list=( `clitkDicomInfo ${rtstruct_file} | grep "3006|0022" | cut -d '[' -f 4 | sed 's/| V 3006|0026[LO] [ROI Number] \|]//'` )
  rtstruct_roi_list=( )
  for i in $(seq 0 1 $(( ${#rtstruct_roi_name_list[@]} - 1 ))); do
    rtstruct_roi_list[$i]=${rtstruct_roi_number_list[$i]}:${rtstruct_roi_name_list[$i]}
  done

  if [ $gui_mode = 1 ]; then
    select_contour_gui ${rtstruct_roi_list[@]}
    rtstruct_roi=`echo ${rtstruct_roi_name} | cut -d ':' -f 1`
    rtstruct_roi_name=`echo ${rtstruct_roi_name} | cut -d ':' -f 2`
  else
    select_contour ${rtstruct_roi_list[@]}
    rtstruct_roi=$rtstruct_roi_number
    rtstruct_roi_name=${rtstruct_roi_name_list[$(( $rtstruct_roi_number - 1))]}
  fi
}

############# main 

if echo $* | grep "\-h"; then
  echo Usage: calculate_motion_amplitude.sh { RTSTRUCT_FILE REFERENCE_IMAGE | --gui }

  exit 0
fi


rtstruct_roi=0

if echo $* | grep "\-\-gui" > /dev/null 2>&1; then
  if [ $# != 1 ]; then
    echo Invalid arguments. Type \'`basename $0` -h\' for help
    exit -1
  fi

  gui_mode=1
  rtstruct_file=`zenity --file-selection --title="Select RT Struct file."`
  select_roi
  rtstruct_ref_image=`zenity --file-selection --title="Select Reference Image."`
else
  if [ $# != 2 ]; then
    echo Invalid arguments. Type \'`basename $0` -h\' for help
    exit -1
  fi

  gui_mode=0
  rtstruct_file=$1
  rtstruct_ref_image=$2
  select_roi
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
