#!/bin/bash
cp clitkSplitImage.ggo clitk$1.ggo
cp clitkSplitImage.cxx clitk$1.cxx
cp clitkSplitImageGenericFilter.h clitk$1GenericFilter.h
cp clitkSplitImageGenericFilter.txx clitk$1GenericFilter.txx
cp clitkSplitImageGenericFilter.cxx clitk$1GenericFilter.cxx

sed -i "s/SplitImage/$1/g" clitk$1*
