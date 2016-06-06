#! /bin/bash

for i in 1 $(seq 4 20) $(seq 30 36)
do
    echo
    echo "Files on host linux${i}.dg.creatis.insa-lyon.fr "
    #ssh -o "StrictHostKeyChecking no" linux${i}.dg.creatis.insa-lyon.fr "find /tmp -user $1 -exec rm -fr {} \; "
    ssh -o "StrictHostKeyChecking no" linux${i}.dg.creatis.insa-lyon.fr "find /tmp -user $1 2>/dev/null"
#ssh -o "StrictHostKeyChecking no" linux${i}.dg.creatis.insa-lyon.fr "rm -fr /tmp/tmp.*/core* /tmp/tmp.*/mac /tmp/tmp.*/data /tmp/tmp.*/output/*root"
#ssh -o "StrictHostKeyChecking no" linux${i}.dg.creatis.insa-lyon.fr "mv /tmp/tmp.* data/gate/hybrid_analog_mc/liver/hybrid_emlivermore/rr/rayleigh/run.O6VN"
done

