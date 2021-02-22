/**/

#include "glob.h"
#include "generator/gen_rnd.h"
#include "Observer.h"
#include "server/SERVER.h"
#include "client/CLIENT.h"


class mySystem: public process 
{

private:

   double simLen;
   char name[128];
  
   //Number of Nodes
   int nodes;
  
   // Cache Size
   int cacheSize, localCacheSize, lcacheSize;
  
   // Observer
   handle<Observer> obs;

	//Queries
	int totalQueries, sentQueries;

	int MAX_DEBIT, Q_PER_SEC;

	//Comandos a ejecutar (DAG)
	char traces[2048];

	//Generador de tasa de llegada
	handle<Gen_rnd> generator;

	int leafset;

	int *ends;

	int Nuser;

	int Rep_Strategy;
	int Peer_Selection;
	int Case_Responsable;
	int cte_trabajo;
	double Thao;
	int nro_servers;
	int param_network;
	int param_peer;
	int param_server;
   
public:

   mySystem(const string& _name,
            double _sl,
            int _totalQueries, 
            int _cacheSize,
	    int _localCacheSize,
	    int _lcacheSize,
            int _nodes, 
            char *_traces,
	    int _leafset,
            int _Nuser,
	    double _Thao,
	    int _RS,
	    int _PS,
	    int _CS,
	    int _cte_trabajo,
	    int _nro_servers,
	    int _param_server,
	    int _param_network,
	    int _param_peer
           ) : process( _name )
   {
      simLen         = _sl;
      Thao           = _Thao; 
      totalQueries   = _totalQueries;
      sentQueries    = 0;
      cacheSize      = _cacheSize;
      localCacheSize = _localCacheSize;
      lcacheSize     = _lcacheSize;
      nodes          = _nodes;
      leafset	     = _leafset;
      strcpy(traces, _traces);
      ends           = new int[nodes+1]; 
      Nuser          = _Nuser;
      Rep_Strategy   = _RS;
      Peer_Selection = _PS;
      Case_Responsable = _CS;
      cte_trabajo	=_cte_trabajo;
      nro_servers = _nro_servers;
      param_server = _param_server;
      param_network = _param_network;
      param_peer = _param_peer;
      
      for ( int i=0; i< nodes+1; i++)
         ends[i]=0;
   }

protected:

   void inner_body( void );

};

  
void mySystem::inner_body( void )
{
   
	char name[128];
	obs = new Observer(nodes, "OBSERVER",totalQueries,nro_servers);
	obs->param_server=param_server;
	obs->param_network=param_network;
	obs->param_peer=param_peer;
	obs->activate();

	// NETWORK
	handle<NETWORK> *network1= new handle<NETWORK>;
	*network1 = new  NETWORK("NETWORK1",nodes,nro_servers,&obs);
	(*network1)->param_network=param_network;
		
	handle<NETWORK> *network2= new handle<NETWORK>;
	*network2 = new  NETWORK("NETWORK2",nodes,nro_servers,&obs);
	(*network2)->param_network=param_network;

	// SERVER
	handle<SERVER> *server;
	server = new handle<SERVER>[nro_servers];
	for ( int i=0; i < nro_servers; i++ ){
		sprintf(name,"SERVER_%d", i);
		server[i] = new SERVER(name,nodes,&obs,i,totalQueries);
		(server[i])->param_server=param_server;
		(server[i])->set_handle_network(network2);	
		(server[i])->activate();
		(*network1)->add_server(&server[i]);
	}

	// Query generator                 
	generator = new Gen_rnd("GENERATOR", traces, &totalQueries, nodes, nro_servers,
												   &obs, ends, Nuser, Peer_Selection); 
	for ( int i=0; i < nro_servers; i++ ){
		generator->add_server(&server[i]);
	}		   

	//Peers
	handle<CLIENT> *c;
	handle<CLIENT> *clientes_server_Peers; 
	clientes_server_Peers = new handle<CLIENT>[nodes];
	for ( int i=0; i < nodes; i++ )	{
		sprintf(name,"ClientWSE_%d", i);
		c = new handle<CLIENT>;
		(*c) = new CLIENT(name, i, network1,nodes,totalQueries); 
		(*c)->set_ClientWSE_obj(c);
		(*c)->param_peer=param_peer;
		clientes_server_Peers[i]=(*c);
		(*(clientes_server_Peers[i]->I_obj))->id_peer_client=i;
		(*network2)->add_peer(&clientes_server_Peers[i]);
	}


	for ( int i=0; i < nro_servers; i++ ){
		for ( int j=0; j < nodes; j++ )	{
			(server[i])->add_peer(&clientes_server_Peers[j]);
		}
	}
	generator ->activate();


	hold( simLen ); // Stops when the simulation time is achieve

	generator->freeGen_rnd();

	end_simulation( );
}


int main( int argc, char* argv[] )
{ 

   char traces[2048];
    
   //Queries
   int totalQueries  = atoi(argv[1]);  //Total queries to process
   cout << "Total Queries " << totalQueries << endl;      
  
   //Rcache
   int cacheSize     = atoi(argv[2]);       //Size of Rcache 
   cout << "Cache Size " << cacheSize << endl;      
  
   //Local cache
   int localCacheSize     = atoi(argv[3]);       //Size in Local Rcache 
   cout << "Local Cache Size " << localCacheSize << endl;      

   //Lcache
   int lcacheSize     = atoi(argv[4]);       //Size of LCache 
   cout << "LCache Size " << lcacheSize << endl;      
  
   int nodes  = atoi(argv[5]);              // Number of nodes in the system
   cout << "Number Nodes " << nodes << endl;      
  
   int leafset = atoi(argv[6]);		// Leafset size
   cout << "Leafset Size " << leafset << endl;

   //DAG
   strcpy( traces , argv[7]);
   cout << "Traces " << traces << endl;      

   //number of user
   int Nusers = atoi(argv[8]);

   double Thao = atof(argv[9]);

   cout << "Nuser " << Nusers << endl;
   cout << "Thao " << Thao << endl;
       
   int Rep_Strategy = atoi(argv[10]);
   cout << "Rep_Strategy " << Rep_Strategy << endl;
   int Peer_Selection = atoi(argv[11]);
   cout << "Peer_Selection " << Peer_Selection << endl;
   int Case_Responsable = atoi(argv[12]);
   cout << "Case_Responsable " << Case_Responsable << endl; 
   int cte_trabajo = atoi(argv[13]);
   cout << "cte_trabajo: " << cte_trabajo << endl; 
   int nro_servers = atoi(argv[14]);
   cout << "nro_servers: " << cte_trabajo << endl; 
   int param_server= atoi(argv[15]);
   cout << "param_server: " << cte_trabajo << endl; 
   int param_network = atoi(argv[16]);
   cout << "param_network: " << cte_trabajo << endl; 
   int param_peer = atoi(argv[17]);
   cout << "param_peer: " << cte_trabajo << endl; 
   
   cout << "cte_trabajo: " << cte_trabajo << endl; 
cout<<"Simulador - argumetos OK"<< endl;
   sched_t schedType = SQSDLL;

   simulation::instance( )->begin_simulation( makeSqs( schedType) );

   handle<mySystem> system( new mySystem("System main", 10000000e100,
                                       totalQueries,
                                       cacheSize,
				       localCacheSize,
				       lcacheSize,
                                       nodes,
                                       traces,
				       leafset,
				       Nusers,
				       Thao,
				       Rep_Strategy,
				       Peer_Selection,
				       Case_Responsable,
				       cte_trabajo,
				       nro_servers,
				       param_server,
				       param_network,
				       param_peer
				       
                                     ) );

   system->activate( );

   simulation::instance( )->run( );
    
   simulation::instance( )->end_simulation( );
  
   //cout <<endl<< "done!" << endl;
   return 0;
} 
