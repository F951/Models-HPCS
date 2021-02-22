#include "gen_rnd.h"
#include "../client/Query.h"


void Gen_rnd::inner_body( )
{
	string line, terms;
	char *ptr;
	int id = 0;
	endStream.open( traces_file );

	//La primera linea se tira si se usa el Log de Yahoo
	getline( endStream, line );
	
	std::ofstream OutTimes;
	OutTimes.open ("Times_"+std::to_string(*totalQueries)+".txt");
	
	std::ofstream OutChosenServers;
	OutChosenServers.open ("ChosenServers_"+std::to_string(nro_servers)+".txt");

	std::ifstream inTimes;
	inTimes.open ("input_values/Times_"+std::to_string(*totalQueries)+".txt");
	string line2;
	
	std::ifstream inChosenServers;
	inChosenServers.open ("input_values/ChosenServers_"+std::to_string(nro_servers)+".txt");
	string line3;
	
	int chosen;
	double CTE;
	
	int generate_logs =0;
	
	while( 1 )
	{
		if( ! getline( endStream, line ) ) passivate();

		prev = actual;
		actual = this->time();

		tokens.clear();
		util->Tokenize( line, tokens, "\t");
		terms = tokens[2];
		tokens.clear();
		util->Tokenize( terms, tokens, " ");
		ptr = util->obtain_terms( tokens );
		
		BIGNUM* hashValue = h->GenerateKey(ptr); 
		
		if (generate_logs){
			chosen = rand() % nro_servers;
			OutChosenServers << chosen << endl;
			CTE = rand() % 1000;
			CTE = CTE/1000.0;
			OutTimes << CTE << endl;
		}else{
			getline( inChosenServers, line3 );
			chosen=stoi(line3);
			
			getline( inTimes, line2 );
			CTE=stod(line2);
		}
		
		hold( CTE );
		
		Message* serverQuery = new Message(NULL, hashValue, ptr, USER,chosen,CTE,sentQueries);
		
		(*server[chosen])->add_request(serverQuery);
		if ((*server[chosen])->idle() && !(*server[chosen])->get_busy())
			(*server[chosen])->activateAfter(current());
		delete [] ptr;
		
		sentQueries++;
		
		id++;
		if (sentQueries >= (*totalQueries) )
		{
			ends[NP] = 1; //Indica que se ha enviado la ultima consulta             
			endStream.close();
			passivate();
		}
		
		
	}
   
}

