#!/bin/bash

LIB_DIR=/home/wolf/Dropbox/Projects/LightSpeed/src/libperf
TRIALS="{1..10}"

# perf wrapped
for i in $TRIALS; do sudo LD_LIBRARY_PATH=$LIB_DIR ./bench 0 >> cpu.libperf.log; done
for i in $TRIALS; do sudo LD_LIBRARY_PATH=$LIB_DIR ./bench 3 >> mem.libperf.log; done
for i in $TRIALS; do sudo LD_LIBRARY_PATH=$LIB_DIR ./bench 1  >> threads.libperf.log; done
for i in $TRIALS; do sudo LD_LIBRARY_PATH=$LIB_DIR ./bench 5  >> IO.libperf.log; done
for i in $TRIALS; do sudo LD_LIBRARY_PATH=$LIB_DIR ./bench 2  >> mutex.libperf.log; done

# bare
for i in $TRIALS; do sudo /usr/bin/sysbench --test=cpu --cpu-max-prime=20000 run >> cpu.bare.log; done
for i in $TRIALS; do sudo /usr/bin/sysbench --test=memory --memory-block-size=4K --memory-total-size=23G run  >> mem.bare.log; done
for i in $TRIALS; do sudo /usr/bin/sysbench --num-threads=64 --test=threads --thread-yields=7500 --thread-locks=8 run  >> threads.bare.log; done
for i in $TRIALS; do sudo /usr/bin/sysbench --num-threads=2 --max-requests=7000 --test=fileio --file-total-size=3G --file-test-mode=rndrw run  >> IO.bare.log; done
for i in $TRIALS; do sudo /usr/bin/sysbench --test=mutex --mutex-num=4096 --mutex-locks=300000000 --mutex-loops=10000000 run  >> mutex.bare.log; done
