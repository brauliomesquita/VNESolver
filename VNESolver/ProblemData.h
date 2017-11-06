#ifndef PROBLEMDATA_H
#define PROBLEMDATA_H

#include <vector>
#include <iostream>

#include "Graph.h"
#include "Request.h"

using namespace std;

class ProblemData {
	Graph *substrate_;
	
	int number_vns_;

	// Parameters
	bool location_;
	bool delay_;
	bool resilience_;

	bool ilp_;
	bool bcp_;

	int fo_;
	int time_limit_;

	char * outputFile_;

public:
	ProblemData();
	~ProblemData();
	
	bool ReadParameters(int argc, char* argv[]);
	bool ReadInputData(int argc, char *argv[]);
	bool ReadSubstrate(char * subGraph);
	bool ReadVNsFolder(char * folder, int numberVNs);
	
	Graph* getSubstrate();
	Request* getRequest(int i);
	int numberVns();

	bool getILP(){ return ilp_; }
	bool getBCP() { return bcp_; }

	bool getLocation(){ return location_;}
	bool getDelay() { return delay_; }
	bool getResilience() {return resilience_; }
	int getOptimizationObjective() { return fo_; }
	int getTimeLimit() { return time_limit_; }

	std::vector<Request*> requests_;
};

#endif