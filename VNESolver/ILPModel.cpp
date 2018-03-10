#include "ILPModel.h"
#include "ProblemData.h"

ILPModel::ILPModel() {

}

void ILPModel::SetCplexParameters(ProblemData * data) {
	problem->setParam(IloCplex::TiLim, data->getTimeLimit());
	//problem->setParam(IloCplex::Threads, 1);

	return;

	problem->setParam(IloCplex::PreInd, 0);
	problem->setParam(IloCplex::AggInd, 0);
	problem->setParam(IloCplex::HeurFreq, -1);

	problem->setParam(IloCplex::NodeSel, CPX_NODESEL_DFS);
	problem->setParam(IloCplex::NodeSel, CPX_NODESEL_BESTBOUND);

	problem->setParam(IloCplex::FracCuts, -1);
	//problem->setParam(IloCplex::LiftProjCuts, -1);
	problem->setParam(IloCplex::FlowCovers, -1);
	problem->setParam(IloCplex::GUBCovers, -1);
	problem->setParam(IloCplex::Covers, -1);

	problem->setParam(IloCplex::ZeroHalfCuts, -1);
	problem->setParam(IloCplex::ImplBd, -1);
	problem->setParam(IloCplex::Cliques, -1);
	problem->setParam(IloCplex::DisjCuts, -1);
	problem->setParam(IloCplex::FlowPaths, -1);
	problem->setParam(IloCplex::MIRCuts, -1);

}

float ILPModel::Solve(ProblemData * data) {
	IloEnv env;
	IloModel model(env);
	IloObjective objective(env);
	problem = new IloCplex(env);

	problem->out() << "Result Compact Model  " << endl;
	problem->out() << "Substrate Size:       " << data->getSubstrate()->getN() << endl;
	problem->out() << "Number os VNs:        " << data->numberVns() << endl;
	problem->out() << "Parameters: " << endl;
	if(data->getLocation())
		problem->out() << "Location" << endl;
	if(data->getDelay())
		problem->out() << "Delay" << endl;
	if(data->getResilience()){
		problem->out() << "Resilience" << endl;
		//P = 2;
	}
	problem->out() << "Objective Function:" << endl;
	if(data->getOptimizationObjective() == 0)
		problem->out() << "Maximize Profit" << endl;
	if(data->getOptimizationObjective() == 1)
		problem->out() << "Minimize Cost" << endl;
	if(data->getOptimizationObjective() == 2)
		problem->out() << "Load Balance" << endl;
	if(data->getOptimizationObjective() == 3)
		problem->out() << "Minimize Delay" << endl;

	problem->out() << endl << endl;

	SetCplexParameters(data);

	x = IntVar4Matrix(env, data->numberVns());
	z = IntVar3Matrix(env, data->numberVns());
	y = IloIntVarArray(env, data->numberVns());

	char var_name[256];
	int num_vars = 0;

	for (int v = 0; v < data->numberVns(); v++) {
		x[v] = IntVar3Matrix(env, data->getRequest(v)->getGraph()->getN());

		for (int kl = 0; kl < data->getRequest(v)->getGraph()->getM(); kl++) {
			x[v][kl] = IntVarMatrix(env, data->getSubstrate()->getN());

			for(int i = 0; i<data->getSubstrate()->getN(); i++){
				x[v][kl][i] = IloIntVarArray(env, data->getSubstrate()->getN());

				for(int j = 0; j<data->getSubstrate()->getN(); j++){
					if(data->getSubstrate()->getAdj(i, j)!=-1){

						sprintf(var_name, "x_%d_%d_%d_%d", v, kl, i, j);
						x[v][kl][i][j] = IloIntVar(env, 0, 1, var_name);
						model.add(x[v][kl][i][j]);
						num_vars++;
					}
				}
			}
		}
	}

	for (int v = 0; v < data->numberVns(); v++) {
		z[v] = IntVarMatrix(env, data->getRequest(v)->getGraph()->getN());
		for (int k = 0; k < data->getRequest(v)->getGraph()->getN(); k++) {
			z[v][k] = IloIntVarArray(env, data->getSubstrate()->getN());
			for (int i = 0; i < data->getSubstrate()->getN(); i++) {

				if (data->getLocation() && data->getRequest(v)->getGraph()->getDist(k, i) > data->getRequest(v)->getMaxD())
					continue;

				sprintf(var_name, "z_%d_%d_%d", v, k, i);
				z[v][k][i] = IloIntVar(env, 0, 1, var_name);
				model.add(z[v][k][i]);
				num_vars++;
			}
		}
	}

	for (int v = 0; v < data->numberVns(); v++) {
		sprintf(var_name, "y_%d", v);
		y[v] = IloIntVar(env, 0, 1, var_name);
		model.add(y[v]);
		num_vars++;
	}

	IloExpr obj(env);

	// Maximize Profit
	if(data->getOptimizationObjective() == 0){
		for (int v = 0; v < data->numberVns(); v++) {
			obj += data->getRequest(v)->getProfit() * y[v];
			cout << "VN_" << v << " Profit: " << data->getRequest(v)->getProfit() << endl;
		}

		objective = IloAdd(model, IloMaximize(env));
	}


	// Minimize Cost
	if(data->getOptimizationObjective() == 1){
		for (int i = 0; i < data->getSubstrate()->getN(); i++) {
			for (int j = 0; j < data->getSubstrate()->getN(); j++) {

				if (data->getSubstrate()->getAdj(i,j) == -1)
					continue;

				for (int v = 0; v < data->numberVns(); v++) {
					for (int kl = 0; kl < data->getRequest(v)->getGraph()->getM(); kl++) {

						obj += data->getSubstrate()->getCost(data->getSubstrate()->getAdj(i, j)) * data->getRequest(v)->getGraph()->getEdges()[kl].getBW() * x[v][kl][i][j];
					}
				}
			}
		}

		for (int v = 0; v < data->numberVns(); v++) {
			obj += (1 - y[v]) * 10000;
		}

		objective = IloAdd(model, IloMinimize(env));
	}

	/*if(data->getOptimizationObjective() == 2)
	problem->out() << "Load Balance" << endl;
	if(data->getOptimizationObjective() == 3)
	problem->out() << "Minimize Delay" << endl;*/

	objective.setExpr(obj);
	obj.end();

	for (int v = 0; v < data->numberVns(); v++) {
		for (int k = 0; k < data->getRequest(v)->getGraph()->getN(); k++) {
			IloExpr expr6(env);

			for (int i = 0; i < data->getSubstrate()->getN(); i++) {

				if (data->getLocation() && data->getRequest(v)->getGraph()->getDist(k, i) > data->getRequest(v)->getMaxD())
					continue;

				expr6 += z[v][k][i];
			}

			model.add(expr6 - y[v] == 0);
		}
	}

	/* Restrição 3: diferentes nós virtuais de uma mesma rede não serão alocados no mesmo nó físico */
	for (int v = 0; v < data->numberVns(); v++) {
		for (int i = 0; i < data->getSubstrate()->getN(); i++) {
			IloExpr expr7(env);
			bool flag = false;
			for (int k = 0; k < data->getRequest(v)->getGraph()->getN(); k++) {

				if (data->getLocation() && data->getRequest(v)->getGraph()->getDist(k, i) > data->getRequest(v)->getMaxD())
					continue;

				flag = true;
				expr7 += z[v][k][i];
			}

			if (flag)
				model.add(expr7 <= 1);
		}
	}

	/* Restrição 4: CPU */
	for (int i = 0; i < data->getSubstrate()->getN(); i++) {
		IloExpr expr4(env);
		bool flag = false;
		for (int v = 0; v < data->numberVns(); v++) {
			for (int k = 0; k < data->getRequest(v)->getGraph()->getN(); k++) {

				if (data->getLocation() && data->getRequest(v)->getGraph()->getDist(k, i) > data->getRequest(v)->getMaxD())
					continue;
				flag = true;
				expr4 += data->getRequest(v)->getGraph()->getNodes()[k].getCPU() * z[v][k][i];
			}
		}
		if (flag)
			model.add(expr4 - data->getSubstrate()->getNodes()[i].getCPU() <= 0);
	}

	// /* Restrição 6: Banda */
	for (int i = 0; i < data->getSubstrate()->getN(); i++) {
		for (int j = 0; j < data->getSubstrate()->getN(); j++) {

			if (data->getSubstrate()->getAdj(i,j) == -1)
				continue;

			IloExpr expr5(env);
			bool flag = false;
			for (int v = 0; v < data->numberVns(); v++) {
				for (int kl = 0; kl < data->getRequest(v)->getGraph()->getM(); kl++) {
					flag = true;

					expr5 += data->getRequest(v)->getGraph()->getEdges()[kl].getBW() * x[v][kl][i][j];
					expr5 += data->getRequest(v)->getGraph()->getEdges()[kl].getBW() * x[v][kl][j][i];
				}
			}
			if(flag)
				model.add(expr5 - data->getSubstrate()->getEdges()[data->getSubstrate()->getAdj(i, j)].getBW() <= 0);
		}
	}

	/* Restrição 7: Fluxo */
	for (int v = 0; v < data->numberVns(); v++) {
		for (int kl = 0; kl < data->getRequest(v)->getGraph()->getM(); kl++) {
			int k = data->getRequest(v)->getGraph()->getEdges()[kl].getOrig();
			int l = data->getRequest(v)->getGraph()->getEdges()[kl].getDest();

			for (int i = 0; i < data->getSubstrate()->getN(); i++) {

				IloExpr exprA(env), exprB(env);
				IloExpr exprC(env), exprD(env);

				for (int j = 0; j < data->getSubstrate()->getN(); j++) {
					if (data->getSubstrate()->getAdj(i, j) == -1)
						continue;
					exprA += x[v][kl][i][j];
				}

				for (int j = 0; j < data->getSubstrate()->getN(); j++) {
					if (data->getSubstrate()->getAdj(i, j) == -1)
						continue;
					exprB += x[v][kl][j][i];
				}

				IloExpr exprL(env);
				if (!data->getLocation() || data->getRequest(v)->getGraph()->getDist(k, i) <= data->getRequest(v)->getMaxD())
					exprC += z[v][k][i];

				if (!data->getLocation() || data->getRequest(v)->getGraph()->getDist(l, i) <= data->getRequest(v)->getMaxD())
					exprD += z[v][l][i];

				model.add(exprA - exprB == exprC - exprD);
			}
		}
	}

	problem->extract(model);
	problem->exportModel("modeloCompacto.lp");

	try {
		if(!problem->solve()){
			cout << "Infeasible model" << endl;
			return -1;
		}

		double gap = problem->getMIPRelativeGap() * 100.0;

		float contador = 0;
		for (int v = 0; v < data->numberVns(); v++) {
			contador += problem->getIntValue(y[v]);
		}

		double accRatio = contador/data->numberVns();

		for (int v = 0; v < data->numberVns(); v++) {
			for(int k=0; k<data->getRequest(v)->getGraph()->getN(); k++){
				for(int i=0; i<data->getSubstrate()->getN(); i++){
					if (data->getLocation() && data->getRequest(v)->getGraph()->getDist(k, i) > data->getRequest(v)->getMaxD())
						continue;
					problem->out() << z[v][k][i].getName() << " = " << problem->getIntValue(z[v][k][i]) << endl;
				}
			}
		}

		for (int v = 0; v < data->numberVns(); v++) {
			for (int kl = 0; kl < data->getRequest(v)->getGraph()->getM(); kl++) {
				for(int i = 0; i<data->getSubstrate()->getN(); i++){
					for(int j = 0; j<data->getSubstrate()->getN(); j++){
						if(data->getSubstrate()->getAdj(i, j)!=-1){
							problem->out() << x[v][kl][i][j].getName()  << " = " <<  problem->getIntValue(x[v][kl][i][j]) << endl;
						}
					}
				}
			}
		}

		float custoSolucao = problem->getObjValue();

		problem->out() << endl << endl;

		problem->out() << "Solution Cost:    " << custoSolucao << endl;
		problem->out() << "Optm GAP:         " << gap << endl;
		problem->out() << "Acceptance Ratio: " << accRatio << endl;
		//problem->out() << "Time:             " << end_ - init_ << endl;

		delete problem;
		env.end();

	} catch (IloException& e) {
		cerr << "ERROR: " << e.getMessage() << endl;
	} catch (...) {
		cerr << "Error" << endl;
	}

	return 0;
}
