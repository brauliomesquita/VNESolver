#ifndef PROBLEMDATA_H
#define PROBLEMDATA_H

#include <vector>
#include <iostream>

#include "Graph.h"
#include "Request.h"

using namespace std;

class ProblemData {
	Graph *substrate_;
	std::vector<Request*> requests_;
	int number_vns_;

	// Parameters
	bool location_;
	bool delay_;
	bool resilience_;

	int fo_;
	int time_limit_;

public:
	ProblemData();
	~ProblemData();

	void ReadSubstrate(char * subGraph);
	void ReadVNsFolder(char * folder, int numberVNs);
	
	Graph* getSubstrate();
	Request* getRequest(int i);
	int numberVns();

	bool getLocation(){ return location_;}
	bool getDelay() { return delay_; }
	bool getResilience() {return resilience_; }
	bool getOptimizationObjective() { return fo_; }
	bool getTimeLimit() { return time_limit_; }
};

#endif