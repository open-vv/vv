#! /bin/sh


MAX_THREADS=2

check_threads()
{
    while [[ $(jobs -p | wc -l) -ge $1 ]]; do
        jobs
        sleep 10
    done
}


