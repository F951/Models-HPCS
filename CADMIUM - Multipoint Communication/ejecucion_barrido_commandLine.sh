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
#for i in {100,300} #PEERS
#for i in {500,700} #PEERS
#for i in 100 #PEERS
for D1 in 1 #Delay Server
do		
for D2 in 3 #Delay isp
do	
for D3 in 5 #Delay peers
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
			TOTALQUERIES=$j
			SERVERS=$k
			echo PEERS: $PEERS
			echo TOTALQUERIES: $TOTALQUERIES
			cd input_data
			./compilar.sh
			./exponential_distribution.o $ARRIVAL_RATE $TOTALQUERIES $SERVERS
			cd ../bin
			#time ./ABP ../input_data/Log_0.txt $PEERS $TOTALQUERIES
			#time ./ABP ../input_data/Log_0.txt $PEERS $TOTALQUERIES > salida_background-$PEERS-$TOTALQUERIES.txt &
			#{ time ./ABP ../input_data/Log_0.txt $PEERS $TOTALQUERIES  1 2 > salida_background-$PEERS-$TOTALQUERIES.txt ; } 2>> time.txt &  
			#psrecord $(pgrep -f 'ABP\s../input_data/Log_0.txt\s'$PEERS'\s'$TOTALQUERIES) --log activity-$PEERS-$TOTALQUERIES.txt  --interval 2 --plot plot-$PEERS-$TOTALQUERIES.png &
			{ time ./ABP ../input_data/Log_0\_$TOTALQUERIES\_$SERVERS.txt $PEERS $TOTALQUERIES $SERVERS $D1 $D2 $D3 1 2 > salida_background-$PEERS-$TOTALQUERIES-$SERVERS.txt ; } 2>> time.txt &  
			#psrecord $(pgrep -f 'ABP\s../input_data/Log_0_'$TOTALQUERIES'.txt\s'$PEERS'\s'$TOTALQUERIES) --log activity-$PEERS-$TOTALQUERIES.txt  --interval 2 --plot plot-$PEERS-$TOTALQUERIES.png 
			psrecord $(pgrep -f 'ABP\s../input_data/Log_0_'$TOTALQUERIES'_'$SERVERS'.txt\s'$PEERS'\s'$TOTALQUERIES) --log activity-$PEERS-$TOTALQUERIES-$SERVERS.txt  --interval 2 --plot plot-$PEERS-$TOTALQUERIES-$SERVERS.png 
			cd ..
		done
	done
done
done
done
done
