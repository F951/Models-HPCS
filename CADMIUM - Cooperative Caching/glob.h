#ifndef GLOBAL_H
#define GLOBAL_H

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
#include <openssl/bn.h>
#include <openssl/md5.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>


using namespace std;

//~ #define TIME_GEN1 0.00006
//~ #define TIME_GEN2 0.0015
//~ #define TIME_GEN3 0.000001
//~ #define TIME_PEER1 0.00115

#define CTE_SCORES  0.0025
#define CTE_AGREGACION_PEERS 0.0000025
#define CTE_AGREGACION_WSE 0.0000015
#define CTE_CONSENSO_WSE 0.00127
#define CTE_UPD_FREQ 0.0006

#define LATENCIA_BD_IMG 0.003
#define LATENCIA_BD_TASK 0.00005
#define LATENCIA_PEER_WSE 0.2
#define LATENCIA_RED_P2P 0.035

#define DELAY_BASE_ISP 0.441
#define LATENCIA_SOLICITUD_TAREA_ISP 0.0001887
#define LATENCIA_SOLICITUD_OBJETO_ISP 0.0001887
#define LATENCIA_OBJETO_OPCIONES_ISP 0.283 	//1.5MB					//0.566	para 3 MB, tasa de transferencia 5,3 MBps 
#define LATENCIA_TRABAJO_TERMINADO_ISP 0.0003773
#define LATENCIA_TRABAJO_AGREGADO_ISP 0.0003773

#define DELAY_BASE_P2P 0.228
#define LATENCIA_SOLICITUD_TAREA_P2P 0.0001153
#define LATENCIA_SOLICITUD_OBJETO_P2P 0.0001153
#define LATENCIA_OBJETO_OPCIONES_P2P 0.173	//1.5MB			//0.346 para 3 MB, tasa de tranferencia 8,67 MBps
#define LATENCIA_TRABAJO_TERMINADO_P2P 0.0002307
#define LATENCIA_TRABAJO_AGREGADO_P2P 0.0002307


#define MAX_BUFFER 2048
#define ENTRY_SIZE 1
#define LCACHE_SIZE 1
#define ASSERT(x) assert(x)
#define MAX_SEQUENTIAL 5
#define OVERLOAD_THRESHOLD 25 //Max Load (ORIGINAL)
#define DELTA_O 15000000 // Observer check
#define DELTA_T 1 //Replicator Check period

#define QUERY_RATE_STRATEGY 0
#define QUERY_DELTA_T 10000 //CADA 10000 Unidades de Tiempo
#define QUERY_DELTA_Q 10000 //CADA 10000 QUERIES

#define MIN_TTL 172800 //1 Hour
#define MAX_TTL 172800 //24 Hours

#define TTL_WSE 180// 3min
#define TIMEOUT_Q 100 // Time out de las consultas

#define WSECACHESIZE 8000
#define SIZE_ANSWER 1
#define STATIC_TIMEOUT 100 // cambiar por dinamico

#define PEER 411
#define USER 412
#define GEN_TAREA 413

//PASTRY CONF
#define BASE 16
#define BITS 160
#define B 4
#define D 10

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

