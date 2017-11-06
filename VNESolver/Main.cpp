#include "Request.h"
#include "ILPModel.h"
//#include "Heuristica.h"

#include <stdio.h>
#include "ProblemData.h"
#include "BP.h"

using namespace std;

int main(int argc, char *argv[])
{
	std::ofstream out("out.txt");
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(out.rdbuf());

	ProblemData * data = new ProblemData();
	bool ret = data->ReadInputData(argc, argv);

	if(!ret){
		return -1;
	}

	if(data->getILP()){
		ILPModel ilp;
		ilp.Solve(data);
	}

	if(data->getBCP()){
		BP bp;
		bp.Solve(data);
	}

	return 0;
}
