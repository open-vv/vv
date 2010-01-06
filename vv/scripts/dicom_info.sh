#!/bin/bash
#Get info about the first dicom image in the directory
find . -iname "*.dcm" -type f | head -n1 | xargs clitkDicomInfo | less
