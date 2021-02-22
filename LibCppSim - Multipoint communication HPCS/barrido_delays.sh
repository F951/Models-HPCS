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
#for i in {100,200,300} #PEERS
for i in 10 #PEERS
do		
#for j in {200,400,600,800} #TOTALQUERIES
for j in 10 #TOTALQUERIES
do	
for k in 4 #SERVERS
do	
	#for D1 in {1,5,7} #Delay Server
	for D1 in 1 #Delay Server
	do		
		#for D2 in {1,5,7} #Delay net
		for D2 in 3 #Delay Server
		do	
			#for D3 in {1,5,7} #Delay peers
			for D3 in 5 #Delay Server
			do	
			PEERS=$i
			QUERIES=$j
			SERVERS=$k
			echo PEERS: $PEERS
			echo QUERIES: $QUERIES
			{ time ./Simulador $QUERIES $CACHE $LOCALCACHE $LCACHE $PEERS 16 Datasets/RISE_20000.DAT $USERS $THAO $RS $PS $CR 10 $SERVERS $D1 $D2 $D3 1 2 >> salida_background-$PEERS-$QUERIES-$SERVERS-$D1-$D2-$D3.txt ; } 2>> time.txt &  
			psrecord $(pgrep Simulador) --log activity-$PEERS-$QUERIES-$SERVERS-$D1-$D2-$D3.txt  --interval 0.5 --plot plot-$PEERS-$QUERIES-$SERVERS-$D1-$D2-$D3.png
		done
	done
done
done
done
done
