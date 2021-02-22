CC=g++
CFLAGS=-std=c++17  

INCLUDECADMIUM=-I ../../cadmium/include
INCLUDEDESTIMES=-I ../../DESTimes/include 
INCLUDEBOOST=-lboost_system -lboost_thread
LDADD = ../../../../Doctorado/Simulador/openssl-1.0.2l/libssl.a ../../../../Doctorado/Simulador/openssl-1.0.2l/libcrypto.a

#LIBCRYPTO = -I ../../../../Doctorado/Simulador/openssl-1.0.2l/

#CREATE BIN AND BUILD FOLDERS TO SAVE THE COMPILED FILES DURING RUNTIME
bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

#TARGET TO COMPILE ALL THE TESTS TOGETHER (NOT SIMULATOR)
message.o: data_structures/message.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) data_structures/message.cpp -o build/message.o
ServerMessage.o: data_structures/ServerMessage.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) data_structures/ServerMessage.cpp -o build/ServerMessage.o
#TLCmessage.o: data_structures/TLCmessage.cpp
#	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) data_structures/TLCmessage.cpp -o build/TLCmessage.o
#PastryMessage.o: data_structures/PastryMessage.cpp
#	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) data_structures/PastryMessage.cpp -o build/PastryMessage.o

build/hash.o: auxiliar/Hash.cc auxiliar/Hash.h	
	g++ -g -O3 -c $(CFLAGS) auxiliar/Hash.cc  -o build/hash.o -Wno-deprecated -Wall 

build/Util.o: auxiliar/Util.cc auxiliar/Util.h	
	g++ -g -O3 -c $(CFLAGS) auxiliar/Util.cc -o build/Util.o -Wno-deprecated -Wall 

#build/Leafset.o: atomics/p2pLayer/Leafset.cc atomics/p2pLayer/Leafset.h	auxiliar/Util.h
#	g++ -g -O3 -c $(CFLAGS) atomics/p2pLayer/Leafset.cc -o build/Leafset.o  -Wno-deprecated -Wall 
#build/routingtable.o: atomics/p2pLayer/RoutingTable.cc atomics/p2pLayer/RoutingTable.h auxiliar/Util.h
#	g++ -g -O3 -c $(CFLAGS) -o build/routingtable.o atomics/p2pLayer/RoutingTable.cc -Wno-deprecated -Wall 
#build/statebuilder.o: atomics/p2pLayer/StateBuilder.cc atomics/p2pLayer/StateBuilder.h
#	g++ -g -O3 -c $(CFLAGS) -o build/statebuilder.o atomics/p2pLayer/StateBuilder.cc -Wno-deprecated -Wall 

#build/Lru.o: atomics/applicationLayer/Lru.cc atomics/applicationLayer/Lru.h
#	g++ -g -O3 -c $(CFLAGS) -o build/Lru.o atomics/applicationLayer/Lru.cc -Wno-deprecated -Wall 	
#build/InLink.o: atomics/applicationLayer/InLink.cc atomics/applicationLayer/InLink.h
#	g++ -g -O3 -c $(CFLAGS) -o build/InLink.o atomics/applicationLayer/InLink.cc -Wno-deprecated -Wall 

main_top.o: top_model/main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES)top_model/main.cpp -o build/main_top.o 
	
#main_subnet_test.o: test/main_subnet_test.cpp
#	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_subnet_test.cpp -o build/main_subnet_test.o

#main_sender_test.o: test/main_sender_test.cpp
#	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_sender_test.cpp -o build/main_sender_test.o

#main_receiver_test.o: test/main_receiver_test.cpp
#	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test/main_receiver_test.cpp -o build/main_receiver_test.o

#tests: main_subnet_test.o main_sender_test.o  message.o main_receiver_test.o
		#$(CC) -g -o bin/SUBNET_TEST build/main_subnet_test.o build/message.o
		#$(CC) -g -o bin/SENDER_TEST build/main_sender_test.o build/message.o 
		#$(CC) -g -o bin/RECEIVER_TEST build/main_receiver_test.o build/message.o  

#TARGET TO COMPILE ONLY ABP SIMULATOR
simulator: main_top.o message.o ServerMessage.o build/hash.o build/Util.o
	#$(CC) -g -o bin/ABP build/main_top.o build/message.o build/ServerMessage.o build/TLCmessage.o build/PastryMessage.o build/hash.o build/Util.o  build/Leafset.o build/routingtable.o build/statebuilder.o build/InLink.o build/Lru.o -ldl -lssl -lz -static $(LDADD) -lcrypto  -L /usr/lib/x86_64-linux-gnu/ 
	#$(CC) -DCADMIUM_EXECUTE_CONCURRENT -g   -o bin/ABP build/main_top.o build/message.o build/ServerMessage.o build/TLCmessage.o build/PastryMessage.o build/hash.o build/Util.o  build/Leafset.o build/routingtable.o build/statebuilder.o build/InLink.o build/Lru.o -ldl -lssl -lz  $(INCLUDEBOOST) -static $(LDADD) -lcrypto  -L /usr/lib/x86_64-linux-gnu/ 
	$(CC) -DCADMIUM_EXECUTE_CONCURRENT -g   -o bin/ABP build/main_top.o build/message.o build/ServerMessage.o build/hash.o build/Util.o -ldl -lssl -lz  $(INCLUDEBOOST) -static $(LDADD) -lcrypto  -L /usr/lib/x86_64-linux-gnu/ 
	 
#TARGET TO COMPILE EVERYTHING (ABP SIMULATOR + TESTS TOGETHER)
all: simulator #tests

#CLEAN COMMANDS
clean: 
	rm -f bin/* build/*