#!/bin/bash
set -u

function usage {
  echo "$0 -i <result> -j <file2> -o <result>"
  exit 1
}

function addToPartialResult {
  IN1=$2
  IN2=$4
  RESULT=$6

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

    # sqrt sum square std_edep*edep: get the 2 values, sum the square, take the sqrt and replace it in the file3
    stdEdep1=$(echo ${file1} | cut -f4 -d' ')
    stdEdep2=$(echo ${file2} | cut -f4 -d' ')
    stdEdep3=$(python <<EOP
import math;
temp=$edep2*$stdEdep2;
print(math.sqrt($stdEdep1*$stdEdep1+temp*temp))
EOP
    )
    stdEdep3=${stdEdep3/#./0.}
    file3=$(echo $file3 |awk -v r=${stdEdep3} '{$4=r}1')

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

    # sqrt sum square std_dose*dose: get the 2 values, sum the square, take the sqrt and replace it in the file3
    stdDose1=$(echo ${file1} | cut -f7 -d' ')
    stdDose2=$(echo ${file2} | cut -f7 -d' ')
    stdDose3=$(python <<EOP
import math;
temp=$dose2*$stdDose2;
print(math.sqrt($stdDose1*$stdDose1+temp*temp))
EOP
    )
    stdDose3=${stdDose3/#./0.}
    file3=$(echo $file3 |awk -v r=${stdDose3} '{$7=r}1')

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

function addWithoutPartialResult {
  IN1=$2
  RESULT=$4

  test -f ${IN1} || usage

  TMP="$(mktemp)"

  nblines=`cat ${IN1} | wc -l`

  #Copy the 1st line in output
  line1=`sed -n "1p" < ${IN1}`
  echo "${line1}" >> ${TMP}
  # for all lines (except the 1st), split according tab
  # sum the some elements (dose, edep) ...
  for i in $(seq 2 $nblines); do
    #Get the lines
    file1=`sed -n "${i}p" < ${IN1}`

    # copy id
    id1=$(echo ${file1} | cut -f1 -d' ')
    id1=${id1/#./0.}
    file3=$(echo $id1)

    # copy volume
    vol1=$(echo ${file1} | cut -f2 -d' ')
    vol1=${vol1/#./0.}
    file3=$(echo "$file3 $vol1")

    # copy edep
    edep1=$(echo ${file1} | cut -f3 -d' ')
    edep1=${edep1/#./0.}
    file3=$(echo "$file3 $edep1")

    # sqrt sum square std_edep*edep
    stdEdep1=$(echo ${file1} | cut -f4 -d' ')
    stdEdep3=$(python <<EOP
import math;
temp=$edep1*$stdEdep1;
print(math.sqrt(temp*temp))
EOP
    )
    stdEdep3=${stdEdep3/#./0.}
    file3=$(echo "$file3 $stdEdep3")

    # copy square_edep
    sqEdep1=$(echo ${file1} | cut -f5 -d' ')
    sqEdep1=${sqEdep1/#./0.}
    file3=$(echo "$file3 $sqEdep1")

    # copy dose
    dose1=$(echo ${file1} | cut -f6 -d' ')
    dose1=${dose1/#./0.}
    file3=$(echo "$file3 $dose1")

    # sqrt sum square std_dose*dose
    stdDose1=$(echo ${file1} | cut -f7 -d' ')
    stdDose3=$(python <<EOP
import math;
temp=$dose1*$stdDose1;
print(math.sqrt(temp*temp))
EOP
    )
    stdDose3=${stdDose3/#./0.}
    file3=$(echo "$file3 $stdDose3")

    # copy square_dose
    sqDose1=$(echo ${file1} | cut -f8 -d' ')
    sqDose1=${sqDose1/#./0.}
    file3=$(echo "$file3 $sqDose1")

    # copy n_hits
    hit1=$(echo ${file1} | cut -f9 -d' ')
    hit1=${hit1/#./0.}
    file3=$(echo "$file3 $hit1")

    # copy n_event_hits
    event1=$(echo ${file1} | cut -f10 -d' ')
    event1=${event1/#./0.}
    file3=$(echo "$file3 $event1")

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

  # check if all 3 text files have the same number of lines
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
    stdEdep1=$(echo ${file1} | cut -f4 -d' ')
    stdEdep3=$(python <<EOP
temp=$stdEdep1/$value
if $edep1!=0:
  temp/=$edep1
if temp==0:
  temp=1
print(temp)
EOP
    )
    stdEdep3=${stdEdep3/#./0.}
    file3=$(echo $file3 |awk -v r=${stdEdep3} '{$4=r}1')

    # Divide uncertainty and replace it in the file3
    dose1=$(echo ${file1} | cut -f6 -d' ')
    stdDose1=$(echo ${file1} | cut -f7 -d' ')
    stdDose3=$(python <<EOP
temp=$stdDose1/$value
if $dose1!=0:
  temp/=$dose1
if temp==0:
  temp=1
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

