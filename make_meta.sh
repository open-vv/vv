#!/bin/bash
#This script creates the files necessary for developement: vim tags files, doxygen documentation, ...
ROOT_DIR=$(pwd)
echo $ROOT_DIR
#ROOT_DIR=${HOME}/workspace/vv
cd ${ROOT_DIR}
[ "$1" = "--full" ] && doxygen clitk.doxygen&

for i in $(find $(pwd) -type d | grep -v "^\.$")
do
    cd $i && [ -n "$(find . -maxdepth 1 -name '*.c??')" ] &&
    {
        rm .vimrc
        ln -s ${ROOT_DIR}/.vimrc .
        ctags -R --languages=c,c++ --exclude=build --langmap=c++:.c++.cc.cp.cpp.cxx.h.h++.hh.hp.hpp.hxx.C.H.txx --exclude=doc --exclude=tests_jef ${ROOT_DIR} &
    }
done
wait
echo "done!"
