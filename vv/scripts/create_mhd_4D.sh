#!/bin/sh

###############################################
# create_mhd_4D 	argument : repertoire #
###############################################
if [ $# -lt 1 ]
then
    echo "Usage: create_mhd_4D directory"
    exit 1
fi

cd $1
nbph=`ls -l *0.mhd | wc -l`
orig=`ls -1 *0.mhd | head -n 1`

cat $orig | sed "s/NDims = .*/NDims = 4/
		 s/TransformMatrix = .*/TransformMatrix = 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1/
		 /Offset/ s/.*/& 0/
		 /CenterOfRotation/ s/.*/& 0/
		 s/AnatomicalOrientation = .*/AnatomicalOrientation = ????/
		 /ElementSpacing/ s/.*/& 1/
		 /DimSize/ s/.*/& $nbph/
		 s/ElementDataFile = .*/ElementDataFile = LIST/" > CT_4D.mhd

ls -1 *0.raw >> CT_4D.mhd
cd ..
