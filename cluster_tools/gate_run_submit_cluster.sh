#!/usr/bin/env bash

set -u
SCRIPTNAME="$(basename "${0}")"

# -------------------------------------------------
function error {
    echo "ERROR: $1"
    usage
    exit 1
}
# -------------------------------------------------

DEFAULTRELEASESUFFIX="NONE"
DEFAULTNUMBEROFJOBS="10"

# -------------------------------------------------
function usage {
    echo "${SCRIPTNAME} mac/main.mac njobs releasesuffix paramtogate"
    echo "default njobs = ${DEFAULTNUMBEROFJOBS}"
    echo "default releasesuffix = ${DEFAULTRELEASESUFFIX} (NONE means use Gate in PATH)"
    echo "default paramtogate = \"\" (use \"\" around params and \\ in front of commas)"
}
# -------------------------------------------------

test $# -eq 0 && usage && exit 0

RELEASESUFFIX=${3:-"${DEFAULTRELEASESUFFIX}"}
RELEASEDIR="${HOME}/releases/grid_release${RELEASESUFFIX}"
if test "$(dnsdomainname)" = "in2p3.fr"
then
    JOBFILE="$(dirname $0)/gate_job_ccin2p3.job"
else
    JOBFILE="$(dirname $0)/gate_job_cluster.job"
fi

echo "Checking stuff"
test -f ${JOBFILE} || error "can't find job file ${JOBFILE}"
if test "${RELEASESUFFIX}" = "${DEFAULTRELEASESUFFIX}"
then
    RELEASEDIR="NONE"
    which Gate 2>&1 >/dev/null || error "there is no Gate in the PATH"
else
    test -d ${RELEASEDIR} || error "invalid release dir ${RELEASEDIR}"
fi
MACRODIR=$(pwd)
test -d ${MACRODIR}/mac && test -d ${MACRODIR}/data || error "invalid path"
MACROFILE=${1:?"provide relative macro path"}
test -f ${MACRODIR}/${MACROFILE} || error "invalid macro"
if test "$(dnsdomainname)" = "in2p3.fr"
then
    OUTPUTDIR=$(mktemp -d -p "${MACRODIR}" run.XXXX || error "can't create temp dir")
    ssh -i ${HOME}/.ssh/ccin2p3 linux1.dg.creatis.insa-lyon.fr mkdir -p "cc/$(basename ${OUTPUTDIR})"
else
    OUTPUTDIR=$(mktemp --tmpdir=${MACRODIR} -d run.XXXX || error "can't create temp dir")
fi
test -d ${OUTPUTDIR} || error "can't locate output dir"
RUNID=${OUTPUTDIR##*.}
NJOBS=${2:-"${DEFAULTNUMBEROFJOBS}"}
NJOBSMAX=${NJOBS}
PARAM="${4:-\"\"}"

echo "Lets roll!!"
echo "runid is ${RUNID}"

QSUB=$(which qsub 2> /dev/null)
# echo "qsub is $(which qsub)"
test -z "${QSUB}" && QSUB="noqsub"
if test "${QSUB}" = "noqsub"
then
    echo "qsub is not found. Simply run Gate on multiple cores."
fi

test -z "${PARAM}" && echo "no param" || echo "param is ${PARAM}"
if test "$RELEASESUFFIX" = "$DEFAULTRELEASESUFFIX"
then
    echo "Gate is $(which Gate)"
else
    echo "Gate release is $(basename ${RELEASEDIR})"
fi
echo "submitting ${NJOBS} jobs"

PARAMFILE="${OUTPUTDIR}/params.txt"
echo "njobs = ${NJOBS}" >> "${PARAMFILE}"
echo "macro = ${MACROFILE}" >> "${PARAMFILE}"
test -z "${PARAM}" || echo "param = ${PARAM}" >> "${PARAMFILE}"

# Copy macros files (for log)
mkdir ${OUTPUTDIR}/mac
cp ${MACROFILE} ${OUTPUTDIR}/mac
files=`grep "control/execute" ${MACROFILE} | cut -d " " -f 2`
for i in $files
do
    cp $i ${OUTPUTDIR}/mac
done

while test $NJOBS -gt 0; do

    if test "${QSUB}" = "noqsub"
    then
        echo "Launching Gate log in ${OUTPUTDIR}/gate_${NJOBS}.log"
        PARAM=\"${PARAM}\" INDEX=${NJOBS} INDEXMAX=${NJOBSMAX} OUTPUTDIR=${OUTPUTDIR}  RELEASEDIR=${RELEASEDIR} MACROFILE=${MACROFILE} MACRODIR=${MACRODIR} PBS_JOBID="local_${NJOBS}" bash "${JOBFILE}" > ${OUTPUTDIR}/gate_${NJOBS}.log &
    elif test "$(dnsdomainname)" = "in2p3.fr"
    then
        PROJECTGROUP=creatis 
        qsub -o "${OUTPUTDIR}" \
             -N "gate.${RUNID}" \
             -v "PARAM=\"${PARAM}\",INDEX=${NJOBS},INDEXMAX=${NJOBSMAX},OUTPUTDIR=${OUTPUTDIR},RELEASEDIR=${RELEASEDIR},MACROFILE=${MACROFILE},MACRODIR=${MACRODIR}" \
             "${JOBFILE}" || error "submission error"
    else
	qsub -N "gatejob.${RUNID}" -o "${OUTPUTDIR}" \
	    -v "PARAM=${PARAM},INDEX=${NJOBS},INDEXMAX=${NJOBSMAX},OUTPUTDIR=${OUTPUTDIR},RELEASEDIR=${RELEASEDIR},MACROFILE=${MACROFILE},MACRODIR=${MACRODIR}" \
	    "${JOBFILE}" || error "submission error"
    fi

    let NJOBS--
done

echo "runid is ${RUNID}"
if test "$(dnsdomainname)" = "in2p3.fr"
then
    rsync -av --remove-source-files -e "ssh -i ${HOME}/.ssh/ccin2p3" ${OUTPUTDIR}/ "linux1.dg.creatis.insa-lyon.fr:./cc/$(basename ${OUTPUTDIR})" --exclude '.__afs*' --exclude "${OUTPUTDIR}/gatejob.*.o*"
fi
