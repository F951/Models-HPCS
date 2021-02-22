/**/

#include "glob.h"
#include "generador/gen_rnd.h"
#include "Observer.h"
#include "applicationLayer/TlcProtocol.h"
#include "p2pLayer/PastryProtocol.h"
#include "p2pLayer/StateBuilder.h"
#include "transportLayer/redtubo.h"
#include "wse/WSE.h"
#include "applicationLayer/ClientTLCWSE.h"




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

// WSE
   handle<WSE> wse;

   //Peers with our solution
   handle<TlcProtocol> *Peers; 
 
  // Network to state builder
   vector<handle<PastryProtocol>*> pastryNetwork;

   //Queries
   int totalQueries, sentQueries;
 
   int MAX_DEBIT, Q_PER_SEC;

   //Comandos a ejecutar (DAG)
   char traces[2048];
  
   //Generador de tasa de llegada
   handle<Gen_rnd> generator;

   //Protocolo Red Tub
   handle<RedTubo> transport;

   // Pastry's parameters
   int leafset;

   //Endinf
   int *ends;

   int Nuser;

   int Rep_Strategy;
   int Peer_Selection;
   int Case_Responsable;
	int cte_trabajo;
   double Thao;
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
	    int _cte_trabajo
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
      for ( int i=0; i< nodes+1; i++)
         ends[i]=0;
   }

protected:

   void inner_body( void );

};

  
void mySystem::inner_body( void )
{
   
   char name[128];
   //cout << "mydebug" << endl;
   // Observer store statistics


// WSE
   wse = new WSE("WSE");
   wse->activate();
//~ cout<<"Simulador - WSE"<< endl;
   // Query generator                 
   generator = new Gen_rnd("GENERATOR", traces, &totalQueries, nodes, 
                           &obs, ends, Nuser, &wse, Peer_Selection); 

   // Initialize Red Tubo 
   transport = new RedTubo("RED");
   transport->activate();     
//~ cout<<"Simulador - 2"<< endl;
   // Initialize Array Tcl Peers 
   Peers = new handle<TlcProtocol>[nodes];
   handle<PastryProtocol> *pastryPeers = new handle<PastryProtocol>[nodes];
  
   Hash* h ;
	h = new Hash();
   handle<ClientTlcWse> *c;
   handle<Replicator> *r;
   //~ std::stringstream str;		//Para debug del Hash
	fflush(stdout);
	Hash HG;
	//~ std::string str2;
	//~ char *aux;
   // Inicitalize each layer on each peer
   srand(1);
   
   //~ ofstream nodeIDsFile;
   //~ nodeIDsFile.open("nodeIDsFile.txt");
   
    string nodeID_string;
	ifstream nodeIDsInput;
	nodeIDsInput.open("nodeIDsFile.txt");
   
   for ( int i=0; i < nodes; i++ )
   {
		sprintf(name, "Pastry_%d", i);
		// create a random nodeID
		//~ nodeid = h->CreateRandomKey();
		//Fixed Key (debug) ----------------------------------------------------------------
		fflush(stdout);
		nodeID_string.clear();
		getline(nodeIDsInput,nodeID_string);
		string aux3=nodeID_string;
		const char* aux2 = (char*)(aux3).c_str();
		BIGNUM *key = BN_new();
		BN_hex2bn(&key,aux2);
		//~ cout << "Peer: " << i  << ". Hash: " << BN_bn2hex(key) << endl;
		fflush(stdout);	
		BIGNUM* nodeid = key;
			
      pastryPeers[i] = new PastryProtocol(name,i,nodeid,leafset);
      pastryPeers[i]->set_H_obj( &pastryPeers[i] );
      pastryPeers[i]->addLowerLayer(&transport);
      pastryPeers[i]->activate();      
	

      sprintf(name,"ClientWSE_%d", i);
      c = new handle<ClientTlcWse>;
      (*c) = new ClientTlcWse(name, &wse); 
      (*c)->set_ClientWSE_obj(c);

      sprintf(name, "Replicator_%d",i);
      r= new handle<Replicator>;
      (*r) = new Replicator(name, Rep_Strategy);

      
      sprintf(name, "Tlc_%d", i); 
      Peers[i] = new TlcProtocol( name, 
                                  nodeid, 
				  cacheSize, 
                                  localCacheSize, 
				  lcacheSize,  
				  c,
				  &obs,
				  r,
				  totalQueries,
				  ends,
				  nodes,
				  Thao,
				  Rep_Strategy,
				  Case_Responsable,
				  cte_trabajo);
     
      Peers[i]->set_H_obj(&Peers[i]);
      Peers[i]->addLowerLayer(&pastryPeers[i]);

      (*c)->set_tlc(&Peers[i]);
      (*c)->activate();
      (*r)->set_tlc(&Peers[i]);
      (*r)->activate();

      generator->add_peer(&Peers[i]);
      Peers[i]->activate();

      // STATE BUILDER 
      pastryNetwork.push_back(&pastryPeers[i]);
   }
 //~ cout<<"Simulador - SB"<< endl;
   StateBuilder* sb = new StateBuilder(pastryNetwork, leafset);
   sb->execute();
     
	obs = new Observer(nodes, "OBSERVER",totalQueries);
   obs->activate();
     
    for (int i=0; i<nodes; i++){
		vector<NodeEntry*> leafset_aux;
		leafset_aux=((*pastryNetwork[i])->leafset)->listAllNodes();
		//~ cout << "Size Leafset Original: " << leafset_aux.size()<< endl;
		//~ for (int j=0;j<leafset_aux.size();j++){
			//~ cout << "Leafset ["<<j<<"] - PastryProtocol "<< i<<": "<< BN_bn2dec(leafset_aux[j]->getNodeID()) << endl;
		//~ }
	}

//~ cout<<"Simulador - GEN"<< endl;
   generator ->activate();


   //~ cout<<"Simulador: Comienza SIMULACION "<<endl; fflush(stdout);
   hold( simLen ); // Stops when the simulation time is achieve
   //~ cout<<"Simulador: Simulacion FINALIZADA "<<endl; fflush(stdout);
   
//-   obs->print();

   for ( int i = 0 ; i < nodes ; i ++)
   {
      pastryPeers[i]->freePastryPeers();
      Peers[i]->freeTlc();
   }
   generator->freeGen_rnd();
   //pastryNetwork.clear();
   //free(sb);

   // Estadistics
   // print statistics all simulation  
   end_simulation( );
}


int main( int argc, char* argv[] )
{ 

   char traces[2048];
    
   //Queries
   int totalQueries  = atoi(argv[1]);  //Total queries to process
   //~ cout << "Total Queries " << totalQueries << endl;      
  
   //Rcache
   int cacheSize     = atoi(argv[2]);       //Size of Rcache 
   ASSERT( cacheSize != 0 ); 
   //~ cout << "Cache Size " << cacheSize << endl;      
  
   //Local cache
   int localCacheSize     = atoi(argv[3]);       //Size in Local Rcache 
   //~ cout << "Local Cache Size " << localCacheSize << endl;      

   //Lcache
   int lcacheSize     = atoi(argv[4]);       //Size of LCache 
   //~ cout << "LCache Size " << lcacheSize << endl;      
  
   int nodes  = atoi(argv[5]);              // Number of nodes in the system
   //~ cout << "Number Nodes " << nodes << endl;      
  
   int leafset = atoi(argv[6]);		// Leafset size
   //~ cout << "Leafset Size " << leafset << endl;

   //DAG
   strcpy( traces , argv[7]);
   //~ cout << "Traces " << traces << endl;      

   //number of user
   int Nusers = atoi(argv[8]);

   double Thao = atof(argv[9]);

   //~ cout << "Nuser " << Nusers << endl;
   //~ cout << "Thao " << Thao << endl;
       
   int Rep_Strategy = atoi(argv[10]);
   //~ cout << "Rep_Strategy " << Rep_Strategy << endl;
   int Peer_Selection = atoi(argv[11]);
   //~ cout << "Peer_Selection " << Peer_Selection << endl;
   int Case_Responsable = atoi(argv[12]);
   //~ cout << "Case_Responsable " << Case_Responsable << endl; 
   int cte_trabajo = atoi(argv[13]);
   //~ cout << "cte_trabajo: " << cte_trabajo << endl; 
//~ cout<<"Simulador - argumetos OK"<< endl;
   sched_t schedType = SQSDLL;

   //simulation::instance( )->begin_simulation( new sqsPrio( ) );

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
				       cte_trabajo
                                     ) );

   system->activate( );

   simulation::instance( )->run( );
    
   simulation::instance( )->end_simulation( );
  
   //cout <<endl<< "done!" << endl;
   return 0;
} 
