#!/bin/bash

TOTALQUERIES=500
ARRIVAL_RATE=5000
PEERS=50


for i in {1..50} #PEERS
#for i in 1 #PEERS
do		
	##for j in {1000,3000,5000,7000,10000} #TOTALQUERIES
	#for j in {100,300,500,700} #TOTALQUERIES
	##for j in 700 #TOTALQUERIES
	#do	
	#	PEERS=$i
	#	TOTALQUERIES=$j
	#	cd input_data
	#	./compilar.sh
	#	./exponential_distribution.o $ARRIVAL_RATE $TOTALQUERIES
	#	cd ../bin
	#	#./ABP ../input_data/Log_0.txt $PEERS $TOTALQUERIES >> salida_exp1.csv
	#	time ./ABP\ ../input_data/Log_0.txt\ $PEERS\ $TOTALQUERIES > salida_background-$PEERS-$TOTALQUERIES.txt &
	#	#./ABP ../input_data/Log_0.txt $PEERS $TOTALQUERIES > salida_background-$PEERS-$TOTALQUERIES.txt &
	#	psrecord $(pgrep ABP) --log activity-$PEERS-$TOTALQUERIES.txt  --interval 2 --plot plot-$PEERS-$TOTALQUERIES.png
	#	cd ..
	#done
	#{ time ./ABP ../input_data/Log_gen2_$TOTALQUERIES.txt $PEERS $TOTALQUERIES  1 2 > salida_background-$PEERS-$TOTALQUERIES.txt ; } 2>> time.txt &  
	make clean 
	( time make ) 2>> M_time.txt 
done