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
    protocol=${!i}
    if [ ${!i} = "all" ];
    then
      echo "Protocol: MI" >> $filename
    else
      echo "Protocol: ${!i}" >> $filename
    fi
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
if [ $protocol = "all" ];
then
  sed -i 's/MI/MSI/' $filename
  ../pin -t obj-intel64/pinatrace.so -- ./$program_to_run
  sed -i 's/MSI/MESI/' $filename
  ../pin -t obj-intel64/pinatrace.so -- ./$program_to_run
  python3 cacheSim_plotter_all.py
else
  #Plot graphs
  python3 cacheSim_graph_plotter.py output_$protocol.csv output_traffic_$protocol.csv
fi