#include "Request.h"
#include "ILPModel.h"
//#include "Heuristica.h"

#include <stdio.h>
#include <iostream>
#include "ProblemData.h"

using namespace std;

int main(int argc, char *argv[])
{
	ProblemData * data = new ProblemData();
	data->ReadSubstrate(argv[1]);
	data->ReadVNsFolder(argv[2], atoi(argv[3]));

	/*substrate->setDist(substrate);
	for(int v=0; v<requests.size(); v++){
		requests[v]->getGraph()->setDist(substrate);
	}*/

	bool location = true;
	bool resilience = false;
	bool delay = false;

	ILPModel ilp;
	ilp.Solve(data);

	return 0;
}
