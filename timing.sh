#!/bin/bash

P_file=results_pietrzak.txt
W_file=results_wesolowski.txt
P_error=error_pietrzak.txt
W_error=error_wesolowski.txt

date > ${P_file}
date > ${W_file}
date > ${P_error}
date > ${W_error}

for rep in 1 2 3
do
  echo "rep = ${rep}"
  for t in 1 2 4 8 16 #24 32
  do
    echo "t = ${t}"
    for lambda in 16 32 64 128
    do
      echo "lambda = ${lambda}"
      for lambdaRSW in 256 512 1024 2048
      do
        echo "lambdaRSW = ${lambdaRSW}"
        ./timing_wesolowski.out ${lambda} ${t} ${lambdaRSW} 1>> ${W_file} 2>> ${W_error}
        for d in 0 1 2 3 thalf t
        do
          echo "d = ${d}"
          for key_size in 16 24 32
          do
            echo "key_size = ${key_size}"
            ./timing_pietrzak.out ${lambda} ${t} ${lambdaRSW} ${d} ${key_size} 1>> ${P_file} 2>> ${P_error}
          done
        done
      done
    done
  done
done

