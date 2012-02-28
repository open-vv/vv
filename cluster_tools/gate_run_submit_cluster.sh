#!/usr/bin/env bash 

set -u
SCRIPTNAME="$(basename "${0}")"

function error {
echo "ERROR: $1"
usage
exit 1
}

DEFAULTRELEASESUFFIX="06z"
DEFAULTNUMBEROFJOBS="10"

function usage {
echo "${SCRIPTNAME} mac/main.mac njobs releasesuffix paramtogate"
echo "default njobs = ${DEFAULTNUMBEROFJOBS}"
echo "default releasesuffix = ${DEFAULTRELEASESUFFIX}"
}

test $# -eq 0 && usage && exit 0

SCRIPTDIR="${HOME}/git/gate-tests/bin"
RELEASEDIR="${HOME}/releases/grid_release${3:-"${DEFAULTRELEASESUFFIX}"}"
JOBFILE="${HOME}/git/gate-tests/job_cluster.job"

echo "Checking stuff"
test -f ${JOBFILE} || error "can't find job file ${JOBFILE}"
test -d ${RELEASEDIR} || error "invalid release dir ${RELEASEDIR}"
MACRODIR=$(pwd)
test -d ${MACRODIR}/mac && test -d ${MACRODIR}/data || error "invalid path"
MACROFILE=${1:?"provide relative macro path"}
test -f ${MACRODIR}/${MACROFILE} || error "invalid macro"
OUTPUTDIR=$(mktemp --tmpdir=${MACRODIR} -d run.XXXX || error "can't create temp dir")
test -d ${OUTPUTDIR} || error "can't locate output dir"
RUNID=${OUTPUTDIR##*.}
NJOBS=${2:-"${DEFAULTNUMBEROFJOBS}"}
NJOBSMAX=${NJOBS}
PARAM="${4:-""}"

echo "Lets roll!!"
echo "runid is ${RUNID}"
echo "qsub is $(which qsub)"
test -z "${PARAM}" && echo "no param" || echo "param is ${PARAM}"
echo "using release $(basename ${RELEASEDIR})"
echo "submitting ${NJOBS} jobs"

PARAMFILE="${OUTPUTDIR}/params.txt"
echo "njobs = ${NJOBS}" >> "${PARAMFILE}"
echo "macro = ${MACROFILE}" >> "${PARAMFILE}"
test -z "${PARAM}" || echo "param = ${PARAM}" >> "${PARAMFILE}"

while test $NJOBS -gt 0; do
	qsub -N "gatejob.${RUNID}" -o "${OUTPUTDIR}" \
	-v "PARAM=\"${PARAM}\",INDEX=${NJOBS},INDEXMAX=${NJOBSMAX},SCRIPTDIR=${SCRIPTDIR},OUTPUTDIR=${OUTPUTDIR},RELEASEDIR=${RELEASEDIR},MACROFILE=${MACROFILE},MACRODIR=${MACRODIR}" \
	"${JOBFILE}" || error "submission error"
	let NJOBS--
done

echo "runid is ${RUNID}"
