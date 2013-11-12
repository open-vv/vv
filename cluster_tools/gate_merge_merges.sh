mkdir run.merge
for i in $(ls -d results.????)
do
	ln -s ../$i run.merge/output_${i}
done
gate_power_merge.sh run.merge
 
