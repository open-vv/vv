#!/bin/bash

if [ $# -ne 1 ]
then
  echo "Usage: `basename $0` ToolName"
  exit 1
fi

find tools -name "clitkFooImage*" |
while read i
do
    cp $i ${i/FooImage/$1}
    sed -i "s/FooImage/$1/ig" ${i/FooImage/$1}
done
echo "Done!"
echo "Don't forget to add your new tool to the various CMakeLists.txt files."
