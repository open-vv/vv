#!/bin/bash
if [ -z "$1" ]
then
    echo "Usage: make_vv_class.sh classname"
    exit 1
fi
if [ -e vv"$1".cxx ]
then
    echo "Error:  vv$1.cxx already exists classname"
    exit 1
fi

cp vvTemplateFile.cxx vv$1.cxx
cp vvTemplateFile.h vv$1.h
sed -i "s/XXX/$1/g" vv$1.*
