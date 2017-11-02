#include "BP.h"


void BP::Solve(ProblemData* data){
	std::vector<GC*> arvore;
	GC * raiz = new GC();
	Branch branch1;
	int y;
	unsigned int saida;
	double bestLB = 0;

	bool * redeAceita = new bool[data->numberVns()];
	for(int v=0; v<data->numberVns(); v++)
		redeAceita[v] = false;
	
	std::vector<Column> solucaoInicial;
	bestLB = 0.0;

	arvore.push_back(raiz);

	const char separator = ' ';
	const int nameWidth = 15;
	double worstUB;
	double tempoExecucao = 0;

	double init, end;

	std::ofstream ofs;
	ofs.open ("outfile", std::ofstream::out);

	ofs << "Output for Branch-and-Price Algorithm  " << endl;
	time_t now = time(0);
	tm *ltm = localtime(&now);
   // print various components of tm structure.
	ofs << ""<< 1 + ltm->tm_mon;
	ofs << "/"<<  ltm->tm_mday;
	ofs << "/" << 1900 + ltm->tm_year;
	ofs << " "<< ltm->tm_hour << ":";
	ofs << ltm->tm_min << ":";
	ofs << ltm->tm_sec << endl;


	ofs << "Substrate Size:       " << data->getSubstrate()->getN() << endl;
	ofs << "Number os VNs:        " << data->numberVns() << endl;
	ofs << "Parameters: " << endl;
	if(data->getLocation())
		ofs << "Location" << endl;
	if(data->getDelay())
		ofs << "Delay" << endl;
	if(data->getResilience()){
		ofs << "Resilience" << endl;
	}

	ofs << "Objective Function:" << endl;
	ofs << "Maximize Profit" << endl;
	ofs << endl << endl;

	ofs << "Initial Solution Cost:\t" << bestLB << endl << endl;

	ofs << "Start..." << endl;
	ofs << left << setw(nameWidth) << setfill(separator) << "ID";
	ofs << left << setw(nameWidth) << setfill(separator) << "Node Obj";
	ofs << left << setw(nameWidth) << setfill(separator) << "Node Int";
	ofs << left << setw(nameWidth) << setfill(separator) << "Best Integer";
	ofs << left << setw(nameWidth) << setfill(separator) << "Max UB";
	ofs << left << setw(nameWidth) << setfill(separator) << "GAP (%)";
	ofs << left << setw(nameWidth) << setfill(separator) << "# Cols";
	ofs << left << setw(nameWidth) << setfill(separator) << "# Gen. Cols";
	ofs << left << setw(nameWidth) << setfill(separator) << "Relax Time";
	ofs << left << setw(nameWidth) << setfill(separator) << "Master Time";
	ofs << left << setw(nameWidth) << setfill(separator) << "Sub Time";
	ofs << left << setw(nameWidth) << setfill(separator) << "Total Time";
	ofs << endl;

	while(arvore.size() != 0){
		if(tempoExecucao >= 3600)
			break;

		int best_cost = -INFINITY;
		int best_index = -1;
		for(int s=0; s<arvore.size(); s++){
			if(arvore[s]->parentUB > best_cost){
				best_cost = arvore[s]->parentUB;
				best_index = s;
			}
		}
		
		//best_index = arvore.size() - 1;	// Depth-First Search
		//best_index = 0;					// Breadth-First Search
		GC * gc = arvore[best_index];
		arvore.erase(arvore.begin() + best_index);

		if(gc->parentUB > bestLB + 0.0001){
			cout << "Número de colunas: " << gc->parentPool.size() << endl;

			init = get_time();
			gc->Solve(data->getSubstrate(), data->requests_, data->getLocation(), data->getDelay(), data->getResilience(), &y, &branch1, &saida);
			end = get_time();
			tempoExecucao += gc->tempoTotal;

			cout << endl << "Número total de colunas: " << gc->pool.size() << endl;

			cout << "Custo da relaxação: " << gc->ub << endl;
			cout << "Custo da Heuristica primal: " << gc->lb << endl;
			cout << "Tempo Relaxação Raiz: " << gc->tempoTotal << endl;

			if(gc->lb > bestLB){
				bestLB = gc->lb;
			}

			if(gc->ub > bestLB){
				for(int i=0; i<=1; i++){
					GC * filho = new GC(gc);
					filho->addBranch(branch1, i);
					filho->id = gc->id * 2 + i;
					arvore.push_back(filho);
				}
			}

			worstUB = bestLB;
			for(int s=0; s<arvore.size(); s++){
				if(arvore[s]->parentUB > worstUB){
					worstUB = arvore[s]->parentUB;
				}
			}

			ofs << fixed << setprecision(4);
			ofs << left << setw(nameWidth) << setfill(separator) << gc->id;
			ofs << left << setw(nameWidth) << setfill(separator) << gc->ub;
			ofs << left << setw(nameWidth) << setfill(separator) << gc->lb;
			ofs << left << setw(nameWidth) << setfill(separator) << bestLB;
			ofs << left << setw(nameWidth) << setfill(separator) << worstUB;
			ofs << left << setw(nameWidth) << setfill(separator) << 100*(1 - bestLB/worstUB);
			ofs << left << setw(nameWidth) << setfill(separator) << gc->nCols;
			ofs << left << setw(nameWidth) << setfill(separator) << gc->gCols;
			ofs << left << setw(nameWidth) << setfill(separator) << gc->tempoRelaxacao;
			ofs << left << setw(nameWidth) << setfill(separator) << gc->tempoMaster;
			ofs << left << setw(nameWidth) << setfill(separator) << gc->tempoSub;
			ofs << left << setw(nameWidth) << setfill(separator) << gc->tempoTotal;
			ofs << left << setw(nameWidth) << setfill(separator) << (end - init);
			ofs << endl;
		}

	//	delete gc;
	//	break;
	}

	worstUB = bestLB;
	for(int s=0; s<arvore.size(); s++){
		if(arvore[s]->parentUB > worstUB){
			worstUB = arvore[s]->parentUB;
		}
	}

	ofs << "Best Integer: " << bestLB << endl;
	ofs << "Lower Bound: " << worstUB << endl;
	ofs << "GAP: " << 100*(1 - bestLB/worstUB) << endl;
	ofs << "Time: " << tempoExecucao << endl;

	ofs << "FINISHED!";

	cout << "Best Solution: " << bestLB << endl;

	ofs.close();
}
