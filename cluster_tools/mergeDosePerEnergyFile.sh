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
echo "merging dose file"
for PARAM in `awk '$1 == "#" {print $3}' ${IN1}`
do
        echo "merging ${PARAM}"
	V1=`awk -v P=${PARAM} '$3 == P {print $4}' ${IN1} `
	V2=`awk -v P=${PARAM} '$3 == P {print $4}' ${IN2} `
        V1=`echo ${V1} | sed -e 's/[eE]+*/\*10\^/'`
        V2=`echo ${V2} | sed -e 's/[eE]+*/\*10\^/'`
	R=`echo "scale=30; ${V1} + ${V2}" | bc -l`
        test -z "${R}" && continue
        echo "# energydose ${PARAM} ${R}" >> ${TMP}
done
mv -f ${TMP} ${RESULT}
