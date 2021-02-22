// exponential_distribution
#include <iostream>
#include <random>
#include <chrono>
#include "../../../DESTimes/include/NDTime.hpp"
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>


void convertir_tiempo_a_NDTime(NDTime *t_in, double t_in_secs){
	*t_in=NDTime("00:00:00:000:000");
	int horas=(int)t_in_secs/3600;
	int minutos=((int)t_in_secs/60) - (horas*3600);
	int segundos=t_in_secs - (horas*3600) - (minutos*60);
	int milisec= (double)(t_in_secs - ((double)horas*3600.0) - ((double)minutos*60.0) - ((double)segundos*1.0) )*1000.0;
	//~ std::cout << "Conversion: " << t_in_secs << " - milisec " << milisec << std::endl;
	int microsec= (double)(t_in_secs - ((double)horas*3600.0) - ((double)minutos*60.0) - ((double)segundos*1.0) - (double)milisec*0.001)*1000000;
	//~ std::cout << "Conversion: " << t_in_secs << " - microsec " << microsec << std::endl;
	for (int i=0;i<horas;i++){
		*t_in += NDTime("01:00:00");
	}
	for (int i=0;i<minutos;i++){
		*t_in += NDTime("00:01:00");
	}
	for (int i=0;i<segundos;i++){
		*t_in += NDTime("00:00:01");
	}
	for (int i=0;i<milisec;i++){
		*t_in += NDTime("00:00:00:001");
	}
	for (int i=0;i<microsec;i++){
		*t_in += NDTime("00:00:00:000:001");
	}
	//~ std::cout << "Conversion: " << t_in_secs << " - " << *t_in << std::endl;
}

int main(int argc, char ** argv)
{
	const int nrolls=10000;  // number of experiments
	const int nstars=100;    // maximum number of stars to distribute
	const int nintervals=10; // number of intervals

	// construct a trivial random generator engine from a time-based seed:
	int seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator (seed);

	bool FLASH_CROWD=true;  // se utiliza o no el flash crowd
	double CROWD_TIME=5.0; // segundos con flash crowd
	double NORMAL_TIME=500.0; //segundos con tasa normal
	double NORMAL_RATE=1000.0; // tasa de arribo sin flash crowd
	double CROWDED_RATE=5000.0; // tasa de arribo con flash crowd
	
	double par;
	par=std::stod(argv[1]);
	int totalQueries;
	totalQueries=std::stoi(argv[2]);
	int NP;
	NP=std::stoi(argv[3]);
	totalQueries+=1;
	std::exponential_distribution<double> distribution(par);
	//~ std::cout << "Tasa de arribo: " << par << std::endl;
	
	std::ofstream txtOut;
	txtOut.open ("Log_gen2_"+std::to_string(totalQueries-1)+".txt");
	
	double number;
	NDTime tiempo=NDTime("00:00:00");
	NDTime tiempo_acc=NDTime("00:00:00");

	std::ifstream infile;
	infile.open("RISE_20000.DAT");
	std::string line;
	
	std::ifstream infile_time;
	infile_time.open("Backup_Times/Times_"+std::to_string(totalQueries-1)+".txt");
	std::string line_time;
	
	std::ifstream infile_chosen;
	infile_chosen.open ("Backup_chosen_peers/ChosenPeers_"+std::to_string(NP)+".txt");
	std::string line_chosen;
	
	int n=0;
	
	NDTime::startDeepView();
	std::getline(infile, line);			//Descartar la primer línea del LOG, ya que no es una Query
	while (std::getline(infile, line))
	{		
		if (n>totalQueries){	
			//Keep same time
		}else{
			std::getline(infile_time, line_time);	//Read Time Log.
			std::getline(infile_chosen, line_chosen);	//Read Time Log.
		}
		
		
		std::cout << n<< " "<< line_time << std::endl;
		/**********************************************************************/
		double elapsed_time;
		if(FLASH_CROWD)
		{
			if(elapsed_time < CROWD_TIME ){
				double newRate = (1.0/NORMAL_RATE);		
				//~ delete distribution;
				std::exponential_distribution<double> distribution(newRate);		
			}
			else if( (elapsed_time > CROWD_TIME) &&(elapsed_time > NORMAL_TIME) )
			{
				double newRate = (1.0/CROWDED_RATE);
				//~ delete distribution;
				std::exponential_distribution<double> distribution(newRate);		
			}
			else if(elapsed_time > NORMAL_TIME)
			{
				double newRate = (1.0/NORMAL_RATE);
				//~ delete distribution;
				std::exponential_distribution<double> distribution(newRate);		
			}	
			
			if (n>=totalQueries){	break;	}
			number = distribution(generator);
			number = std::stod(line_time);
			//~ number=0.0002;
			convertir_tiempo_a_NDTime(&tiempo, number);
			elapsed_time+=number;
			tiempo_acc+=tiempo;
			txtOut << tiempo_acc << " " << line_chosen << ";" << line << std::endl;
			n++;
		}
		else
		{
			if (n>=totalQueries){	break;	}
			number = distribution(generator);
			number = std::stod(line_time);
			//~ number=0.0002;
			convertir_tiempo_a_NDTime(&tiempo, number);
			tiempo_acc+=tiempo;
			txtOut << tiempo_acc << " " << line_chosen << ";" << line << std::endl;
			n++;
		}
		//~ std::cout << "TIME GEN: " << tiempo_acc << std::endl;
		
	}
	txtOut.close();
	return 0;
}
