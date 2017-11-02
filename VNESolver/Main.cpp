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
	bool ret = data->ReadInputData(argv);

	if(!ret)
		return -1;

	bool location = true;
	bool resilience = false;
	bool delay = false;

	ILPModel ilp;
	ilp.Solve(data);

	return 0;
}
