#!/bin/bash

ARRIVAL_RATE=5000
CACHE=0
LOCALCACHE=0
LCACHE=0 
D1=1
D2=1
D3=1

SERVERS=1
PEERS=500
TOTALQUERIES=20


#time ./Simulador $QUERIES $CACHE $LOCALCACHE $LCACHE $PEERS 16 Datasets/RISE_16M.DAT $USERS $THAO $RS $PS $CR 10 $SERVERS
time ./Simulador $TOTALQUERIES $CACHE $LOCALCACHE $LCACHE $PEERS 16 Datasets/RISE_16M.DAT 0 0 0 0 0 10 $SERVERS $D1 $D2 $D3
#time ./ABP ../input_data/Log_0.txt $PEERS $TOTALQUERIES >> salida_background.txt &
#psrecord $(pgrep ABP) --log activity.txt  --interval 2 --plot plot.png