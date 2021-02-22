#!/bin/bash

#INCLUDEDESTIMES=-I ../../DESTimes/include 


#g++ -o chi_squared_distribution.o chi_squared_distribution.cpp
g++ -I ../../DESTimes/include  -o exponential_distribution.o exponential_distribution.cpp
g++ -I ../../DESTimes/include  -o TimeAssign.o TimeAssign.cpp
#g++ -o weibull_distribution.o weibull_distribution.cpp
#g++ -o weibull_distribution_traza_sim.o weibull_distribution_traza_sim.cpp
#g++ -o uniform_real_distribution.o uniform_real_distribution.cpp