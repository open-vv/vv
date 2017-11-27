#Write all help of syd tools in a file sydTool.md
#Execute it in src folder but the tools have to be into ../bin/bin (or change it)
#Results are in src folder

#!/bin/bash
set -ev
currentFolder=$PWD
#Copy all help into a temporary file
tempFile=tempDoc.txt
cd ./vv_bin/bin
clitkTools=`ls clitk*`
echo "This page contains additional information on how to use clitk tools:" > $tempFile
echo "<<TableOfContents()>>" >> $tempFile
echo "" >> $tempFile
for tool in $clitkTools
do
  echo "==== " $tool " ====" >> $tempFile
  $tool -h >> $tempFile
  echo "" >> $tempFile
  echo "" >> $tempFile
  echo "" >> $tempFile
  echo "" >> $tempFile
done

#Write the .md file
docFile="$currentFolder/clitkTool.txt"
rm $docFile
helpLine=false  #Boolean to know if we are reading the command lines starting with '-h, --help' and finishing with '##' or just the help
echoLine=""
writeLine=false;
while IFS='' read -r line || [[ -n "$line" ]]; do #read all lines
    copyLine=$line;
    line=`echo "$line" | sed -e 's/^[ \t]*//'`; # prevent whitespace at the begining of the line
    if [[ $line == *"-h, --help"* ]]; then
      helpLine=true;
    fi
    if [[ $line == *"===="* ]]; then
      if $writeLine; then
        echo "$echoLine||" >> $docFile;
      fi
      echoLine="";
      echo "" >> $docFile;
      echo "" >> $docFile;
      echo "" >> $tempFile
      echo "" >> $tempFile
      writeLine=false;
      helpLine=false;
    fi

    if $helpLine; then
      if [[ $line == "" ]]; then
        continue;
      fi
      #Create the string according to the different case of ggo
      #Start with -h
      #Start with --help
      #Start with comment
      #Start with title
      if [[ $line == "-"[a-zA-Z]* ]]; then
        if $writeLine; then
          echoLine="$echoLine||";
          echo "$echoLine" >> $docFile;
        fi
        writeLine=true;
        echoLine="||";
        tempString=`echo "$line" | cut -c1-2`; # eg take the -h
        echoLine="$echoLine$tempString";
        line=`echo "$line" | cut -d' ' -f2-`; # remove -h, (first word)
        line=`echo "$line" | sed -e 's/^[ \t]*//'`; # prevent whitespace at the begining of the line
        echoLine="$echoLine||";
        tempString=`echo "$line" | cut -f 1 -d " "`; # eg take the --help (first word)
        echoLine="$echoLine$tempString";
        line=`echo "$line" | cut -d' ' -f2-`; # remove --help (first word)
        line=`echo "$line" | sed -e 's/^[ \t]*//'`; # prevent whitespace at the begining of the line
        echoLine="$echoLine||$line";
      elif [[ $line == "--"* ]]; then
        if $writeLine; then
          echoLine="$echoLine||";
          echo "$echoLine" >> $docFile;
        fi
        writeLine=true;
        echoLine="|| ||";
        tempString=`echo "$line" | cut -f 1 -d " "`; # eg take the --help (first word)
        echoLine="$echoLine$tempString";
        line=`echo "$line" | cut -d' ' -f2-`; # remove --help (first word)
        line=`echo "$line" | sed -e 's/^[ \t]*//'`; # prevent whitespace at the begining of the line
        echoLine="$echoLine||$line";
      elif [[ $copyLine == " "* ]]; then
        echoLine="$echoLine$line";
      else
        if $writeLine; then
          echoLine="$echoLine||";
          echo "$echoLine" >> $docFile;
        fi
        echoLine="||||||<style=\"&quot; &amp; quot;text-align:center&amp; quot; &quot;\">$line||";
        echo "$echoLine" >> $docFile;
        echoLine="";
        writeLine=false;
      fi
    else
      if [[ `echo "$line" | cut -c1-1` = "-" ]]; then
        echo "  " >> $docFile;
        echo "\\$line" >> $docFile;
      else
        echo "$line" >> $docFile;
      fi
    fi
done < "$tempFile"
echo "$echoLine||" >> $docFile
rm $tempFile
cd $currentFolder
