#!/usr/bin/env bash

set -u

function error {
    echo "ERROR: $1"
    exit 1
}

warning_count=0
function warning {
    let "warning_count++"
    echo "MERGE_WARNING: $1"
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

function check_interfile {
    local input_interfile="${1:?"provide input interfile"}"

    grep -qs '!INTERFILE :=' "${input_interfile}" || return 1

    local header_byte_size=$(awk -F' ' '
BEGIN { zsize = 0; }
/matrix size/ && $3 == "[1]" { xsize = $5; }
/matrix size/ && $3 == "[2]" { ysize = $5; }
/number of projections/ { zsize += $5; }
/number of bytes per pixel/ { byte_per_pixel = $7; }
END { print xsize * ysize * zsize * byte_per_pixel; }' "${input_interfile}")

    local raw_interfile="$(dirname "${input_interfile}")/$(awk -F' := ' '/name of data file/ { print $2; }' "${input_interfile}")"

    test -f "${raw_interfile}" || return 1
    test $(stat -c%s "${raw_interfile}") -eq ${header_byte_size} || return 1
}

function write_mhd_header {
    local input_interfile="${1:?"provide input interfile"}"
    local output_mhd="$(dirname "${input_interfile}")/$(basename "${input_interfile}" ".hdr").mhd"

    check_interfile "${input_interfile}" || error "${input_interfile} isn't an interfile image"

    local header_start='ObjectType = Image
NDims = 3
AcquisitionDate = none
BinaryData = True
BinaryDataByteOrderMSB = False
CompressedData = False
TransformMatrix = 1 0 0 0 1 0 0 0 1
Offset = 0 0 0
CenterOfRotation = 0 0 0
DistanceUnits = mm
AnatomicalOrientation = RIP'

    echo "${header_start}" > "${output_mhd}"

    awk -F' ' '
/scaling factor/ && $4 == "[1]" { xspacing = $6; }
/scaling factor/ && $4 == "[2]" { yspacing = $6; }
END { print "ElementSpacing = " xspacing " " yspacing " 1"; }' "${input_interfile}" >> "${output_mhd}"

    awk -F' ' '
BEGIN { zsize = 0; }
/matrix size/ && $3 == "[1]" { xsize = $5; }
/matrix size/ && $3 == "[2]" { ysize = $5; }
/number of projections/ { zsize += $5; }
END { print "DimSize = " xsize " " ysize " " zsize; }' "${input_interfile}" >> "${output_mhd}"

    awk -F' := ' '
/number format/ { format = $2; }
/number of bytes per pixel/ { byte_per_pixel = $2 }
END {
if (format == "unsigned integer" && byte_per_pixel == 8) { print "ElementType = MET_ULONG"; exit };
if (format == "unsigned integer" && byte_per_pixel == 4) { print "ElementType = MET_UINT"; exit };
if (format == "unsigned integer" && byte_per_pixel == 2) { print "ElementType = MET_USHORT"; exit };
if (format == "unsigned integer" && byte_per_pixel == 1) { print "ElementType = MET_UCHAR"; exit };
if (format == "integer" && byte_per_pixel == 8) { print "ElementType = MET_LONG"; exit };
if (format == "integer" && byte_per_pixel == 4) { print "ElementType = MET_INT"; exit };
if (format == "integer" && byte_per_pixel == 2) { print "ElementType = MET_SHORT"; exit };
if (format == "integer" && byte_per_pixel == 1) { print "ElementType = MET_CHAR"; exit };
if (format == "float" && byte_per_pixel == 8) { print "ElementType = MET_FLOAT"; exit };
if (format == "float" && byte_per_pixel == 4) { print "ElementType = MET_DOUBLE"; exit };
print "ElementType = MET_INT";
}' "${input_interfile}" >> "${output_mhd}"

    awk -F' := ' '
/name of data file/ { print "ElementDataFile = " $2; }' "${input_interfile}" >> "${output_mhd}"
}

rootMerger="clitkMergeRootFiles"
test -x "./clitkMergeRootFiles" && rootMerger="./clitkMergeRootFiles"

function merge_root {
    local merged="$1"
    shift
    echo "  ${indent}entering root merger"
    echo "  ${indent}merger is ${rootMerger}"
    echo "  ${indent}creating ${merged}"
    #echo "######## $#"
    #echo "######## $*"

    if test $# -eq 1
    then
        echo "  ${indent}just one partial file => just copy it"
        cp "$1" "${merged}"
        return
    fi

    local count=0
    local arguments=" -o ${merged}"
    while test $# -gt 0
    do
        local partial="$1"
        shift
        let count++
        local arguments=" -i ${partial} ${arguments}"
    done
    ${rootMerger} ${arguments} > /dev/null || error "error while calling ${rootMerger}"
    echo "  ${indent}merged ${count} files"
}

statMerger="mergeStatFile.py"
test -x "./mergeStatFile.sh" && statMerger="./mergeStatFile.sh"

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
        ${statMerger} -i "${merged}" -j "${partial}" -o "${merged}" 2> /dev/null > /dev/null || warning "error while calling ${statMerger}"
    done
    end_bar
    echo "  ${indent}merged ${count} files"
}

doseMerger="mergeDosePerEnegryFile.sh"
test -x "./mergeDosePerEnergyFile.sh" && doseMerger="./mergeDosePerEnergyFile.sh"

function merge_dose {
    local merged="$1"
    shift
    echo "  ${indent}entering dose merger"
    echo "  ${indent}merger is ${doseMerger}"
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
        ${doseMerger} -i "${merged}" -j "${partial}" -o "${merged}" 2> /dev/null > /dev/null || warning "error while calling ${doseMerger}"
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
        ${txtImageMerger} -i "${partial}" -j "${merged}" -o "${tmp}" 2> /dev/null > /dev/null || warning "error while calling ${txtImageMerger}"
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
        ${hdrImageMerger} -t 0 -i "${partial}" -j "${merged}" -o "${merged}" 2> /dev/null > /dev/null || warning "error while calling ${hdrImageMerger}"
    done
    end_bar
    echo "  ${indent}merged ${count} files"
}

mhdImageMerger="clitkImageArithm"
test -x "./clitkImageArithm" && mhdImageMerger="./clitkImageArithm"

function merge_mhd_image {
    local merged="$1"
    local merged_bin="${merged%.*}.raw"
    local last_character=${merged#${merged%?}}
    shift
    echo "  ${indent}entering mhd image merger"
    echo "  ${indent}merger is ${mhdImageMerger}"
    echo "  ${indent}creating ${merged}"
    local count=0
    start_bar $#
    while test $# -gt 0
    do
        local partial="$1"
        local partial_bin="$(dirname "${partial}")/$(awk -F' = ' '/ElementDataFile/ { print $2; }' "${partial}")"
        shift
        let count++

        if test ! -f "${merged}"
        then
            update_bar ${count} "copying first partial result ${partial}"
            cp "${partial}" "${merged}"
            if test "$last_character" = "d"
            then
                cp "${partial_bin}" "${merged_bin%.*}.${partial_bin##*.}"
            fi
            continue
        fi

        update_bar ${count} "adding ${partial}"
        ${mhdImageMerger} -t 0 -i "${partial}" -j "${merged}" -o "${merged}" 2> /dev/null > /dev/null || warning "error while calling ${mhdImageMerger}"
        if test "$last_character" = "d" && test "${merged_bin}" != "${merged_bin%.*}.${partial_bin##*.}"
        then
            mv "${merged_bin}" "${merged_bin%.*}.${partial_bin##*.}"
            sed -i "s/$(basename "${merged_bin}")/$(basename "${merged_bin%.*}.${partial_bin##*.}")/" "${merged}"
        fi
    done
    end_bar
    echo "  ${indent}merged ${count} files"
}

function merge_dispatcher {
    local indent="  ** "
    local outputfile="${1:?"provide output filename"}"
    echo "merging ${outputfile}"

    local partialoutputfiles="$(find -L "${rundir}" -mindepth 2 -type f -name "${outputfile}")"
    local nboutputfiles="$(echo "${partialoutputfiles}" | wc -l)"
    if test ${nboutputdirs} -ne ${nboutputfiles}
    then
        warning "missing files"
        return
    fi

    local firstpartialoutputfile="$(echo "${partialoutputfiles}" | head -n 1)"
    local firstpartialoutputextension="${firstpartialoutputfile##*.}"
    echo "${indent}testing file type on ${firstpartialoutputfile}"

    if test "${firstpartialoutputextension}" == "hdr" && grep -qs 'INTERFILE' "${firstpartialoutputfile}"
    then
        echo "${indent}this is a interfile image"
        echo "${indent}creating mhd headers"
        for partialoutputfile in $partialoutputfiles; do write_mhd_header "${partialoutputfile}"; done
        local mhd_partialoutputfiles="$(for partialoutputfile in $partialoutputfiles; do echo "${partialoutputfile%.*}.mhd"; done)"
        local mhd_mergedfile="${outputdir}/$(basename "${firstpartialoutputfile}" ".hdr").mhd"
        merge_mhd_image "${mhd_mergedfile}" ${mhd_partialoutputfiles} || error "error while merging"
        echo "${indent}cleaning mhd headers"
        for mhd_partialoutputfile in $mhd_partialoutputfiles; do rm "${mhd_partialoutputfile}"; done
        rm "${mhd_mergedfile}"
        echo "${indent}copy interfile header"
        cp "${firstpartialoutputfile}" "${outputdir}"
        return
    fi

    if test "${firstpartialoutputextension}" == "hdr"
    then
        echo "${indent}this is a analyse image"
        local mergedfile="${outputdir}/$(basename "${firstpartialoutputfile}")"
        merge_hdr_image "${mergedfile}" ${partialoutputfiles} || error "error while merging"
        return
    fi

    if test "${firstpartialoutputextension}" == "mhd" || test "${firstpartialoutputextension}" == "mha"
    then
        echo "${indent}this is a mhd image"
        local mergedfile="${outputdir}/$(basename "${firstpartialoutputfile}")"
        merge_mhd_image "${mergedfile}" ${partialoutputfiles} || error "error while merging"
        return
    fi

    if test "${firstpartialoutputextension}" == "root"
    then
        echo "${indent}this is a root file"
        local mergedfile="${outputdir}/$(basename "${firstpartialoutputfile}")"
        merge_root "${mergedfile}" ${partialoutputfiles} || error "error while merging"
        return
    fi

    if test "${firstpartialoutputextension}" == "txt" && grep -qs 'NumberOfEvent' "${firstpartialoutputfile}"
    then
        echo "${indent}this is a stat file"
        local mergedfile="${outputdir}/$(basename "${firstpartialoutputfile}")"
        merge_stat "${mergedfile}" ${partialoutputfiles} || error "error while merging"
        return
    fi

    if test "${firstpartialoutputextension}" == "txt" && grep -qs 'energydose' "${firstpartialoutputfile}"
    then
        echo "${indent}this is a dose file"
        local mergedfile="${outputdir}/$(basename "${firstpartialoutputfile}")"
        merge_dose "${mergedfile}" ${partialoutputfiles} || error "error while merging"
        return
    fi



    if test "${firstpartialoutputextension}" == "txt" && grep -qs 'Resol' "${firstpartialoutputfile}"
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

    if test "${firstpartialoutputextension}" == "txt"
    then
        echo "${indent}this is a non specific txt output"
        local mergedfile="${outputdir}/$(basename "${firstpartialoutputfile}")"
        local nbdifferent="$(md5sum ${partialoutputfiles} | awk '{ print $1; }' | sort | uniq | wc -l)"
        echo "  ${indent}${nbdifferent} different files"
        if test ${nbdifferent} -gt 1
        then
            echo "  ${indent}catting to ${mergedfile}"
            cat ${partialoutputfiles} > "${mergedfile}" || error "error while merging"
            return
        else
            echo "  ${indent}moving to ${mergedfile}"
            cp "${firstpartialoutputfile}" "${mergedfile}" || error "error while merging"
            return
        fi
    fi

    error "unknown file type"
}

function merge_dispatcher_uncertainty {
    local indent="  ** "
    local outputfile="${1:?"provide output filename"}"

    local partialoutputfiles="$(find -L "${rundir}" -mindepth 2 -type f -name "${outputfile}")"
    local nboutputfiles="$(echo "${partialoutputfiles}" | wc -l)"
    if test ${nboutputdirs} -ne ${nboutputfiles}
    then
        warning "missing files"
        return
    fi

    local firstpartialoutputfile="$(echo "${partialoutputfiles}" | head -n 1)"
    local firstpartialoutputextension="${firstpartialoutputfile##*.}"

    if [[ "${firstpartialoutputfile}" == *Uncertainty* ]]
    then
    	if test "${firstpartialoutputextension}" == "mhd" || test "${firstpartialoutputextension}" == "mha"
        then
            echo "${indent}Uncertainty file found: ${firstpartialoutputfile}"
            ## search for sum
            local mergedfile="${outputdir}/$(basename "${firstpartialoutputfile}")"
            summed_merged_file=${mergedfile//-Uncertainty/}
            if [ ! -f ${summed_merged_file} ];
            then
                warning "${summed_merged_file} does not exist. Error, no uncertainty computed"
                return;
            fi
            echo "${indent}${summed_merged_file} found"
            ## search for Squared
            squared_merged_file=${mergedfile//-Uncertainty/-Squared}
            if [ ! -f ${squared_merged_file} ];
            then
                warning "${squared_merged_file} does not exist. Error, no uncertainty computed"
                return;
            fi
            echo "${indent}${squared_merged_file} found"
            ## search for NumberOfEvent
            totalEvents=0;
            for outputfile in $(find -L "${rundir}" -regextype 'posix-extended' -type f -regex "${rundir}/output.*\.(hdr|mhd|mha|root|txt)" | awk -F '/' '{ print $NF; }' | sort | uniq)
            do
                #echo $outputfile
                if grep -q 'NumberOfEvent' "${outputdir}/${outputfile}"
                then
                    totalEvents="$(grep "NumberOfEvents" "${outputdir}/${outputfile}" | cut -d' ' -f4)"
                    echo "${indent}Find the NumberOfEvent in $outputfile: ${totalEvents}"
                fi
            done

            if test ${totalEvents} -gt 0
            then
                uncerImageMerger="clitkImageUncertainty"
                test -x "./clitkImageUncertainty" && uncerImageMerger="./clitkImageUncertainty"
                ${uncerImageMerger} -i ${summed_merged_file} -s ${squared_merged_file} -o ${mergedfile} -n ${totalEvents}
            else
                warning "${totalEvents} not positive. A at least one stat file (SimulationStatisticActor) must be provided. Error, no uncertainty computed"
                return;
            fi
	else
            error "merge_dispatcher_uncertainty does not handle ${firstpartialoutputfile} files"
        fi
    fi

}

echo "!!!! this is $0 v0.3k !!!!"

rundir="${1?"provide run dir"}"
rundir="$(echo "${rundir}" | sed 's|/*$||')"
nboutputdirs="$(find "${rundir}" -mindepth 1 -type d -name 'output*' -o -type l -name 'output*' | wc -l)"

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
for outputfile in $(find -L "${rundir}" -regextype 'posix-extended' -type f -regex "${rundir}/output.*\.(hdr|mhd|mha|root|txt)" | awk -F '/' '{ print $NF; }' | sort | uniq)
do
    merge_dispatcher "${outputfile}"
done

echo ""
echo "Merging done. Special case for statistical uncertainty"
for outputfile in $(find -L "${outputdir}" -regextype 'posix-extended' -type f -regex "${outputdir}/.*\.(hdr|mhd|mha|root|txt)" | awk -F '/' '{ print $NF; }' | sort | uniq)
do
    merge_dispatcher_uncertainty "${outputfile}"
done

if [ -f "${rundir}/params.txt" ]
then
    echo "copying params file"
    cp "${rundir}/params.txt" "${outputdir}/params.txt"
fi

if [ -d "${rundir}/mac" ]
then
    echo "copying mac folder"
    cp -r "${rundir}/mac" "${outputdir}/mac"
fi

echo "these was ${warning_count} warning(s)"
