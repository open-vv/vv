#!/bin/bash

. common.sh

releasearchive="${1:?"provide path to release archive"}"
test -f ${releasearchive} || error "file ${releasearchive} doesn't exist"
releasename="${prefix}$(basename "${releasearchive}")"

ensure_proxy || error "no valid proxy"

echo "releasearchive=${releasearchive}"
echo "releasename=${releasename}"
echo "lfnrelease=${lfnrelease}"
check_user || exit 2

upload_file "${releasearchive}" "${lfnrelease}${releasename}" && echo "success" || echo "failed"
