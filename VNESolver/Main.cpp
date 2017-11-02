#include "Request.h"
#include "ILPModel.h"
//#include "Heuristica.h"

#include <stdio.h>
#include <iostream>
#include "ProblemData.h"
#include "BP.h"

using namespace std;

int main(int argc, char *argv[])
{
	ProblemData * data = new ProblemData();
	bool ret = data->ReadInputData(argv);

	if(!ret)
		return -1;

	/*ILPModel ilp;
	ilp.Solve(data);*/

	/*BP bp;
	bp.Solve(data);*/

	KnapsackProblem * kp = new KnapsackProblem();

	float Weights[] = {2.75, 2.25, 30, 14.25, 25};
	float Values[] = {1.1, 2.02, 3.003, 4.0004, 5.00005};

	float retKP = kp->Solve(5, Weights, Values, 5);

	cout << retKP << endl;

	return 0;
}
