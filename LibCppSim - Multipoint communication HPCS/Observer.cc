#include "Observer.h"
#include <sstream>

	void Observer::inner_body(void)
	{

		stringstream outStream;
		outStream << "time" <<  ";" << "D1" << ";" << "D2" << ";" << "D3" << ";" << "Servers" << ";" << "Peers" << ";" << "nQueries" << ";" << "terminadas"<< ";" << "latencia"<< ";" << "latencia_network" << endl;

		s = outStream.str();
		cout << outStream.str();
		fflush(stdout);

		while (1)
		{

			hold(DELTA_O);

			outStream.str("");

			iteracion++;
			outStream << this->time() <<  ";" << param_server<< ";" << param_network<<";" << param_peer<<";" << nro_servers << ";" << np << ";" << totalQueries << ";" << terminadas<< ";" << latencia<< ";" << latencia_network << endl;

			s.append(outStream.str());
			cout << outStream.str();	  
			fflush(stdout);      

			if (terminadas>=totalQueries) 	{
				passivate();  
			}

		}

	}




