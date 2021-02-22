#include "Observer.h"
#include <sstream>
#include <iomanip>      // std::setprecision

void Observer::inner_body(void)
{

   stringstream outStream;
   outStream << "time;Peers;nQueriesOut;nQueriesIn;hitLocal;hitGlobal;hitLCache;miss;avHop;nReplicas;Gini;avLat" << endl;
   
   s = outStream.str();
   cout << outStream.str();
   fflush(stdout);

   while (1)
   {
      
	if (ends) passivate();

      hold(DELTA_O);

      // compute statistics
      outStream.str("");

      double avHop=0;
      double avLatency=0;

      if(nqueriesIn >0)
      {
      	avHop = (double)hops/nqueriesIn;
         avLatency=(double)latencia/nqueriesIn;
      }
      //~ cout << "Latencia: " << latencia << ". avLat: " << avLatency << ".nqueries_in: " << nqueriesIn << endl;
      //~ cout << "Ends: " << ends << endl;
    //  else
     // {
      // avHop=-1;
      // avLatency-1;
     // }

      double gini = Observer::getGini();
      iteracion++;
			//~ cout << "#Vector: " << "LOAD" << endl;
			//~ for (int k=0;k<(int)(load.size());k++){
				//~ cout << " " << load[k];
			//~ }cout << "\n";
		outStream << std::fixed;
		outStream << setprecision(9) <<  this->time() << ";" << np << ";" << nqueriesOut << ";" << nqueriesIn << ";" << hitLocal ;
		outStream<< ";"  << hitGlobal << ";" << hitLCache << ";" ;
		outStream << miss << ";" << avHop << ";" << nReplicas;
		outStream << ";" << gini << ";" << avLatency << endl;
		
		//~ cout << "observer debug"<< endl;
		
      s.append(outStream.str());
      cout << outStream.str();	  
      fflush(stdout);      
      // limpia variables
      /************************
      nqueriesIn=0;
      nqueriesOut=0;
      hitLocal=0;
      hitGlobal=0;
      hitLCache=0;
      miss=0;
      hops=0;
      latencia=0;
      nReplicas=0;
      for(int i =0; i < np; i++)
      {
         load[i]=0;
      }
		************************/
      // dormir
      if (nqueriesIn == totalqueries) {
		  ends=1;
		  passivate();
	  }
      
   }

}



double Observer::getGini()
{
    double l=0;
    double gini=0;
    double mean=0;

	vector<int> load2=load;
	
    for(int i=0; i< np ; i++)
    {
       l=l+load2[i];
    }
    
    mean=l/np;

    sort(load2.begin(),load2.end() );

    for (int i=0; i < np; i++)
    {
       int j= i + 1;
       gini = gini + (( 2 * j - ( np - 1 )) * load2[i] );
    }

    if (mean>0)
       gini= gini / (np*np*mean);

    return gini;
}

int Observer::getIteracion()
{
   return iteracion;
}

