#!/bin/sh


#####################################################################
# create_mhd_4D 	argument : repertoire nom_fichier_de_sortie #
#####################################################################
if [ $# -lt 1 ]
then
    echo "Usage: create_mhd_4D.sh directory output_file_name"
    exit 1
fi

cd $1
nbph=`find -iname *0.mhd | wc -l`
if [ $nbph = 0 ]
then
    echo "Error: no phase found"
    exit 1
fi
orig=`ls -1 *0.mhd | head -n 1`

cat $orig | sed "s/NDims = .*/NDims = 4/
		 s/TransformMatrix = .*/TransformMatrix = 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1/
		 /Offset/ s/.*/& 0/
		 /CenterOfRotation/ s/.*/& 0/
		 s/AnatomicalOrientation = .*/AnatomicalOrientation = ????/
		 /ElementSpacing/ s/.*/& 1/
		 /DimSize/ s/.*/& $nbph/
		 s/ElementDataFile = .*/ElementDataFile = LIST/" > $2

ls -1 *0.raw >> $2
cd ..
