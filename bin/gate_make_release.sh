#!/bin/bash

function usage {
echo "ERROR: $1"
echo "$(basename $0) [name]"
exit 1
}

GATENAME="${1:-Gate}"
which "${GATENAME}" > /dev/null || usage "cant locate ${GATENAME} binary"
GATEBIN="$(which ${GATENAME})"
echo "${GATENAME} executable is ${GATEBIN}"

test -d "${G4NEUTRONHPDATA}" || usage "can't locate neutron data. please set G4NEUTRONHPDATA"
echo "neutron data is ${G4NEUTRONHPDATA}"
test -d "${G4LEVELGAMMADATA}" || usage "can't locate gamma data. please set G4LEVELGAMMADATA"
echo "gamma data is ${G4LEVELGAMMADATA}"
test -d "${G4RADIOACTIVEDATA}" || usage "can't locate radioactivity data. please set G4RADIOACTIVEDATA"
echo "radioactivity data is ${G4RADIOACTIVEDATA}"
test -d "${G4ABLADATA}" || usage "can't locate abla data. please set G4ABLADATA"
echo "abla data is ${G4ABLADATA}"
test -d "${G4LEDATA}" || usage "can't locate em data. please set G4LEDATA"
echo "em data is ${G4LEDATA}"
test -d "${G4REALSURFACEDATA}" || usage "can't locate surface data. please set G4REALSURFACEDATA"
echo "surface data is ${G4REALSURFACEDATA}"
test -d "${G4NEUTRONXSDATA}" || usage "can't locate neutron xs data. please set G4NEUTRONXSDATA"
echo "neutron xs data is ${G4NEUTRONXSDATA}"
test -d "${G4PIIDATA}" || usage "can't locate pii data. please set G4PIIDATA"
echo "pii data is ${G4PIIDATA}"

echo "Cleaning previous build"
rm -fr $(basename ${G4NEUTRONHPDATA})
rm -fr $(basename ${G4LEVELGAMMADATA})
rm -fr $(basename ${G4RADIOACTIVEDATA})
rm -fr $(basename ${G4ABLADATA})
rm -fr $(basename ${G4LEDATA})
rm -fr $(basename ${G4REALSURFACEDATA})
rm -fr test_libs fgate_shared_libs.tar.gz fgate_release.tar.gz

echo "Copying libraries"
function get_deps {
ldd $1 | while read library; do
	libfile="$(echo ${library} | awk -F' ' '/=> \// {print $3}')"
	test $libfile || continue # didn't macht regex
	test -f "test_libs/$(basename ${libfile})" && continue # already exists
	echo "${libfile}"
	cp "${libfile}" "test_libs/$(basename ${libfile})"
	get_deps "${libfile}"
done
}

mkdir test_libs
get_deps "${GATEBIN}"

echo "Removing unused libraries"
rm -f test_libs/libdl.so*
rm -f test_libs/libm.so*
rm -f test_libs/libstdc++.so*
rm -f test_libs/libgcc_s.so*
rm -f test_libs/libpthread.so*
rm -f test_libs/libc.so*

echo "Zipping libraries"
(
	cd test_libs
	tar -czvf ../fgate_shared_libs.tar.gz **
) || usage "can't create libraries tar"

echo "Copying binary"
cp "${GATEBIN}" .

echo "Copying data"
cp -r "${G4NEUTRONHPDATA}" .
cp -r "${G4LEVELGAMMADATA}" .
cp -r "${G4RADIOACTIVEDATA}" .
cp -r "${G4ABLADATA}" .
cp -r "${G4LEDATA}" .
cp -r "${G4REALSURFACEDATA}" .
cp -r "${G4NEUTRONXSDATA}" .
cp -r "${G4PIIDATA}" .

echo "Making release"
tar -czvf fgate_release.tar.gz \
	${GATENAME} fgate_shared_libs.tar.gz \
	$(basename ${G4NEUTRONHPDATA}) \
	$(basename ${G4LEVELGAMMADATA}) \
	$(basename ${G4RADIOACTIVEDATA}) \
	$(basename ${G4ABLADATA}) \
	$(basename ${G4LEDATA}) \
	$(basename ${G4REALSURFACEDATA}) \
	$(basename ${G4NEUTRONXSDATA}) \
	$(basename ${G4PIIDATA}) \
        || usage "can't create release zip"



