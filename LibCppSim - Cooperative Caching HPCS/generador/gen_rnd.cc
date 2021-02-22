#include "gen_rnd.h"
#include "../applicationLayer/TlcProtocol.h"
#include "../applicationLayer/Query.h"
#include <time.h>
#include <stdlib.h>
 	#include <fstream>

inline bool exists_test (const std::string& name) {
    ifstream f(name.c_str());
    return f.good();
}

void Gen_rnd::inner_body( )
{
	handle<TlcProtocol> *gq;
	string line, terms;
	char *ptr;
	Query *q;
	int id = 0;
	int chosen=0;
	int envP2P = 0;  
	cout << "Archivo Trazas: " << traces_file << " - Get_Query.h" << endl;
	endStream.open( traces_file );

	
	std::string name_times="Backup_Times/Times_"+std::to_string(*totalQueries)+".txt";
	std::string name_peers="Backup_chosen_peers/ChosenPeers_"+std::to_string(NP)+".txt";
	int flag_files=0;
	if (exists_test(name_times) && exists_test( name_peers)){
		flag_files=1;
	}
	
	std::ifstream txtTimes;
	std::ifstream txtChosen;
	if (flag_files){
		//Use prerecorded values
		txtTimes.open ("Backup_Times/Times_"+std::to_string(*totalQueries)+".txt");
		txtChosen.open ("Backup_chosen_peers/ChosenPeers_"+std::to_string(NP)+".txt");
	}
	
	
	//Use random values:
	
	//La primera linea se tira si se usa el Log de Yahoo
	getline( endStream, line );
	
	srand ( 1 );

	double phase_time = this->time();

	srand(1);
	string line2;
	string line3;					

	//cout<<"Linea "<<linea<<endl;
	while( 1 )
	{
		if( ! getline( endStream, line ) ) passivate();

		prev = actual;
		actual = this->time();
		//     double time = actual - prev;

		tokens.clear();
		util->Tokenize( line, tokens, "\t");
		terms = tokens[2];
		tokens.clear();
		util->Tokenize( terms, tokens, " ");
		ptr = util->obtain_terms( tokens );
		cout << "PTR: " << ptr << endl;
		//  Verifica si va a un peer o al WSE, cuando la consulta llega desde un nodo fuera
		//  de la red de peers
		double prob = SelectSource->value();
		if (prob > porcentaje_peers)//viene fuera de la red peer
		{
			BIGNUM* hashValue = h->GenerateKey(ptr); 
			MessageWSE* wseQuery = new MessageWSE(NULL, hashValue, ptr, USER);
			delete [] ptr;
			(*wse)->add_request(wseQuery);
			if ((*wse)->idle() && !(*wse)->get_busy())
				(*wse)->activateAfter(current());
		}
			else // la consulta llega a un peer
		{
			envP2P = 1; 
			gq= Peers[0];
			Peer_Selection=1;
			//~ switch((int)Peer_Selection)
			switch(Peer_Selection)
			{
				case 0:
				{
					//RANDOM SELECTION -> UNIFORM
					int rnd = rand()%NP;
					gq = Peers[rnd];
					ends[rnd]++;   
					break;
				}
				case 1:
				{
					
					
					fflush(stdout);
					int zipf;
					if (flag_files){
						//Use prerecorded values:
						getline( txtChosen, line2 );
						zipf = stoi(line2);
					}else{
						//Use random values:
						//ZIPF SELECTION -> CLUSTERING
						rand_val(1);
						zipf = getZipf(1.0, NP-1);
						//cout << "Peer " << zipf <<endl;
					}
					
					gq = Peers[zipf];
					ends[zipf]++;
					//~ txtOutChosen << zipf << endl;
					cout << "chosen" << zipf << endl;
					break;
				}
				case 2:
				{
					// STATIC DEBUG PROPOSE - Round Robin
					gq = Peers[chosen];
					/********************************/
					//~ chosen=0;
					//~ cout << "chosen: " <<chosen << endl;
					/********************************/
					ends[chosen]++;
					chosen++;
					if(chosen>=NP)
					chosen=0;
					break;
				}
			}

			BIGNUM* hashValue = h->GenerateKey(ptr);
			if (0){std::cout << "PTR generator: " << ptr << " - Hash: " << BN_bn2dec(hashValue)<<   " - CurrentQueries: " <<  sentQueries <<   std::endl;}
			//cout << " Saliendo Query " << sentQueries << "-" <<id << endl;
			q = new Query(sentQueries, ptr, hashValue,  this->time());
			(*observer)->addNQueriesOut();
			delete [] ptr;
			//~ cout << "Generator send query to peer: " << (*gq)->name() << ". Time: " << this->time() << endl;        
			(*gq)->add_query( q );     
			if( (*gq)->idle())// && !(*gq)->get_busy() )
			{
				(*gq)->activateAfter( current() );
			}
		}//else

		///CTE = arrival_time->value();
		sentQueries++;


		if(FLASH_CROWD)
		{
			double elapsed_time=this->time()-phase_time;
			//cout << "elapsed: " << elapsed_time << endl;
			if(!phase)
			{
				if(elapsed_time > NORMAL_TIME)
				{
					//cout << "change to crowd " << this->time() << endl; 
					phase=true;
					phase_time= this->time();
					double newRate = (1.0/CROWDED_RATE);
					delete arrival_time;
					arrival_time = new rngExp("Arrive Time", newRate);
					arrival_time->reset();
					
				}
			}
			else
			{
				if(elapsed_time > CROWD_TIME)
				{
					//cout << "change to normal" << this->time() << endl;
					phase=false;
					phase_time=this->time();
					double newRate = (1.0/NORMAL_RATE);
					delete arrival_time;
					arrival_time= new rngExp("Arrive Time", newRate);
					arrival_time->reset();
				}	
			}
		}
		
		
		if (flag_files){
			//Use prerecorded values:
			getline( txtTimes, line3 );
			CTE = stod(line3);
		}else{
			//Use random values:
			CTE=arrival_time->value();
		}
		cout << "Generator - time next hold: " << CTE << ". Current time: " << this->time() << ". Next time: " << this->time() + CTE << endl;

		//~ txtOutTimes << CTE <<  endl;
		id++;
		
				//cout<<"Generador envia "<<sentQueries<<endl; 
		if (sentQueries >= (*totalQueries) )
		{
			ends[NP] = 1; //Indica que se ha enviado la ultima consulta             
			//cout<<"ULTIMA CONSULTA "<<endl;   
			if ( envP2P == 0 ) //No se enviaron consultas a los peers
			{
				//Apaga replicadores
				for( int i=0 ;i<(int)Peers.size();i++)
				{ 
					gq = Peers[i];
					//~ (*gq)->EndReplicator();
				}
			}   
			// (*observer)->end();
			endStream.close();
			passivate();
		}
		hold( CTE );
		cout << "Generator - Hold done - Current time: " << this->time() << endl;
	}
   //endStream.close();
}

int Gen_rnd::getZipf (int alpha, int n)
{
	//~ srand(1);
	//~ srand48(time(NULL));
	//~ srand ( time(NULL) );
	static bool first = true;      // Static first time flag
	static double c = 0;          // Normalization constant
	double z;                     // Uniform random number (0 < z < 1)
	double sum_prob;              // Sum of probabilities
	double zipf_value=0;            // Computed exponential value to be returne
	int i;                        // Loop counter

	// Compute normalization constant on first call only
	if (first == true)
	{
		for (i=1; i <= n; i++)
		c = c + (1.0 / pow((double) i, alpha));
		c = 1.0 / c;
		first = false;
		//cout << "PRIMERO" << endl;
	}

	// Pull a uniform random number (0 < z < 1)
	do
	{
		z = (((double) rand())/(RAND_MAX+1.0));
		// cout << "Z VALUE " << z << endl;
	}
	while ((z == 0) || (z == 1));
	
	// Map z to the value
	sum_prob = 0;
	for (i=1; i<=n; i++){
		//      cout <<"Z " << z << endl;
		//     cout << "SUM PROB: " << sum_prob << endl;
		//      cout << "C " << c <<endl;
		//      cout << "ALPHA " << alpha << endl;
		sum_prob = sum_prob + (c / pow((double) i, alpha));
		if (sum_prob >= z)
		{
			zipf_value = i;
			break;
		}
	}
	//   cout << "ZIPF VALUE " << zipf_value << endl;
	// Assert that zipf_value is between 1 and N
	ASSERT((zipf_value >=1) && (zipf_value <= n));
	return zipf_value ;
}


void Gen_rnd::setQueryRate( int newRate)
{
	srand(1);
   // newRate queries/sec
   switch (QUERY_RATE_STRATEGY)
   {
      // DELTA T TIME
      case 0:
         if(this->time() > QUERY_DELTA_T )
	 {
            double lambda = (1.0/newRate);
	    delete arrival_time;
	    arrival_time = new rngExp("Arrive Time", lambda);
	    arrival_time ->reset();
	 }  
         break;

      // Q QUERIES 
      case 1:
         if( (lastStepQueries + QUERY_DELTA_Q) < sentQueries)
         {
	    lastStepQueries = sentQueries;
	    double lambda = (1.0/newRate);
	    delete arrival_time;
	    arrival_time = new rngExp("Arrive Time", lambda);
	    arrival_time ->reset();
	 }
	 break;
   }
}


 double Gen_rnd::rand_val(int seed)
 { 
	 //~ srand(1);
    const long a =16807;// Multiplier
    const long m = 2147483647;// Modulus
    const long q = 127773;// m div a
    const long r = 2836;// m mod a
    static long x; // Random int value
    long x_div_q; // x divided by q
    long x_mod_q; // x modulo q
    long x_new; // New x value

 // Set the seed if argument is non-zero and then return zero
    if (seed > 0)
    {
       x = seed;
       return(0.0);
    }
 //
 // // RNG using integer arithmetic
     x_div_q = x / q;
     x_mod_q = x % q;
     x_new = (a * x_mod_q) - (r * x_div_q);
     if (x_new > 0)
        x = x_new;
     else
        x = x_new + m;
 //
 // // Return a random value between 0.0 and 1.0
    return((double) x / m);
 // }


}
