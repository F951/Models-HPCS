#!/bin/bash

ARRIVAL_RATE=5000

SERVERS=1
PEERS=500
TOTALQUERIES=20
D1=1
D2=1
D3=1

cd input_data
echo "Compile"
./compilar.sh
echo "Create Log"
./exponential_distribution.o $ARRIVAL_RATE $TOTALQUERIES $SERVERS
cd ../bin
echo "Run"
#time ./ABP ../input_data/Log_0_$TOTALQUERIES.txt $PEERS $TOTALQUERIES $SERVERS 1 1 1
time ./ABP ../input_data/Log_0\_$TOTALQUERIES\_$SERVERS.txt $PEERS $TOTALQUERIES $SERVERS $D1 $D2 $D3
#time ./ABP ../input_data/Log_0.txt $PEERS $TOTALQUERIES >> salida_background.txt &
#psrecord $(pgrep ABP) --log activity.txt  --interval 2 --plot plot.png