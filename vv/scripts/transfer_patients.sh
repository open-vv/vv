#!/bin/bash
cd current
echo -n "Patients before: "
find . -type l | wc -l
for i in /home/gauthier/Base_de_donnees_stereo_poumon/*; do [ -e "$i" ] || ln -s "$i" .; done
echo -n "Patients after: "
find . -type l | wc -l
for i in $(find . -type l); do date; echo $i; irsync -rv "$i" "i:/rhone-alpes/home/clb/patients/$i" >> "$i"_$(date "+%H:%m-%d_%b%Y").log; done
