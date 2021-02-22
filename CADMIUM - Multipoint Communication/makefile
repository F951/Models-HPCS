CC=g++
#CFLAGS=-std=c++17  
CFLAGS=-std=c++17  -DCADMIUM_EXECUTE_CONCURRENT

INCLUDECADMIUM=-I ../../cadmium/include
INCLUDEDESTIMES=-I ../../DESTimes/include 
INCLUDEBOOST=-lboost_system -lboost_thread
LDADD = ../../../../Doctorado/Simulador/openssl-1.0.2l/libssl.a ../../../../Doctorado/Simulador/openssl-1.0.2l/libcrypto.a

#CREATE BIN AND BUILD FOLDERS TO SAVE THE COMPILED FILES DURING RUNTIME
bin_folder := $(shell mkdir -p bin)
build_folder := $(shell mkdir -p build)
results_folder := $(shell mkdir -p simulation_results)

#TARGET TO COMPILE ALL THE TESTS TOGETHER (NOT SIMULATOR)
message.o: data_structures/message.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) data_structures/message.cpp -o build/message.o  $(INCLUDEBOOST) 
ServerMessage.o: data_structures/ServerMessage.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) data_structures/ServerMessage.cpp -o build/ServerMessage.o  $(INCLUDEBOOST) 

build/hash.o: auxiliar/Hash.cc auxiliar/Hash.h	
	g++ -g -O3 -c $(CFLAGS) auxiliar/Hash.cc  -o build/hash.o -Wno-deprecated -Wall $(INCLUDEBOOST) 

build/Util.o: auxiliar/Util.cc auxiliar/Util.h	
	g++ -g -O3 -c $(CFLAGS) auxiliar/Util.cc -o build/Util.o -Wno-deprecated -Wall $(INCLUDEBOOST) 

main_top.o: top_model/main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES)top_model/main.cpp -o build/main_top.o $(INCLUDEBOOST) 
	
#TARGET TO COMPILE ONLY ABP SIMULATOR
simulator: main_top.o message.o ServerMessage.o build/hash.o build/Util.o
	#$(CC) -g  -DCADMIUM_EXECUTE_CONCURRENT  -o bin/ABP build/main_top.o build/message.o build/ServerMessage.o build/hash.o build/Util.o -ldl -lssl -lz  $(INCLUDEBOOST)  -lcrypto  -L /usr/lib/x86_64-linux-gnu/ 
	$(CC) -g  -DCADMIUM_EXECUTE_CONCURRENT  -o bin/ABP build/main_top.o build/ServerMessage.o build/hash.o build/Util.o -ldl -lssl -lz  $(INCLUDEBOOST)  -lcrypto  -L /usr/lib/x86_64-linux-gnu/ 
	 
#TARGET TO COMPILE EVERYTHING
all: simulator

#CLEAN COMMANDS
clean: 
	rm -f bin/* build/*