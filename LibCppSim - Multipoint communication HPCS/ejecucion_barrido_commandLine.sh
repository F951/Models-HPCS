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
#RS=2 #REPLICATION STRATEGY  0:Leafset  1:Bubble  2:TLC
RS=0 #REPLICATION STRATEGY  0:Leafset  1:Bubble  2:TLC
PS=1 #PEER SELECTION  0:Uniform  1: Zipf
CR=0 #CASE RESPONSABLE 0:
#==============================================================================================
rm activity*
rm salida_background*
rm salida_stats*
rm plot*
rm time.txt

for D1 in 1 #Delay Server
do		
for D2 in 3 #Delay Server
do	
for D3 in 5 #Delay Server
do	
#for i in {100,200,300} #PEERS
for i in {500,700,900,1100} #PEERS
do		
	#for j in {200,400,600,800} #TOTALQUERIES
	for j in {2000,3000,4000,5000} #TOTALQUERIES
	do	
		for k in {1,3} #SERVERS
		#for k in 3 #SERVERS
		do	
			PEERS=$i
			QUERIES=$j
			SERVERS=$k
			echo PEERS: $PEERS
			echo QUERIES: $QUERIES
			#cd input_data
			#./compilar.sh
			#./exponential_distribution.o $ARRIVAL_RATE $TOTALQUERIES
			#cd ../bin
			#time ./ABP ../input_data/Log_0.txt $PEERS $TOTALQUERIES
			#time ./ABP ../input_data/Log_0.txt $PEERS $TOTALQUERIES >> salida_background-$PEERS-$TOTALQUERIES.txt &
			#{ time ./ABP ../input_data/Log_0.txt $PEERS $TOTALQUERIES  1 2 >> salida_background-$PEERS-$TOTALQUERIES.txt ; } 2>> time.txt &  
			#{ time ./Simulador $QUERIES $CACHE $LOCALCACHE $LCACHE $PEERS 16 Datasets/RISE_16M.DAT $USERS $THAO $RS $PS $CR 10  1 2 >> salida_stats.txt ; } 2>> time.txt &  
			{ time ./Simulador $QUERIES $CACHE $LOCALCACHE $LCACHE $PEERS 16 Datasets/RISE_16M.DAT $USERS $THAO $RS $PS $CR 10 $SERVERS $D1 $D2 $D3 1 2 >> salida_background-$PEERS-$QUERIES-$SERVERS.txt ; } 2>> time.txt &  
			psrecord $(pgrep Simulador) --log activity-$PEERS-$QUERIES-$SERVERS.txt  --interval 0.001 --plot plot-$PEERS-$QUERIES-$SERVERS.png
		done
	done
done
done
done
done