#ifndef GC_H
#define GC_H

#include <ilcplex/ilocplex.h>
#include <vector>

#include "Branch.h"
#include "Column.h"
#include "Pricing.h"
#include "ProblemData.h";
#include "Utility.h"

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
	OneDimRange constraint_cpu;
	TwoDimRange constraint_lambda;
	ThreeDimRange constraint_saida;
	ThreeDimRange constraint_entrada;

	OneDimRange cover_cuts_;

	ProblemData * data;
	
	float Values[10000];
	float Weights[10000];

	public:
		GC(ProblemData * data);
		GC(GC * parent);
	~GC();
		void GenerateCoverCuts();
		int Solve(Branch * branch);
		void BuildModel();	
		void CreateVariables();
		void CreateObjectiveFunction();
		void CreateConstraints();
		void AddColumns(std::vector<Column> colunas);
		void getDuals(IloNumArray2 * gamma, IloNumArray3 * alpha, IloNumArray3 * pi, IloNumArray * beta);
		void SetCplexParameters();
		float getGAP();

		bool CoverCut(IloRangeArray constraints);

		void addBranchLambda(int m, int valor);
		void addBranch(Branch branch, int valor);
		/*float tempoMaster =0, tempoSub=0, tempoTotal=0, tempoRelaxacao=0;
		float ub = INFINITY;
		float lb = -INFINITY;
		bool sol_inteira = true;
		unsigned int id = 1;
		float parentUB = INFINITY;
		unsigned int nCols = 0, gCols = 0;*/
		float tempoMaster, tempoSub, tempoTotal, tempoRelaxacao;
		float ub;
		float lb;
		bool sol_inteira;
		unsigned int id;
		float parentUB;
		unsigned int nCols, gCols;
		std::vector<Column> parentPool;
		std::vector<Column> forbidden;
		std::vector<Branch> branchs;
		std::vector<Column> pool;
		Pricing * pricing;
};

#endif /* GC_H */

