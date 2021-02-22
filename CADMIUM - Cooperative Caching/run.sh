#!/bin/bash


TOTALQUERIES=30
ARRIVAL_RATE=5000
PEERS=50
cd input_data
./compilar.sh
./exponential_distribution.o $ARRIVAL_RATE $TOTALQUERIES $PEERS
cd ../bin
#time ./ABP ../input_data/Log_0_$TOTALQUERIES.txt $PEERS $TOTALQUERIES
time ./ABP ../input_data/Log\_0\_$TOTALQUERIES\_$PEERS.txt $PEERS $TOTALQUERIES
