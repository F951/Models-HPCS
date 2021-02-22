#ifndef SERVER_MESSAGE_HPP
#define SERVER_MESSAGE_HPP

#include <assert.h>
#include <iostream>
#include <string>
#include "../glob.h"
//~ #include "../atomics/p2pLayer/NodeEntry.h"
//~ #include "../atomics/applicationLayer/Query.h"
#include "../auxiliar/Query.h"

using namespace std;

/*******************************************/
/**************** ServerMessage ****************/
/*******************************************/

	class stats_msg{
		public:
		int local_hits;
		int global_hits;
		int hops;
		int replicas;
		int misses;
		int load;
		int queries_in;
		int terminadas;
		int queries_WSE;
		double latencia;
		
		stats_msg (){
			local_hits=0;
			global_hits=0;
			hops=0;
			replicas=0;
			misses=0;
			load=0;
			queries_in=0;
			latencia=0;	
			terminadas=0;
			queries_WSE=0;
		}
		
		void reset_stats(){
			local_hits=0;
			global_hits=0;
			hops=0;
			replicas=0;
			misses=0;
			load=0;
			queries_in=0;
			latencia=0;			
			terminadas=0;
			queries_WSE=0;
		}
		
	};

class ServerMessage{
public:
	string key; 
	BIGNUM* query;
	int version;
	//~ handle<ClientWSE>* src;
	int source;  	
	int tag; 
	int dest; 
	int id_tarea;
	int id_imagen;
	double latencia;
	double size;
	Query *query_TLC;
	vector<int> integer_stats;
	vector<double> float_stats;
	stats_msg* stats_TLC;
	
	ServerMessage(){}
	
	//~ ServerMessage(string _key, BIGNUM* _query, int _source, int _dest, int _tag){	
		//~ key = _key;
		//~ query = _query;
		//~ source=_source; 
		//~ dest=_dest;
		//~ tag = _tag; 
	//~ }
	
	//~ MessageWSE(handle<ClientWSE>* _src,  BIGNUM* _query, string _key, int _srcK)
	ServerMessage(  BIGNUM* _query, string _key, int _srcK, int _tag, int _dest, int _id_tarea, double _size)
	{
		query  = BN_dup(_query);
		key.assign(_key);
		source = _srcK;   
		tag = _tag;
		dest = _dest;
		id_tarea = _id_tarea;
		stats_TLC = new stats_msg();
		size = _size;
	}

	ServerMessage clone(ServerMessage m_in){
		//~ source = m_in.source;
		//~ dest = m_in.dest;		
		//~ key = m_in.key;
		//~ query = m_in.query;	
		
		//~ stats_TLC.global_hits = m_in.stats_TLC.global_hits;
		//~ stats_TLC.local_hits = m_in.stats_TLC.local_hits;
		//~ stats_TLC.misses = m_in.stats_TLC.misses;
		//~ stats_TLC.hops = m_in.stats_TLC.hops;
		//~ stats_TLC.replicas = m_in.stats_TLC.replicas;
		//~ stats_TLC.load = m_in.stats_TLC.load;
		//~ stats_TLC.queries_in = m_in.stats_TLC.queries_in;
		//~ stats_TLC.latencia = m_in.stats_TLC.latencia;
	}
	
	stats_msg* getStats(){
		return stats_TLC; 
	}
};

istream& operator>> (istream& is, ServerMessage& msg);

ostream& operator<<(ostream& os, const ServerMessage& msg);

#endif // SERVER_MESSAGE_HPP
