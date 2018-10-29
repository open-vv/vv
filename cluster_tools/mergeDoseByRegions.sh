#!/bin/bash
set -u

function usage {
  echo "$0 -i <file1> -j <file2> -o <result>"
  exit 1
}

if [ $# != 6 ]
then
  usage
fi

IN1=$2
IN2=$4
RESULT=$6

test -f ${IN1} && test -f ${IN2} || usage

TMP="$(mktemp)"
echo "merging dose by regions file"
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
  line1=`sed -n "${i}p" < ${IN1}`
  line2=`sed -n "${i}p" < ${IN2}`
  line3=`sed -n "${i}p" < ${RESULT}`

  # sum edep: get the 2 values, sum them and replace it in the line3
  # The /#./0. is important to add 0 for decimal value between 0 and 1
  edep1=$(echo ${line1} | cut -f3 -d' ')
  edep2=$(echo ${line2} | cut -f3 -d' ')
  edep3=$(python -c "print($edep1+$edep2)")
  edep3=${edep3/#./0.}
  line3=$(echo $line3 |awk -v r=${edep3} '{$3=r}1')

  # sqrt sum square std_edep: get the 2 values, sum the square, take the sqrt and replace it in the line3
  edep1=$(echo ${line1} | cut -f4 -d' ')
  edep2=$(echo ${line2} | cut -f4 -d' ')
  edep3=$(python -c "import math; print(math.sqrt($edep1*$edep1+$edep2*$edep2))")
  edep3=${edep3/#./0.}
  line3=$(echo $line3 |awk -v r=${edep3} '{$4=r}1')

  # sum square_edep: get the 2 values, sum them and replace it in the line3
  edep1=$(echo ${line1} | cut -f5 -d' ')
  edep2=$(echo ${line2} | cut -f5 -d' ')
  edep3=$(python -c "print($edep1+$edep2)")
  edep3=${edep3/#./0.}
  line3=$(echo $line3 |awk -v r=${edep3} '{$5=r}1')

  # sum dose: get the 2 values, sum them and replace it in the line3
  edep1=$(echo ${line1} | cut -f6 -d' ')
  edep2=$(echo ${line2} | cut -f6 -d' ')
  edep3=$(python -c "print($edep1+$edep2)")
  edep3=${edep3/#./0.}
  line3=$(echo $line3 |awk -v r=${edep3} '{$6=r}1')

  # sqrt sum square std_dose: get the 2 values, sum the square, take the sqrt and replace it in the line3
  edep1=$(echo ${line1} | cut -f7 -d' ')
  edep2=$(echo ${line2} | cut -f7 -d' ')
  edep3=$(python -c "import math; print(math.sqrt($edep1*$edep1+$edep2*$edep2))")
  edep3=${edep3/#./0.}
  line3=$(echo $line3 |awk -v r=${edep3} '{$7=r}1')

  # sum square_dose: get the 2 values, sum them and replace it in the line3
  edep1=$(echo ${line1} | cut -f8 -d' ')
  edep2=$(echo ${line2} | cut -f8 -d' ')
  edep3=$(python -c "print($edep1+$edep2)")
  edep3=${edep3/#./0.}
  line3=$(echo $line3 |awk -v r=${edep3} '{$8=r}1')

  # sum n_hits: get the 2 values, sum them and replace it in the line3
  edep1=$(echo ${line1} | cut -f9 -d' ')
  edep2=$(echo ${line2} | cut -f9 -d' ')
  edep3=$(python -c "print($edep1+$edep2)")
  line3=$(echo $line3 |awk -v r=${edep3} '{$9=r}1')

  # sum n_event_hits: get the 2 values, sum them and replace it in the line3
  edep1=$(echo ${line1} | cut -f10 -d' ')
  edep2=$(echo ${line2} | cut -f10 -d' ')
  edep3=$(python -c "print($edep1+$edep2)")
  line3=$(echo $line3 |awk -v r=${edep3} '{$10=r}1')

  #Write the output
  echo "${line3}" >> ${TMP}
done
mv -f ${TMP} ${RESULT}

