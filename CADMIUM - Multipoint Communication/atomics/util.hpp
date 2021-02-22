#ifndef __util_HPP__
#define __util_HPP__

NDTime convertir_tiempo(double _Tin){
		int hours = (int) _Tin/3600;
		int mins = (int) (_Tin - hours*3600) / 60;
		int secs = (int) (_Tin - hours*3600 - mins*60);
		int msecs =  1000.0*(_Tin - hours*3600 - mins*60 - secs);
		std::string Shours = std::to_string(hours);
		std::string Smins = std::to_string(mins);
		std::string Ssecs = std::to_string(secs);
		std::string Smsecs = std::to_string(msecs);
		std::string TotalTime = Shours+":"+Smins+":"+Ssecs+":"+Smsecs;
		//std::cout << "_Tin: " << _Tin << std::endl;
		//std::cout << "Shours: " << Shours << std::endl;
		//std::cout << "Smins: " << Smins << std::endl;
		//std::cout << "Ssecs: " << Ssecs << std::endl;
		//std::cout << "Smsecs: " << Smsecs << std::endl;
		//std::cout << "TotalTime: " << TotalTime << std::endl;
		return TotalTime;
	}

#endif // __util_HPP__
