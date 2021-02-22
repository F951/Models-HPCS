#!/bin/bash

TOTALQUERIES=500
ARRIVAL_RATE=5000
PEERS=50

#==============================================================================================
#PEERS=500
#QUERIES=3000
CACHE=100
LOCALCACHE=50
LCACHE=50
USERS=0
THAO=0.05
RS=0 #REPLICATION STRATEGY  0:Leafset  1:Bubble  2:TLC
PS=1 #PEER SELECTION  0:Uniform  1: Zipf
CR=0 #CASE RESPONSABLE 0:
#==============================================================================================
rm activity*
rm salida_background*
rm salida_stats*
rm plot*
rm time.txt
for i in {100,300,500,700} #PEERS
#for i in {150,350,550,750} #PEERS
#for i in 100 #PEERS
do		
	for j in {400,800,1200,1600} #TOTALQUERIES
	#for j in {410,810,1210,1610} #TOTALQUERIES
	do	
		PEERS=$i
		QUERIES=$j
		echo PEERS: $PEERS
		echo QUERIES: $QUERIES
		{ time ./Simulador $QUERIES $CACHE $LOCALCACHE $LCACHE $PEERS 16 Datasets/RISE_20000.DAT $USERS $THAO $RS $PS $CR 10  1 2 >> salida_background-$PEERS-$QUERIES.txt ; } 2>> time.txt &  
		psrecord $(pgrep Simulador) --log activity-$PEERS-$QUERIES.txt  --interval 0.1 --plot plot-$PEERS-$QUERIES.png
		#cd ..
	done
done
