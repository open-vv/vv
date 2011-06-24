#!/usr/bin/env bash

set -u 

function error {
echo "ERROR: $1"
exit 1
}

warning_count=0
function warning {
let "warning_count++"
echo "WARNING: $1"
}

function start_bar {
count_max="${1:?"provide count max"}"
}

function update_bar {
local count="${1:?"provide count"}"
local message="${2:?"provide message"}"
local percent=$(echo "100*${count}/${count_max}" | bc)
#printf "[%03d/%03d] %3d%% %-80.80s\r" ${count} ${count_max} ${percent} "${message}"
printf "[%03d/%03d] %3d%% %-80.80s\n" ${count} ${count_max} ${percent} "${message}"
}

function end_bar {
unset count_max
#echo -ne '\n'
}

rootMerger="clitkMergeRootFiles"
test -x "./clitkMergeRootFiles" && rootMerger="./clitkMergeRootFiles"

function merge_root {
local merged="$1"
shift
echo "  ${indent}entering root merger"
echo "  ${indent}merger is ${rootMerger}"
echo "  ${indent}creating ${merged}"
local count=0
local arguments=" -o ${merged}"
while test $# -gt 0
do
    local partial="$1"
    shift
    let count++
    local arguments=" -i ${partial} ${arguments}"
done
${rootMerger} ${arguments} > /dev/null || { warning "error while calling ${rootMerger}" && return; }
echo "  ${indent}merged ${count} files"
}

statMerger="mergeStatFile.py"
test -x "./mergeStatFile.sh" && statMerger="./mergeStatFile.sh"
test -x "./mergeStatFile.py" && statMerger="./mergeStatFile.py"

function merge_stat {
local merged="$1"
shift
echo "  ${indent}entering stat merger"
echo "  ${indent}merger is ${statMerger}"
echo "  ${indent}creating ${merged}"
local count=0
start_bar $#
while test $# -gt 0
do
    local partial="$1"
    shift
    let count++

    if test ! -f "${merged}"
    then
        update_bar ${count} "copying first partial result ${partial}"
        cp "${partial}" "${merged}"
        continue
    fi

    update_bar ${count} "adding ${partial}"
    ${statMerger} -i "${merged}" -j "${partial}" -o "${merged}" 2> /dev/null > /dev/null || { warning "error while calling ${statMerger}" && return; }
done
end_bar
echo "  ${indent}merged ${count} files"
}

txtImageMerger="clitkMergeAsciiDoseActor"
test -f "./clitkMergeAsciiDoseActor" && txtImageMerger="./clitkMergeAsciiDoseActor"

function merge_txt_image {
local merged="$1"
shift
echo "  ${indent}entering text image merger"
echo "  ${indent}merger is ${txtImageMerger}"
echo "  ${indent}creating ${merged}"
local count=0
start_bar $#
while test $# -gt 0
do
    local partial="$1"
    shift
    let count++

    if test ! -f "${merged}"
    then
        update_bar ${count} "copying first partial result ${partial}"
        cp "${partial}" "${merged}"
        continue
    fi

    update_bar ${count} "adding ${partial}"
    local header="$(cat "${merged}" | head -n 6)"
    local tmp="$(mktemp)"
    ${txtImageMerger} -i "${partial}" -j "${merged}" -o "${tmp}" 2> /dev/null > /dev/null || { warning "error while calling ${txtImageMerger}" && return; }
    echo "${header}" > "${merged}"
    grep -v '## Merge' "${tmp}" >> "${merged}"
    rm "${tmp}"
done
end_bar
echo "  ${indent}merged ${count} files"
}

hdrImageMerger="clitkImageArithm"
test -x "./clitkImageArithm" && hdrImageMerger="./clitkImageArithm"

function merge_hdr_image {
local merged="$1"
local merged_bin="${merged%.*}.img"
shift
echo "  ${indent}entering hdr image merger"
echo "  ${indent}merger is ${hdrImageMerger}"
echo "  ${indent}creating ${merged}"
local count=0
start_bar $#
while test $# -gt 0
do
    local partial="$1"
    local partial_bin="${partial%.*}.img"
    shift
    let count++

    if test ! -f "${merged}"
    then
        update_bar ${count} "copying first partial result ${partial}"
        cp "${partial}" "${merged}"
        cp "${partial_bin}" "${merged_bin}"
        continue
    fi

    update_bar ${count} "adding ${partial}"
    ${hdrImageMerger} -t 0 -i "${partial}" -j "${merged}" -o "${merged}" 2> /dev/null > /dev/null || { warning "error while calling ${hdrImageMerger}" && return; }
done
end_bar
echo "  ${indent}merged ${count} files"
}

function merge_dispatcher {
    local indent="  ** "
    local outputfile="${1:?"provide output filename"}"
    echo "merging ${outputfile}"

    local partialoutputfiles="$(find "${rundir}" -type f -name "${outputfile}")"
    local nboutputfiles="$(echo "${partialoutputfiles}" | wc -l)"
    if test ${nboutputdirs} -ne ${nboutputfiles}
    then
        warning "missing files"
        return
    fi

    local firstpartialoutputfile="$(echo "${partialoutputfiles}" | head -n 1)"
    local firstpartialoutputextension="${firstpartialoutputfile##*.}"
    echo "${indent}testing file type on ${firstpartialoutputfile}"

    if test "${firstpartialoutputextension}" == "hdr"
    then
        echo "${indent}this is a analyse image"
        local mergedfile="${outputdir}/$(basename "${firstpartialoutputfile}")"
        merge_hdr_image "${mergedfile}" ${partialoutputfiles} || error "error while merging"
        return
    fi

    if test "${firstpartialoutputextension}" == "root"
    then
        echo "${indent}this is a root file"
        local mergedfile="${outputdir}/$(basename "${firstpartialoutputfile}")"
        merge_root "${mergedfile}" ${partialoutputfiles} || error "error while merging"
        return
    fi

    if test "${firstpartialoutputextension}" == "txt" && grep 'NumberOfEvent' "${firstpartialoutputfile}" > /dev/null
    then
        echo "${indent}this is a stat file"
        local mergedfile="${outputdir}/$(basename "${firstpartialoutputfile}")"
        merge_stat "${mergedfile}" ${partialoutputfiles} || error "error while merging"
        return
    fi

    if test "${firstpartialoutputextension}" == "txt" && grep 'Resol' "${firstpartialoutputfile}" > /dev/null
    then
        local resol="$(sed -nr '/Resol/s/^.*=\s+\((.+)\)\s*$/\1/p' "${firstpartialoutputfile}")"
        local resolx="$(echo "${resol}" | cut -d',' -f1)"
        local resoly="$(echo "${resol}" | cut -d',' -f2)"
        local resolz="$(echo "${resol}" | cut -d',' -f3)"
        if test "${resol}" == "1,1,1"
        then
            echo "${indent}this is a txt integral value"
            local mergedfile="${outputdir}/$(basename "${firstpartialoutputfile}")"
            merge_txt_image "${mergedfile}" ${partialoutputfiles} || error "error while merging"
            return
        fi
        if test \( "${resolx}" == "1" -a "${resoly}" == "1" \) -o \( "${resoly}" == "1" -a "${resolz}" == "1" \) -o \( "${resolz}" == "1" -a "${resolx}" == "1" \)
        then
            echo "${indent}this is a txt profile"
            local mergedfile="${outputdir}/$(basename "${firstpartialoutputfile}")"
            merge_txt_image "${mergedfile}" ${partialoutputfiles} || error "error while merging"
            return
        fi
    fi

    warning "unknown file type"
}

rundir="${1?"provide run dir"}"
nboutputdirs="$(find "${rundir}" -mindepth 1 -type d -name 'output*' | wc -l)"

test ${nboutputdirs} -gt 0 || error "no output dir found"
echo "found ${nboutputdirs} partial output dirs"

outputdir="results"
if [ "${rundir}" != "." -a "${rundir##*.}" != "${rundir}" ]
then
    outputdir="results.${rundir##*.}"
fi
outputdir="$(basename "${outputdir}")"
echo "output dir is ${outputdir}"
test -d "${outputdir}" && rm -r "${outputdir}"
mkdir "${outputdir}"

for outputfile in $(find "${rundir}" -regextype 'posix-extended' -type f -regex '.*\.(hdr|root|txt)' | awk -F '/' '{ print $NF }' | sort | uniq)
do
    merge_dispatcher "${outputfile}"
done

echo "these was ${warning_count} warning(s)"
