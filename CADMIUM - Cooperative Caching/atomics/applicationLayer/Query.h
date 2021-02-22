#ifndef _Query_h
#define _Query_h

#include "../glob.h"
#include "../p2pLayer/NodeEntry.h"
class Query
{
public:

	//char* term;
	string term; 
	BIGNUM* hashValue;
	int id;
	int id_tarea;
	int rte;
	double t_start;
	int hops;
	NodeEntry* src;
	long int timeOut;
	bool fc;
	
	Query(int _id, int _id_tarea, string _term, BIGNUM* _hashValue,  double _t_start)
	//Query(int _id, string _term, BIGNUM* _hashValue,  double _t_start, double _TTL_tarea, list<int> _opciones, int _prioridad_tarea, 	Rtas_Peers* _rta)
   {
		term=_term;
		id=_id;
		id_tarea = _id_tarea;
		t_start = _t_start;
		hops=0;
		hashValue=_hashValue;
		timeOut = _t_start + TIMEOUT_Q;
		fc=false;
   }
   
	void setQuerySrc(NodeEntry* _ne){
		src=_ne;
	}
   
   long int getTimeOut(){
		return timeOut;
	}

	void renewTimeOut(long int thistime){
		timeOut = thistime + TIMEOUT_Q;
	}

	void forceRouting(){
		fc=true;
	}
	bool getForceRouting(){
		return fc;
	}
	
	void printQuery(){
		cout << "Print Query -------------------------------------------------" << endl;
		cout << "id_tarea: "<< id_tarea << endl;
		cout << "term: "<< term << endl;
		cout << "id: "<< id << endl;
		cout << "t_start: "<< t_start << endl;
		cout << "hops: "<< hops << endl;
		cout << "hashValue: "<< BN_bn2dec(hashValue) << endl;
		cout << "timeOut: "<< timeOut << endl;
		cout << "fc: "<< fc << endl;
		cout << "----------------------------------------------------------------" << endl;
	}
	void printQuery2(){
		cout << "!!!Print Query -------------------------------------------------" << endl;
		cout << "!!!id_tarea: "<< id_tarea << endl;
		cout << "term: "<< term << endl;
		cout << "id: "<< id << endl;
		cout << "t_start: "<< t_start << endl;
		cout << "hops: "<< hops << endl;
		cout << "hashValue: "<< BN_bn2dec(hashValue) << endl;
		cout << "timeOut: "<< timeOut << endl;
		cout << "fc: "<< fc << endl;
		cout << "----------------------------------------------------------------" << endl;
	}
	void printQuery3(){
		cout << "###Print Query -------------------------------------------------" << endl;
		cout << "###id_tarea: "<< id_tarea << endl;
		cout << "term: "<< term << endl;
		cout << "id: "<< id << endl;
		cout << "t_start: "<< t_start << endl;
		cout << "hops: "<< hops << endl;
		cout << "hashValue: "<< BN_bn2dec(hashValue) << endl;
		cout << "timeOut: "<< timeOut << endl;
		cout << "fc: "<< fc << endl;
		cout << "----------------------------------------------------------------" << endl;
	}
	
   
}; 
#endif
