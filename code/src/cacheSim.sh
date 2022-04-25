#!/bin/bash
filename=cacheSim.config
if [ "$#" -ne 12 ]; then
    echo "Error in the number of arguments. The correct usage is:"
    echo "./cacheSim.sh -s 5 -E 1 -b 5 -p MSI -n 3 -o thread_hello"
    exit 1
fi
# Create config file if it doesn't exist
if [ ! -f $filename ]
then
    touch $filename
fi
# Parse through all the input arguments and store them in a Config file
for ((i=1; i<=$#; i++))
do
  if [ ${!i} = "-s" ];
  then
    ((i = i + 1))
    echo "Set: ${!i}" > $filename
  elif [ ${!i} = "-E" ];
  then
    ((i = i + 1))
    echo "Associativity: ${!i}" >> $filename
  elif [ ${!i} = "-b" ];
  then
    ((i = i + 1))
    echo "Bytes: ${!i}" >> $filename
  elif [ ${!i} = "-p" ];
  then
    ((i = i + 1))
    echo "Protocol: ${!i}" >> $filename
  elif [ ${!i} = "-n" ];
  then
    ((i = i + 1))
    echo "NumCores: ${!i}" >> $filename
  elif [ ${!i} = "-o" ];
  then
    ((i = i + 1))
    program_to_run=${!i}
  elif [ ${!i} = "-h" ];
  then
    echo "Sample example to run the script:"
    echo "./cacheSim.sh -s 5 -E 1 -b 5 -p MSI -n 3 -o thread_hello"
    exit 1
  fi
done
echo "Running cacheSim on $program_to_run"
# Running actual code
../pin -t obj-intel64/pinatrace.so -- ./$program_to_run

#Plot graphs
python3 cacheSim_graph_plotter.py