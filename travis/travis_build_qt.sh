#!/bin/bash
set -ev

#Install the correct Qt Version
if [ "$QT_VERSION" == "4.8.7" ]; then
  if test $TRAVIS_OS_NAME == linux ; then sudo apt-get -y --force-yes install qt4-dev-tools; fi
  
  if test $TRAVIS_OS_NAME == osx ; then brew install cartr/qt4/qt; fi
  if test $TRAVIS_OS_NAME == osx ; then brew link qt --force; fi
  if test $TRAVIS_OS_NAME == osx ; then qmake -v; fi
elif [ "$QT_VERSION" == "5.5.1" ]; then
  if test $TRAVIS_OS_NAME == linux ; then sudo add-apt-repository --yes ppa:beineri/opt-qt551-trusty; fi
  if test $TRAVIS_OS_NAME == linux ; then sudo apt-get update -qq; fi
  if test $TRAVIS_OS_NAME == linux ; then sudo apt-get -y --force-yes install qt55tools; fi
  
  if test $TRAVIS_OS_NAME == osx ; then brew install qt5; fi
  if test $TRAVIS_OS_NAME == osx ; then brew link qt5 --force; fi
  if test $TRAVIS_OS_NAME == osx ; then brew linkapps qt5; fi
fi


