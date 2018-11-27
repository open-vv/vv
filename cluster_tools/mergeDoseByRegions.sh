#!/bin/bash
set -u

function usage {
  echo "$0 -i <result> -j <file2> -o <result>"
  exit 1
}

function addToPartialResult {
  IN1=$2 #merged file for previous jobs
  IN2=$4 # current job
  RESULT=$6 #output merged file

  test -f ${IN1} && test -f ${IN2} || usage

  TMP="$(mktemp)"

  # check if all 3 text files have the same number of lines
  nblines=`cat ${IN1} | wc -l`
  nb2=`cat ${IN2} | wc -l`
  nb3=`cat ${RESULT} | wc -l`

  if [ $nblines -ne $nb2 ] || [ $nblines -ne $nb3 ]; then
    echo "Files does not have the same size"
    exit 1
  fi

  #Copy the 1st line in output
  line1=`sed -n "1p" < ${RESULT}`
  echo "${line1}" >> ${TMP}
  # for all lines (except the 1st), split according tab
  # sum the some elements (dose, edep) ...
  for i in $(seq 2 $nblines); do
    #Get the 3 lines
    file1=`sed -n "${i}p" < ${IN1}`
    file2=`sed -n "${i}p" < ${IN2}`
    file3=`sed -n "${i}p" < ${RESULT}`

    # sum edep: get the 2 values, sum them and replace it in the file3
    # The /#./0. is important to add 0 for decimal value between 0 and 1
    edep1=$(echo ${file1} | cut -f3 -d' ')
    edep2=$(echo ${file2} | cut -f3 -d' ')
    edep3=$(python -c "print($edep1+$edep2)")
    edep3=${edep3/#./0.}
    file3=$(echo $file3 |awk -v r=${edep3} '{$3=r}1')

    # sum square_edep: get the 2 values, sum them and replace it in the file3
    sqEdep1=$(echo ${file1} | cut -f5 -d' ')
    sqEdep2=$(echo ${file2} | cut -f5 -d' ')
    sqEdep3=$(python -c "print($sqEdep1+$sqEdep2)")
    sqEdep3=${sqEdep3/#./0.}
    file3=$(echo $file3 |awk -v r=${sqEdep3} '{$5=r}1')

    # sum dose: get the 2 values, sum them and replace it in the file3
    dose1=$(echo ${file1} | cut -f6 -d' ')
    dose2=$(echo ${file2} | cut -f6 -d' ')
    dose3=$(python -c "print($dose1+$dose2)")
    dose3=${dose3/#./0.}
    file3=$(echo $file3 |awk -v r=${dose3} '{$6=r}1')

    # sum square_dose: get the 2 values, sum them and replace it in the file3
    sqDose1=$(echo ${file1} | cut -f8 -d' ')
    sqDose2=$(echo ${file2} | cut -f8 -d' ')
    sqDose3=$(python -c "print($sqDose1+$sqDose2)")
    sqDose3=${sqDose3/#./0.}
    file3=$(echo $file3 |awk -v r=${sqDose3} '{$8=r}1')

    # sum n_hits: get the 2 values, sum them and replace it in the file3
    hit1=$(echo ${file1} | cut -f9 -d' ')
    hit2=$(echo ${file2} | cut -f9 -d' ')
    hit3=$(python -c "print($hit1+$hit2)")
    file3=$(echo $file3 |awk -v r=${hit3} '{$9=r}1')

    # sum n_event_hits: get the 2 values, sum them and replace it in the file3
    event1=$(echo ${file1} | cut -f10 -d' ')
    event2=$(echo ${file2} | cut -f10 -d' ')
    event3=$(python -c "print($event1+$event2)")
    file3=$(echo $file3 |awk -v r=${event3} '{$10=r}1')

    #Write the output
    echo "${file3}" >> ${TMP}
  done
  mv -f ${TMP} ${RESULT}
}

function divideUncertaintyResult {
  IN1=$2
  value=$4
  RESULT=$6

  TMP="$(mktemp)"

  # check if all 2 text files have the same number of lines
  nblines=`cat ${IN1} | wc -l`
  nb3=`cat ${RESULT} | wc -l`

  if [ $nblines -ne $nb3 ]; then
    echo "Files does not have the same size"
    exit 1
  fi

  #Copy the 1st line in output
  line1=`sed -n "1p" < ${RESULT}`
  echo "${line1}" >> ${TMP}
  # for all lines (except the 1st), split according tab
  # sum the some elements (dose, edep) ...
  for i in $(seq 2 $nblines); do
    #Get the lines
    file1=`sed -n "${i}p" < ${IN1}`
    file3=`sed -n "${i}p" < ${RESULT}`

    # Divide uncertainty and replace it in the file3
    edep1=$(echo ${file1} | cut -f3 -d' ')
    sqEdep1=$(echo ${file1} | cut -f5 -d' ')
    stdEdep3=$(python <<EOP
import math
temp=math.sqrt(($sqEdep1/$value-pow($edep1/$value, 2))/($value-1))/$edep1/$value
print(temp)
EOP
    )
    stdEdep3=${stdEdep3/#./0.}
    file3=$(echo $file3 |awk -v r=${stdEdep3} '{$4=r}1')

    # Divide uncertainty and replace it in the file3
    dose1=$(echo ${file1} | cut -f6 -d' ')
    sqDose1=$(echo ${file1} | cut -f8 -d' ')
    stdDose3=$(python <<EOP
import math
temp=math.sqrt(($sqDose1/$value-pow($dose1/$value, 2))/($value-1))/$dose1/$value
print(temp)
EOP
    )
    stdDose3=${stdDose3/#./0.}
    file3=$(echo $file3 |awk -v r=${stdDose3} '{$7=r}1')

    #Write the output
    echo "${file3}" >> ${TMP}
  done
  mv -f ${TMP} ${RESULT}
}

