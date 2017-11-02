#include "Utility.h"

float get_time() {
	/*struct tms time_;
	 clock_t time_tic;
	 float cpt = sysconf(_SC_CLK_TCK);
	 times(&time_);
	 time_tic = time_.tms_utime;
	 return (float) (time_tic / cpt);*/
	/*struct tms time_;
	clock_t time_tic;
	float cpt = sysconf(_SC_CLK_TCK);
	time_tic = times(&time_);*/
	return (float) 5;//(time_tic / cpt);
}

int round(float value){
	return (int) (value + 0.5);
}