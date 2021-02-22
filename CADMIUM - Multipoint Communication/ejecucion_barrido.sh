#!/bin/bash

TOTALQUERIES=500
ARRIVAL_RATE=5000
PEERS=50

#for m in {1..2} #RUN
for m in 1 #RUN
do	
	for i in 100 #PEERS
	#for i in {100,200,300} #PEERS
	do		
		#for j in {200,400,600} #TOTALQUERIES
		for j in 300 #TOTALQUERIES
		do	
			for k in {1,3} #SERVERS
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
				time ./ABP ../input_data/Log_0_$TOTALQUERIES.txt $PEERS $TOTALQUERIES $SERVERS 1 1 1 > salida_background-$PEERS-$TOTALQUERIES-$SERVERS-$RUN.txt &
				#./ABP ../input_data/Log_0.txt $PEERS $TOTALQUERIES > salida_background-$PEERS-$TOTALQUERIES.txt &
				psrecord $(pgrep ABP) --log activity-$PEERS-$TOTALQUERIES-$SERVERS-$RUN.txt  --interval 0.2 --plot plot-$PEERS-$TOTALQUERIES-$SERVERS-$RUN.png
				cd ..
			done
		done
	done
done