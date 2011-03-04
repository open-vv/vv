#!/bin/bash

set -u

# print error message and exit immediately
programname="$(basename ${0})"
function error {
echo "${programname} **ERROR** $1"
exit 1
}

# ensure a valid proxy is present for at least one hour
# if no proxy is found, try to create a new one
# return 0 if a valid proxy is found or created
# else return voms-proxy-init error code
function ensure_proxy {
voms-proxy-info --exists -valid 1:0 > /dev/null && return 0 
voms-proxy-init --voms biomed -valid 24:00 || exit 1
}

# print prompt to ensure that the user want to continue further
# the user has to answer with 'y' to continue
function check_user {
prompt="${1:-is that correct?}"
read -p "${prompt} [y/n] " answer
test "${answer}" == "y" && return 0
test "${answer}" == "n" && return 1
check_user "${prompt}"
}

releaselfndir="/grid/biomed/creatis/fgate/releases/"
prefix="${USER:?"USER must be set"}_"
releasearchive="${1:?"provide path to release archive"}"
releasearchive="$(readlink -f "${releasearchive}")" # convert to absolute path
test -f ${releasearchive} || error "file ${releasearchive} doesn't exist"
releasename="${prefix}$(basename "${releasearchive}")"

ensure_proxy || error "no valid proxy"

echo "releasearchive=${releasearchive}"
echo "releasename=${releasename}"
echo "releaselfndir=${releaselfndir}"
check_user || exit 2

lcg-cr -v -d ccsrm02.in2p3.fr -l "lfn:${releaselfndir}${releasename}" "file:${releasearchive}"
