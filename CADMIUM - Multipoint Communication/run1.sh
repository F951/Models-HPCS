#!/bin/bash

TOTALQUERIES=500
ARRIVAL_RATE=5000
PEERS=50

#for m in {1..2} #RUN
for m in 1 #RUN
do	
for i in 10 #PEERS
#for i in {100,200,300} #PEERS
do		
#for j in {200,400,600} #TOTALQUERIES
for j in 10 #TOTALQUERIES
do	
	for k in 4 #SERVERS
	do	
		for D1 in 1 #Delay Server
		do		
			for D2 in 3 #Delay isp
			do	
				for D3 in 5 #Delay peers
				do	
				PEERS=$i
				TOTALQUERIES=$j
				SERVERS=$k
				RUN=$m
				cd input_data
				./compilar.sh
				./exponential_distribution.o $ARRIVAL_RATE $TOTALQUERIES
				cd ../bin
				#./ABP ../input_data/Log_0.txt $PEERS $TOTALQUERIES >> salida_exp1.csv
				time ./ABP ../input_data/Log_0_$TOTALQUERIES.txt $PEERS $TOTALQUERIES $SERVERS $D1 $D2 $D3 #> salida_background-$PEERS-$TOTALQUERIES-$SERVERS-$D1-$D2-$D3.txt &
				#./ABP ../input_data/Log_0.txt $PEERS $TOTALQUERIES > salida_background-$PEERS-$TOTALQUERIES.txt &
				psrecord $(pgrep ABP) --log activity-$PEERS-$TOTALQUERIES-$SERVERS-$D1-$D2-$D3.txt  --interval 0.2 --plot plot-$PEERS-$TOTALQUERIES-$SERVERS-$D1-$D2-$D3.png
				cd ..
				done
			done
		done
	done
done
done
done