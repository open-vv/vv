
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

# checks if the lfn file exists
function file_exists {
lfnfile="${1:?"provide lfn to file"}"
lfc-ls ${lfnfile} 2>&1 > /dev/null
}

# upload file to grid storage element
# source can be a relative or an absolute path to the source file 
# dest must be the lfn to the target **file** (not the directory) 
# if dest already exists, it prompts the user for overwritting
function upload_file {
sourcefile=${1:?"provide source file"}
destlfn=${2:?"provide destination file lfn"}
sourcefile="$(readlink -f "${sourcefile}")" # convert to absolute path
test -f "${sourcefile}" || error "can't find ${sourcefile}"
file_exists ${destlfn} && ( \
	check_user "${destlfn} already exists. overwrite it?" || return 2 && \
	lcg-del -a "lfn:${destlfn}" || error "failed to delete ${destlfn}" \
) && \
lcg-cr -v -d ccsrm02.in2p3.fr -l "lfn:${destlfn}" "file:${sourcefile}" 
}


# common path used
lfnbase="/grid/biomed/creatis/fgate/"
lfnrelease="${lfnbase}releases/"
lfnworkflow="${lfnbase}"
lfngasw="${lfnbase}gasw/"
lfnscript="${lfnbase}bin/"

# define the prefix for uploaded file
# default to local machine username
prefix="${USER:?"USER must be set"}_"
