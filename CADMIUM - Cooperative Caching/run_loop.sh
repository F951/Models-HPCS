#!/bin/bash

TOTALQUERIES=500
ARRIVAL_RATE=5000
PEERS=50


cd bin
#rm activity*
#rm plot*
#rm salida_back*
#rm Salida_stats.csv
#rm time.txt
cd ..
#for i in {100,300,500,700} #PEERS
for i in 100 #PEERS
do		
	cd input_data
	cd ..
	#for j in {400,800,1200,1600} #TOTALQUERIES
	for j in 400 #TOTALQUERIES
	do	
		PEERS=$i
		TOTALQUERIES=$j
		echo PEERS: $PEERS
		echo TOTALQUERIES: $TOTALQUERIES
		cd input_data
		./compilar.sh
		./exponential_distribution.o $ARRIVAL_RATE $TOTALQUERIES $PEERS
		cd ../bin
		echo ../input_data/Log\_0\_$TOTALQUERIES\_$PEERS.txt
		{ time ./ABP ../input_data/Log\_0\_$TOTALQUERIES\_$PEERS.txt $PEERS $TOTALQUERIES  1 2 > salida_background-$PEERS-$TOTALQUERIES.txt ; } 2>> time.txt &  
		psrecord $(pgrep -f 'ABP\s../input_data/Log_0_'$TOTALQUERIES'_'$PEERS'.txt\s'$PEERS'\s'$TOTALQUERIES) --log activity-$PEERS-$TOTALQUERIES.txt  --interval 2 --plot plot-$PEERS-$TOTALQUERIES.png 
		cd ..
	done
done	
