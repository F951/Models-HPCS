#ifndef GLOBAL_H
#define GLOBAL_H

//#include "/homes/rscheihi/carlos/SIMULADOR/libcppsim-0.2.1/src/cppsim.hh"
#include "../../src/cppsim.hh"
#include "../../src/rng.hh"
#include "../../src/sqsPrio.hh"
#include <iostream>
#include <map>
#include <sstream>
#include <set>
#include <vector>
#include <list>
#include <assert.h>
#include <fstream>
#include <string>
#include <limits>
#include <queue>
#include <math.h>
#include <algorithm>
//#include "../../bigint/BigIntegerLibrary.hh"
#include <openssl/bn.h>
#include <openssl/md5.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>


using namespace std;


#define MAX_BUFFER 2048
#define ENTRY_SIZE 1
#define LCACHE_SIZE 1
#define ASSERT(x) assert(x)
#define MAX_SEQUENTIAL 5
//#define TIME_VALID 0 //TTL
#define OVERLOAD_THRESHOLD 25 //Max Load
//~ #define DELTA_O 15000 // Observer check
#define DELTA_O 0.0001 // Observer check
#define DELTA_T 1 //Replicator Check period

//#define REP_STRATEGY 0  // 0: Leafset   1: Bubble    2: TLC
//#define PEER_SELECTION 0 // 0: Random   1: Zipf    2:static_debug
//#define CASE_RESPONSABLE 0 // variable para Leafset y Bubble 1:replicas contactan a WSE 0: replicas borran entradas con TTL vencido


#define QUERY_RATE_STRATEGY 0
#define QUERY_DELTA_T 10000 //CADA 10000 Unidades de Tiempo
#define QUERY_DELTA_Q 10000 //CADA 10000 QUERIES
#define MIN_TTL 100 //1 Hour
#define MAX_TTL 86400 //24 Hours
#define TTL_WSE 180 // 3min
#define TIMEOUT_Q 100 // Time out de las consultas

#define WSECACHESIZE 8000
#define SIZE_ANSWER 1
#define STATIC_TIMEOUT 100 // cambiar por dinamico

#define PEER 411
#define USER 412

//PASTRY CONF
#define BASE 16
#define BITS 160
#define B 4
#define D 10

// FLASH CROWDS
#define FLASH_CROWD true  // se utiliza o no el flash crowd
#define CROWD_TIME 5.0 // segundos con flash crowd
#define NORMAL_TIME 500.0 //segundos con tasa normal
#define NORMAL_RATE 1000.0 // tasa de arribo sin flash crowd
#define CROWDED_RATE 5000.0 // tasa de arribo con flash crowd

//int cte_trabajo=10;

/****************/
//SERVICIOS
//#define USER      500
//#define END_QUERY 501
//#define IS        502
//#define MERGE     503

//--DEBUGING
#define DEBUG

#ifdef DEBUG
#define dout cout
#else
#define dout if(false) cout
#endif

#endif

