#ifndef GC_H
#define GC_H

#include <ilcplex/ilocplex.h>
#include <vector>
#include <malloc.h>
#include <fstream>

#include "Request.h"
#include "Column.h"
#include "MappingNode.h"
#include "Pricing.h"
#include "Utility.h"
#include "Branch.h"

using namespace std;

#define M -5000
#define INFINITY 10000

typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<NumVarMatrix> NumVar3Matrix;
typedef IloArray<NumVar3Matrix> NumVar4Matrix;
typedef IloArray<NumVar4Matrix> NumVar5Matrix;

typedef IloRangeArray OneDimRange;
typedef IloArray<IloRangeArray> TwoDimRange;
typedef IloArray<IloArray<IloRangeArray>> ThreeDimRange;
typedef IloArray<IloArray<IloArray<IloRangeArray>>> FourDimRange;

class GC {
	IloEnv env;
	IloModel model;
	IloObjective objective;
	IloCplex * master;

	/* Variáveis do Modelo */
	IloNumVarArray y;
	IloNumVarArray lambda;
	NumVar3Matrix z;

	/* Restrições */
	OneDimRange constraint_bw;
	TwoDimRange constraint_lambda;
	ThreeDimRange constraint_saida;
	ThreeDimRange constraint_entrada;

	Graph *substrate;
	std::vector<Request*> requests;
	
	
	public:
		GC();
		GC(GC * parent);
		void Solve(Graph *substrate, std::vector<Request*> requests, bool location, bool delay, bool resilience, int *y_, Branch *branch, unsigned int *saida);
		void CreateVariables();
		void CreateObjectiveFunction();
		void CreateConstraints();
		void addColumns(std::vector<Column> colunas);
		void getDuals(IloNumArray2 * gamma, IloNumArray3 * alpha, IloNumArray3 * pi, IloNumArray * beta);
		void SetCplexParameters();
		double getGAP();

		void addBranchLambda(int m, int valor);
		void addBranch(Branch branch, int valor);
		/*double tempoMaster =0, tempoSub=0, tempoTotal=0, tempoRelaxacao=0;
		double ub = INFINITY;
		double lb = -INFINITY;
		bool sol_inteira = true;
		unsigned int id = 1;
		double parentUB = INFINITY;
		unsigned int nCols = 0, gCols = 0;*/
		double tempoMaster, tempoSub, tempoTotal, tempoRelaxacao;
		double ub;
		double lb;
		bool sol_inteira;
		unsigned int id;
		double parentUB;
		unsigned int nCols, gCols;
		std::vector<Column> parentPool;
		std::vector<Column> forbidden;
		std::vector<Branch> branchs;
		std::vector<Column> pool;

		bool location;
};

#endif /* GC_H */

