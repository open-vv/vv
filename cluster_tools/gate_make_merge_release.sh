#!/bin/bash

set -u
set -e

function error {
echo "ERROR: $1"
echo "$(basename $0)"
exit 1
}

function get_deps {
targetbinary=${1}
targetdir=${2}
test -d ${targetdir} || error "${targetdir} isn't a directory"
ldd ${targetbinary} | while read library; do
	libfile="$(echo ${library} | awk -F' ' '/=> \// {print $3}')"
	test $libfile || continue # didn't macht regex
	test -f "${targetdir}/$(basename ${libfile})" && continue # already exists
	cp "${libfile}" "${targetdir}"
	get_deps "${libfile}" "${targetdir}"
done
}

function package_target {
targetname="${1}"
which "${targetname}" > /dev/null || error "cant locate ${targetname}"
targetbin="$(which ${targetname})"
echo "${targetname} executable is ${targetbin}"

echo "Getting libraries"
targetdir="$(mktemp -d)"
get_deps "${targetbin}" "${targetdir}"

echo "Removing unused libraries"
rm -f ${targetdir}/libdl.so*
rm -f ${targetdir}/libm.so*
rm -f ${targetdir}/libstdc++.so*
rm -f ${targetdir}/libgcc_s.so*
rm -f ${targetdir}/libpthread.so*
rm -f ${targetdir}/libc.so*

echo "Copying binary"
cp "${targetbin}" .
for filename in $(find ${targetdir} -name '*.so*'); do cp ${filename} . ; done

echo "Cleaning up"
rm -r "${targetdir}"
}

filenames=("clitkImageArithm" "clitkMergeRootFiles" "clitkMergeAsciiDoseActor" "clitkImageUncertainty" "mergeStatFile.sh" "gate_power_merge.sh")

for input in "${filenames[@]}"; do
	package_target "${input}" || error "error while packaging ${input}"
done

echo "Making release"
tar -czvf merge_release.tar.gz ** \
        || usage "can't create release zip"



