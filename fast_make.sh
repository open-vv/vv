#!/bin/bash
vv_dir=$(dirname $(readlink -e $(which $0)))
echo clitk3 directory: $vv_dir
cd ${vv_dir}/build


function handle_exit
{
    rm mem_use 2>>/dev/null
    killall -s SIGCONT make
    killall make
    killall cc1plus
    echo "Terminated, exiting..."
    echo
    echo
    exit
}

trap handle_exit SIGINT
available_mem=$(cat /proc/meminfo | grep MemTotal | grep -o [0-9]*)
if [ -a "memory_exhausted_lock" ]
then
   echo "Running in memory conservation mode..."
   max_cpp_process_mem_use=1600000
   cpus=$(( $available_mem / $max_cpp_process_mem_use ))
   echo "Using $cpus cpu(s) should be safe..."
   sleep 1
   make -j${cpus}
else #use all the available computing power by default
    cpus=$(( $(cat /proc/cpuinfo | grep -c ^processor) + 0 ))
    echo "Building with ${cpus} cpus..."
fi

nice -n12 ionice -c3 make -j ${cpus} $@ &
make_pid=$(jobs -p %nice)

#watch memory use to avoid crashes
while ps $make_pid >>/dev/null 
do
    if [ x"$(ps aux | grep cc1plus | grep -v grep | wc -l)" != x0 ]
    then
        ps ax -o vsize,comm | grep cc1plus | grep -o "\<[0-9]*\>" > mem_use
        used_mem=$(awk 'BEGIN {sum=0;} {sum+=$1;} END {print sum;}' mem_use)
        if (( "$used_mem"> ($available_mem - 300) ))
        then
            touch memory_exhausted_lock
            echo "Stopping due to exagerated memory use ( $used_mem )"
            handle_exit
        elif (( "$used_mem"> ($available_mem/2) ))
        then
            if [ x$high_mem != xtrue ]
            then
                echo "Warning, high memory use, not spawning any more compilation jobs... ( $used_mem )"
                killall -s SIGSTOP make
                killall -s SIGCONT cc1plus
                high_mem="true"
                date_mem=$(date +%s)
            fi
            echo mem $used_mem / $available_mem
        elif [ x$high_mem = xtrue ] && (( $(date +%s) > ( $date_mem + 5 ) ))
        then
            echo "Memory use back to normal"
            high_mem=""
            killall -s SIGCONT make
        fi
        rm mem_use
    fi
    sleep 1
done
rm memory_exhausted_lock 2>>/dev/null
wait
echo Done!
echo
