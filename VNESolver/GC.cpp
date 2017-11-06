#include "GC.h"

GC::GC(ProblemData * data):	tempoMaster(0), tempoSub(0), tempoTotal(0), tempoRelaxacao(0),
							ub(INFINITY), lb(-INFINITY), parentUB(INFINITY),
							id(0), sol_inteira(false), nCols(0), gCols(0)
{
	this->data = data;

	pool = std::vector<Column>();
	parentPool = std::vector<Column>();
	forbidden = std::vector<Column>();
	branchs = std::vector<Branch>();

	for(int v=0; v<data->numberVns(); v++)
	{
		for(int kl = 0; kl < data->getRequest(v)->getGraph()->getM(); kl++){
			Column * fakeColumn = new Column(v, kl);
			fakeColumn->custoFO = 5000;
			fakeColumn->l = -1;
			fakeColumn->k = -1;

			parentPool.push_back(*fakeColumn);
		}
	}

}

GC::GC(GC * parent):	tempoMaster(0), tempoSub(0), tempoTotal(0), tempoRelaxacao(0),
						ub(INFINITY), lb(-INFINITY), id(-1)
{
	this->data = parent->data;
	
	this->pool = std::vector<Column>();
	this->parentPool = std::vector<Column>(parent->pool);
	this->forbidden = std::vector<Column>(parent->forbidden);
	this->branchs = std::vector<Branch>(parent->branchs);
	
	this->parentUB = parent->ub;
}

GC::~GC(){
	delete pricing;
	delete master;
	env.end();
}

bool GC::CoverCut(IloRangeArray constraints)
{
	// For each constraint
	for(int c = 0; c < constraints.getSize(); c++)
	{
		int num_vars = 0;

		// Get the variables in the constraint
		for (IloExpr::LinearIterator it = constraints[c].getLinearIterator(); it.ok();++it)
		{
			Weights[num_vars] = it.getCoef();
			Values[num_vars] = master->getValue(it.getVar());
			++num_vars;
		}

		if(!num_vars)
			continue;

		float rhs = constraints[c].getUB();

		// Solve Knapsack problem
		IloEnv env2;
		IloModel model2(env2);
		IloCplex cplex2(env2);
		IloObjective objective2(env2);

		cplex2.setParam(IloCplex::MIPDisplay, 0);
		cplex2.setParam(IloCplex::SimDisplay, 0);
		cplex2.setParam(IloCplex::SiftDisplay, 0);

		IloIntVarArray x(env2, num_vars);
		for(int i=0; i<num_vars; i++)
		{
			char varname[32];
			sprintf(varname, "x_%d", i);
			x[i] = IloIntVar(env2, 0, 1, varname);
		}
		model2.add(x);

		IloExpr obj2(env2), expr2(env2);
		for(int i=0; i<num_vars; i++)
		{
			obj2 += (1 - Values[i]) *  x[i];
			expr2 += Weights[i] * x[i];
		}

		model2.add(IloMinimize(env2, obj2));
		model2.add(expr2 >= rhs + 0.001);

		cplex2.extract(model2);
		//cplex2.exportModel("knapsack_model.lp");

		if(!cplex2.solve())
			continue;

		if(cplex2.getObjValue() < 1)
		{
			IloExpr ccut(env);
			int count = 0, contador = 0;
			for (IloExpr::LinearIterator it = constraints[c].getLinearIterator(); it.ok(); ++it)
			{
				if(cplex2.getIntValue(x[count]) == 1)
				{
					ccut += it.getVar();
					contador++;
				}
				++count;;
			}

			cover_cuts_.add(ccut <= contador - 1);
		}
	}

	return false;
}

void GC::GenerateCoverCuts(){
	
	if(master->getStatus() != IloAlgorithm::Optimal)
		return;

	CoverCut(constraint_bw);
	CoverCut(constraint_cpu);

	model.add(cover_cuts_);

	master->extract(model);
	master->exportModel("master_cover_cuts.lp");
}

void GC::addBranchLambda(int m, int valor){

	this->parentPool[m].lowerBound = this->parentPool[m].upperBound = valor;

	if(valor == 0){
		this->forbidden.push_back(parentPool[m]);
	}
	
}

void GC::addBranch(Branch branch, int valor){
	branch.valor = valor;
	this->branchs.push_back(branch);

	// Se branch_tipo == 2, eliminar colunas
	if(branch.tipo_branch == 2){
		int v = branch.v;
		int kl = branch.x;
		int ij = branch.y;

		for(int p=0; p<parentPool.size(); p++){
			
			if(parentPool[p].v != v || parentPool[p].kl != kl){
				continue;
			}

			bool flag = false;
			std::vector<Edge> edges = parentPool[p].getEdges();
			for(int e=0; e<edges.size(); e++){
				if(ij == edges[e].getId()){
					flag = true;
				}
			}
			
			if(flag != valor){
				parentPool[p].lowerBound = parentPool[p].upperBound = 0;
			}
						
		}

	}

}

void GC::SetCplexParameters() {
	master->setParam(IloCplex::TiLim, 3600.0);

	/*master->setParam(IloCplex::PreInd, 0);
	master->setParam(IloCplex::AggInd, 0);
	master->setParam(IloCplex::HeurFreq, -1);

	master->setParam(IloCplex::FracCuts, -1);
	master->setParam(IloCplex::LiftProjCuts, -1);
	master->setParam(IloCplex::FlowCovers, -1);
	master->setParam(IloCplex::GUBCovers, -1);
	master->setParam(IloCplex::Covers, -1);

	master->setParam(IloCplex::ZeroHalfCuts, -1);
	master->setParam(IloCplex::ImplBd, -1);
	master->setParam(IloCplex::Cliques, -1);
	master->setParam(IloCplex::DisjCuts, -1);
	master->setParam(IloCplex::FlowPaths, -1);
	master->setParam(IloCplex::MIRCuts, -1);
*/
	master->setParam(IloCplex::MIPDisplay, 0);
	master->setParam(IloCplex::SimDisplay, 0);
	master->setParam(IloCplex::SiftDisplay, 0);

	//master->setParam(IloCplex::Threads, 1);
}

void GC::BuildModel()
{
	this->CreateVariables();
	this->CreateObjectiveFunction();
	this->CreateConstraints();
}

void GC::CreateVariables(){
	y = IloNumVarArray(env, data->numberVns());
	lambda = IloNumVarArray(env);

	char var_name[256];

	for (int v = 0; v < data->numberVns(); v++) {
		sprintf(var_name, "y_%d", v);
		y[v] = IloNumVar(env, 0, 1, var_name);
		model.add(y[v]);
	}

	z = NumVar3Matrix(env, data->numberVns());
	for (int v = 0; v < data->numberVns(); v++) {
		z[v] = NumVarMatrix(env, data->getRequest(v)->getGraph()->getN());
		for (int k = 0; k < data->getRequest(v)->getGraph()->getN(); k++) {
			z[v][k] = IloNumVarArray(env, data->getSubstrate()->getN());
			for (int i = 0; i < data->getSubstrate()->getN(); i++) {

				if (data->getLocation() && data->getRequest(v)->getGraph()->getDist(k, i) > data->getRequest(v)->getMaxD())
					continue;

				sprintf(var_name, "z_%d_%d_%d", v, k, i);
				z[v][k][i] = IloNumVar(env, 0, 1, var_name);
				model.add(z[v][k][i]);
			}
		}
	}
}

void GC::CreateObjectiveFunction() {

	objective = IloAdd(model, IloMinimize(env));

	IloExpr obj(env);

	for (int v = 0; v < data->numberVns(); v++) {
//		obj += data->getRequest(v)->getProfit() * y[v];
		obj += 10000 * (1 - y[v]);
	}

	objective.setExpr(obj);
	obj.end();

}

void GC::CreateConstraints() {

	for (int v = 0; v < data->numberVns(); v++) {
		for (int k = 0; k < data->getRequest(v)->getGraph()->getN(); k++) {
			IloExpr expr6(env);

			for (int i = 0; i < data->getSubstrate()->getN(); i++) {

				if (data->getLocation() && data->getRequest(v)->getGraph()->getDist(k, i) > data->getRequest(v)->getMaxD())
					continue;

				expr6 += z[v][k][i];
			}

			model.add(expr6 == y[v]);
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
	constraint_cpu = OneDimRange(env, data->getSubstrate()->getN());
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
			constraint_cpu[i] = expr4 <= data->getSubstrate()->getNodes()[i].getCPU();
	}
	model.add(constraint_cpu);

	char cName[256];

	constraint_lambda = TwoDimRange(env, data->numberVns());
	for (int v = 0; v < data->numberVns(); v++) {
		constraint_lambda[v] = OneDimRange(env, data->getRequest(v)->getGraph()->getM());
		for(int kl=0; kl<data->getRequest(v)->getGraph()->getM(); kl++){
			sprintf(cName, "constLambda_%d_%d", v, kl);
			constraint_lambda[v][kl] = -y[v] == 0;
			constraint_lambda[v][kl].setName(cName);
		}
		model.add(constraint_lambda[v]);
	}
	
	// Bandwidth constraints
	constraint_bw = OneDimRange(env, data->getSubstrate()->getM());
	for (int i = 0; i < data->getSubstrate()->getM(); i++) {
		for (int j = i; j < data->getSubstrate()->getN(); j++) {
			if(data->getSubstrate()->getAdj(i,j) != -1){
				IloExpr expr_band(env);
				constraint_bw[data->getSubstrate()->getAdj(i,j)] = expr_band <= data->getSubstrate()->getEdges()[data->getSubstrate()->getAdj(i,j)].getBW();
				sprintf(cName, "constBW_%d_%d", i, j);
				constraint_bw[data->getSubstrate()->getAdj(i,j)].setName(cName);
			}
		}
	}
	model.add(constraint_bw);

		constraint_saida = ThreeDimRange(env, data->numberVns());
	for (int v = 0; v < data->numberVns(); v++) {
		constraint_saida[v] = TwoDimRange(env, data->getRequest(v)->getGraph()->getM());
		for(int kl=0; kl<data->getRequest(v)->getGraph()->getM(); kl++){
			constraint_saida[v][kl] = OneDimRange(env, data->getSubstrate()->getN());
			int k = data->getRequest(v)->getGraph()->getEdges()[kl].getOrig();
			for(int i = 0; i < data->getSubstrate()->getN(); i++){			
				if(!data->getLocation() || data->getRequest(v)->getGraph()->getDist(k, i) <= data->getRequest(v)->getMaxD()){
					sprintf(cName, "constSaida_%d_%d_%d", v, kl, i);

					constraint_saida[v][kl][i] = -z[v][k][i] <= 0;
					model.add(constraint_saida[v][kl][i]);
					constraint_saida[v][kl][i].setName(cName);
				}
			}
		}
	}

	constraint_entrada = ThreeDimRange(env, data->numberVns());
	for (int v = 0; v < data->numberVns(); v++) {
		constraint_entrada[v] = TwoDimRange(env, data->getRequest(v)->getGraph()->getM());
		for(int kl=0; kl<data->getRequest(v)->getGraph()->getM(); kl++){
			constraint_entrada[v][kl] = OneDimRange(env, data->getSubstrate()->getN());
			int l = data->getRequest(v)->getGraph()->getEdges()[kl].getDest();
			for(int i = 0; i < data->getSubstrate()->getN(); i++){				
				if(!data->getLocation() || data->getRequest(v)->getGraph()->getDist(l, i) <= data->getRequest(v)->getMaxD()){
					sprintf(cName, "constEnt_%d_%d_%d", v, kl, i);
					constraint_entrada[v][kl][i] = -z[v][l][i] <= 0;
					model.add(constraint_entrada[v][kl][i]);
					constraint_entrada[v][kl][i].setName(cName);
				}
			}
		}
	}

	cover_cuts_ = OneDimRange(env);
}

void GC::AddColumns(std::vector<Column> colunas){

	for(int m=0; m < colunas.size(); m++){
		if(colunas[m].lowerBound + colunas[m].upperBound == 0)
			continue;

		int v = colunas[m].v;
		int kl = colunas[m].kl;

		char buffer[30];
		if(colunas[m].custoFO == 5000)
		{
			sprintf(buffer, "lambda_%d_%d_ART", v, kl);
		} else {
			sprintf(buffer, "lambda_%d_%d_%d", v, kl, lambda.getSize());
		}

		IloNumVar new_variable;

		new_variable = IloNumVar(env, colunas[m].lowerBound, colunas[m].upperBound, buffer);
		lambda.add(new_variable);
		model.add(new_variable);

		//if(colunas[m].custoFO == 5000)
		objective.setLinearCoef(new_variable, colunas[m].custoFO);

		constraint_lambda[v][kl].setLinearCoef(new_variable, 1);

		std::vector<Edge> edges = colunas[m].getEdges();
		for(int e=0; e<edges.size(); e++){
			int ij = edges[e].getId();

			constraint_bw[ij].setLinearCoef(new_variable, data->getRequest(v)->getGraph()->getEdges()[kl].getBW());
		}

		if(colunas[m].k != -1 ){
			constraint_saida[v][kl][colunas[m].k].setLinearCoef(new_variable, 1);
		}
		if(colunas[m].l != -1){
			constraint_entrada[v][kl][colunas[m].l].setLinearCoef(new_variable, 1);
		}

		this->pool.push_back(colunas[m]);

		nCols++;
	}

}

void GC::getDuals(IloNumArray2 * gamma, IloNumArray3 * alpha, IloNumArray3 * pi, IloNumArray * beta){

	for (int v = 0; v < data->numberVns(); v++) {
		for(int kl=0; kl<data->getRequest(v)->getGraph()->getM(); kl++){
			(*gamma)[v][kl] = master->getDual(constraint_lambda[v][kl]);
		}
	}
	
	// Bandwidth constraints
	for (int i = 0; i < data->getSubstrate()->getM(); i++) {
		for (int j = i; j < data->getSubstrate()->getN(); j++) {
			if(data->getSubstrate()->getAdj(i,j) != -1){
				(*beta)[data->getSubstrate()->getAdj(i,j)] = master->getDual(constraint_bw[data->getSubstrate()->getAdj(i,j)]);
			}
		}
	}

	for (int v = 0; v < data->numberVns(); v++) {
		for(int kl=0; kl<data->getRequest(v)->getGraph()->getM(); kl++){
			int k = data->getRequest(v)->getGraph()->getEdges()[kl].getOrig();
			//int l = data->getRequest(v)->getGraph()->getEdges()[kl].getDest();
			for(int i = 0; i < data->getSubstrate()->getN(); i++){				
				if(!data->getLocation() || data->getRequest(v)->getGraph()->getDist(k, i) <= data->getRequest(v)->getMaxD()){
					(*alpha)[v][kl][i] = master->getDual(constraint_saida[v][kl][i]);
				}
			}
		}
	}

	for (int v = 0; v < data->numberVns(); v++) {
		for(int kl=0; kl<data->getRequest(v)->getGraph()->getM(); kl++){
			int l = data->getRequest(v)->getGraph()->getEdges()[kl].getDest();
			for(int i = 0; i < data->getSubstrate()->getN(); i++){				
				if(!data->getLocation() || data->getRequest(v)->getGraph()->getDist(l, i) <= data->getRequest(v)->getMaxD()){
					(*pi)[v][kl][i] = master->getDual(constraint_entrada[v][kl][i]);
				}
			}
		}
	}

}

float GC::getGAP(){
	return 100*(1 - lb/ub);
}

int GC::Solve(Branch * branch){
	env = IloEnv();
	model = IloModel(env);
	model.setName("Master Problem - Column Generation - Path Generation");

	master = new IloCplex(model);

	IloNumArray3 alpha = IloNumArray3(env, data->numberVns());
	IloNumArray3 pi = IloNumArray3(env, data->numberVns());
	IloNumArray2 gamma = IloNumArray2(env, data->numberVns());

	for(int v=0; v<data->numberVns(); v++){
		alpha[v] = IloNumArray2(env, data->getRequest(v)->getGraph()->getM());
		pi[v] = IloNumArray2(env, data->getRequest(v)->getGraph()->getM());

		gamma[v] = IloNumArray(env, data->getRequest(v)->getGraph()->getM());

		for(int kl=0; kl<data->getRequest(v)->getGraph()->getM(); kl++){
			alpha[v][kl] = IloNumArray(env, data->getSubstrate()->getN());
			pi[v][kl] = IloNumArray(env, data->getSubstrate()->getN());
		}
	}

	IloNumArray beta = IloNumArray(env, data->getSubstrate()->getM());

	BuildModel();

	SetCplexParameters();

	pricing = new Pricing();
	tempoSub = tempoMaster = 0.0;
	float init, end;

	for(int b=0; b<branchs.size(); b++){
		if(branchs[b].tipo_branch == 1){
			int v = branchs[b].v;
			int k = branchs[b].x;
			int i = branchs[b].y;
			int valor = branchs[b].valor;

			z[v][k][i].setBounds(valor, valor);
		}
		
		if(branchs[b].tipo_branch == 3){
			y[branchs[b].v].setBounds(branchs[b].valor, branchs[b].valor);
		}
	}

	std::vector<Column> colunas = std::vector<Column>();
	AddColumns(parentPool);
	gCols = nCols;
	
	std::ofstream ofs;
	ofs.open ("genCols", std::ofstream::out);

	while(true){
		init = get_time();
		if(!master->solve()){
			return 0;
		}
		end =  get_time();
		tempoMaster += end - init;

		getDuals(&gamma, &alpha, &pi, &beta);

		init = get_time();
		pricing->Solve(data->getSubstrate(), data->requests_, data->getLocation(), data->getDelay(), data->getResilience(), gamma, alpha, pi, beta, &colunas, forbidden, branchs);
		end =  get_time();
		tempoSub += end - init;

		// Loga Colunas Geradas
		for(int m=0; m < colunas.size(); m++){
			int v = colunas[m].v;
			int kl = colunas[m].kl;

			ofs << "lambda_" << v << "_" << kl << "_" << lambda.getSize() + m << "\t";	
			ofs << "Start: " << colunas[m].k << "\tEnd: " << colunas[m].l << "\t";
			ofs << "Physical Edges: ";
			std::vector<Edge> edges = colunas[m].getEdges();
			for(int e=0; e<edges.size(); e++){
				int ij = edges[e].getId();

				ofs << ij << " ";
			}

			ofs << endl;
		}

		if(colunas.size() == 0)
			break;

		AddColumns(colunas);

		//master->solve();
		//GenerateCoverCuts();

		colunas.clear();
	}
	gCols = nCols - gCols;

	master->exportModel("master.lp");
	
	cout << "Obj Value: " << master->getObjValue() << endl;

#if _DEBUG

	for (int v = 0; v < data->numberVns(); v++){
		if(master->getValue(y[v]) > 0.0001)
			cout << y[v].getName() << "\t" << master->getValue(y[v]) << endl;
	}

	for (int v = 0; v < data->numberVns(); v++) {
		for (int k = 0; k < data->getRequest(v)->getGraph()->getN(); k++) {
			for (int i = 0; i < data->getSubstrate()->getN(); i++) {
				if (data->getLocation() && data->getRequest(v)->getGraph()->getDist(k, i) > data->getRequest(v)->getMaxD())
					continue;
				if(master->getValue(z[v][k][i]) > 0.0001)
					cout << z[v][k][i].getName() << "\t" << master->getValue(z[v][k][i]) << endl;
			}
		}
	}

	for (int m = 0; m < lambda.getSize(); m++) {
		if(master->getValue(lambda[m]) > 0.0001)
			cout << lambda[m].getName() << "\t" << master->getValue(lambda[m]) << endl;
	}

#endif

	master->solve();

	cout << "Obj Value: " << master->getObjValue() << endl;

	this->lb = master->getObjValue();

	tempoRelaxacao = tempoMaster + tempoSub;
	tempoTotal = tempoRelaxacao;

	// Artificial Variable Used
	for (int m = 0; m < lambda.getSize(); m++) {
		float value = master->getValue(lambda[m]);
		if(value >= 0.001 && pool[m].custoFO == -5000){
			return 0;
		}
	}

	branch->v = branch->x = branch->y = branch->valor = -1;

	sol_inteira = true;
	float mais_frac = 0.001;

	for (int v = 0; v < data->numberVns(); v++){
		float value = master->getValue(y[v]);
		if(abs(value - round(value)) > mais_frac){
			mais_frac = abs(value - round(value));
			sol_inteira = false;
			branch->v = v;
			branch->tipo_branch = 3;
		}
	}

	if(!sol_inteira)
	{
		return 1;
	}

	for (int v = 0; v < data->numberVns(); v++) {
		for (int k = 0; k < data->getRequest(v)->getGraph()->getN(); k++) {
			for (int i = 0; i < data->getSubstrate()->getN(); i++) {
				if (data->getLocation() && data->getRequest(v)->getGraph()->getDist(k, i) > data->getRequest(v)->getMaxD())
					continue;
				float value = master->getValue(z[v][k][i]);
				
				if(abs(value - round(value))>mais_frac){
					mais_frac = abs(value - round(value));
					sol_inteira = false;

					branch->v = v;
					branch->x = k;
					branch->y = i;
					branch->tipo_branch = 1;
				}
			}
		}
	}

	if(!sol_inteira)
	{
		return 1;
	}

	// Existe lambda fracionário?

	float *** uso = new float**[data->numberVns()];
	for (int v = 0; v < data->numberVns(); v++) {
		uso[v] = new float*[data->getRequest(v)->getGraph()->getM()];
		for (int kl = 0; kl < data->getRequest(v)->getGraph()->getM(); kl++) {
			uso[v][kl] = new float[data->getSubstrate()->getM()];
			for(int ij=0; ij<data->getSubstrate()->getM(); ij++){
				uso[v][kl][ij] = 0.0;
			}
		}
	}

	for (int m = 0; m < lambda.getSize(); m++) {
		float value = master->getValue(lambda[m]);
		
		value = abs(value - round(value));
			
		float valorLambda = master->getValue(lambda[m]);
			
		if(value >= 0.001)
			sol_inteira = false;

		int v = pool[m].v;
		int kl = pool[m].kl;

		std::vector<Edge> edges = pool[m].getEdges();
		for(int e=0; e<edges.size(); e++){
			int ij = edges[e].getId();
				
			uso[v][kl][ij] += valorLambda;
		}
	}

	float maior_ = 0.0;
	for (int v = 0; v < data->numberVns(); v++) {
		for (int kl = 0; kl < data->getRequest(v)->getGraph()->getM(); kl++) {
			for(int ij=0; ij<data->getSubstrate()->getM(); ij++){

				if(abs(uso[v][kl][ij] - round(uso[v][kl][ij])) > maior_){
					maior_ = abs(uso[v][kl][ij] - round(uso[v][kl][ij]));

					branch->v = v;
					branch->x = kl;
					branch->y = ij;
					branch->tipo_branch = 2;
				}
			}
		}
	}

	if(!sol_inteira){
		return 1;
	}

	this->ub = this->lb;

	return 2;
}
