#ifndef ILPMODEL_H_
#define ILPMODEL_H_

#include <ilcplex/ilocplex.h>
#include <vector>

class ProblemData;
using namespace std;

#define M 10000

typedef IloArray<IloIntVarArray> IntVarMatrix;
typedef IloArray<IntVarMatrix> IntVar3Matrix;
typedef IloArray<IntVar3Matrix> IntVar4Matrix;
typedef IloArray<IntVar4Matrix> IntVar5Matrix;

class ILPModel {
public:
	ILPModel();

	void setRelaxacao();
 	float Solve(ProblemData * data);
	void SetCplexParameters(ProblemData * data);

private:
	bool relaxacao;

	IntVar4Matrix x;
	IntVar3Matrix z;
	IloIntVarArray y;
	IloCplex * problem;
};

#endif /* ILPMODEL_H_ */
