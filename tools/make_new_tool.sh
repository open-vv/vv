#!/bin/bash
cp clitkBinarizeImage.ggo clitk$1.ggo
cp clitkBinarizeImage.cxx clitk$1.cxx
cp clitkBinarizeImageGenericFilter.h clitk$1GenericFilter.h
cp clitkBinarizeImageGenericFilter.cxx clitk$1GenericFilter.cxx

if test "$(uname)" = "Darwin"
then
  sed -i "" "s/BinarizeImage/$1/g" clitk$1*
else
  sed -i "s/BinarizeImage/$1/g" clitk$1*
fi

