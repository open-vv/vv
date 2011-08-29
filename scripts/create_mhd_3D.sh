#!/bin/sh


#################################################################################
# create_mhd_3D 	argument : {image data} raw_image nom_fichier_de_sortie #
#################################################################################
if [ $# -lt 1 ] 
then
    echo "Usage: create_mhd_3D.sh dimx dimy dimz spcx spcy spcz offx offy offz pixel_type raw_image_file output_file"
    echo "dim*: dimensions of the image"
    echo "spc*: pixel spacing along each dimension"
    echo "off*: offset along each dimension"
    echo "pixel_type: CHAR, UCHAR, SHORT, USHORT, FLOAT"
    echo "raw_image_file: image to be referenced by the mhd file created"
    echo "output_file: mhd to be created"
    exit 1
fi

# can point to existing raw files
n=`ls ${11} | wc -l`
if [ $n -eq 0 ] 
then
    echo "${11} does not exist. Cannot create mhd file."
    exit 
fi

# check if the raw file has the "raw" extension
n=`ls ${11} | grep .raw | wc -l`
if [ $n -eq 0 ] 
then
    # change extension to raw
    raw_base=`echo ${11} | cut -d . -f 1`; 
    raw_file=$raw_base".raw"; 
    mv ${11} $raw_file
else
    raw_file=${11}
fi

# create file (with some default values...)
echo "NDims = 3" > ${12}
echo "TransformMatrix = 1 0 0 0 1 0 0 0 1" >> ${12}
echo "Offset = " $7 $8 $9 >> ${12}
echo "CenterOfRotation = 0 0 0" >> ${12}
echo "AnatomicalOrientation = RAI" >> ${12}
echo "ElementSpacing = " $4 $5 $6 >> ${12}
echo "DimSize = " $1 $2 $3 >> ${12}
echo "ElementType = MET_"${10} >> ${12}
echo "ElementDataFile = " $raw_file >> ${12}


