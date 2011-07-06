#!/bin/sh
#
# arg[1] is TESTING_BINARY_DIR
# This script has been edited from itk/Modules/ThirdParty/NIFTI/src/nifti/Testing/nifti_regress_test/cmake_testscripts/fetch_data_test.sh
if cd $1
then
  git pull
else
  git clone $2 $1
fi
exit 0

