#! /bin/bash 

compress_file()
{
  local f=$1
  if [ "$verbose" = "1" ]; then
    echo "Compressing $f..."
  fi

  local msg=`clitkImageConvert -i $f -o $f -c`
  if [ -z "$msg" ]; then
    raw=`echo $f | sed 's/\.mhd/\.raw/'`
    if test -e $raw; then
      rm $raw
    fi
  fi
}

compress_directory()
{
  local input=$1

  files=$(find $input -name "*.mhd" | sort)
  for f in $files; do
    if ! grep -iq 'LIST' $f; then
      # process all except 4D files
      compress_file $f
    else
      # process only 4D files (just update pointers to new zraw files)
      # assumes each .RAW in the list has a corresponding .MHD, which
      # must be compressed separately
      sed -i 's/\.raw/\.zraw/g' $f
      if grep -q "CompressedData" $f; then
        sed -i 's/CompressedData.*/CompressedData = True/' $f
      else
        tmp=/tmp/$RANDOM.mhd
        echo "CompressedData = True" > $tmp
        cat $f >> $tmp
        mv $tmp $f
      fi 
    fi
  done
}

# main program

if [ $# -lt 1 -o $# -gt 2 ]; then
  echo "Invalid params. `basename $0` [-h | --help] for help" 1>&2
  exit -1
fi

if [ -n "`echo $@ | grep '\-h\|--help'`" ]; then
  echo "Usage: `basename $0` {FILE | DIRECTORY | -h | --help | -v}"
  exit 0
fi

if [ -n "`echo $@ | grep '\-v'`" ]; then
  echo Verbose mode : ON
  verbose=1
fi  

input=$1
if test -d $input; then
  compress_directory $input
elif test -f $input; then
  compress_file $input
else 
  echo "Unknow input file type." 1>&2
fi
