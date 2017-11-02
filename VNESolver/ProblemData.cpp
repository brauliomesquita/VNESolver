#include "ProblemData.h"
#include <cassert>

ProblemData::ProblemData()
{
	this->substrate_ = nullptr;
	this->requests_ = std::vector<Request*>();

	this->location_ = false;
	this->delay_ = false;
	this->resilience_ = false;

	this->fo_ = 1;
	this->time_limit_ = 3600;
}

ProblemData::~ProblemData()
{
	delete substrate_;

	for(int v = 0; v < requests_.size(); v++){
		delete requests_[v];
	}
	requests_.clear();
}

bool ProblemData::ReadInputData(char* argv[])
{
	if(!this->ReadSubstrate(argv[1]))
		return false;
	
	if(!this->ReadVNsFolder(argv[2], atoi(argv[3])))
		return false;
	
	this->getSubstrate()->setDist(this->getSubstrate());
	
	for(int v=0; v<numberVns(); v++)
		this->getRequest(v)->getGraph()->setDist(this->getSubstrate());

	return true;
}

bool ProblemData::ReadSubstrate(char * subGraph) {
	FILE * arquivo = fopen(subGraph, "r");

	if (!arquivo){
		cout << "Erro ao abrir arquivo!" << endl;
		return false;
	}

	int n, m, k, l;
	int x, y;
	double cpu, banda, atraso;

	fscanf(arquivo, "%d %d", &n, &m);

	substrate_ = new Graph(n, m);

	for (int i = 0; i < substrate_->getN(); i++) {
		fscanf(arquivo, "%d %d %lf", &x, &y, &cpu);

		substrate_->addNode(Node(i, x, y, cpu));
	}

	for (int i = 0; i < substrate_->getM(); i++) {
		fscanf(arquivo, "%d %d %lf %lf", &k, &l, &banda, &atraso);

		substrate_->addEdge(Edge(i, k, l, banda, atraso));
	}

	return true;
}

bool ProblemData::ReadVNsFolder(char * folder, int numberVNs) {

	char file[512];

	int n, m, split, chegada, duracao, topologia, raio;

	this->number_vns_ = numberVNs;

	for (int v = 0; v < numberVNs; v++) {
		sprintf(file, "%sreq%d.txt", folder, v);

		FILE * arquivo = fopen(file, "r");

		if(!arquivo){
			cout << "Erro ao abrir arquivo '" << file << "'!" << endl;
			return false;
		}

		fscanf(arquivo, "%d %d %d %d %d %d %d", &n, &m, &split, &chegada,
			&duracao, &topologia, &raio);

		Request * r = new Request(v, chegada, duracao, raio);

		Graph * g = new Graph(n, m);

		int k, l;
		int x, y;
		double cpu;
		double banda, atraso;
		double profit = 0;

		for (int i = 0; i < g->getN(); i++) {
			fscanf(arquivo, "%d %d %lf", &x, &y, &cpu);

			g->addNode(Node(i, x, y, cpu));
			profit += cpu;
		}

		for (int i = 0; i < g->getM(); i++) {
			fscanf(arquivo, "%d %d %lf %lf", &k, &l, &banda, &atraso);

			g->addEdge(Edge(i, k, l, banda, atraso));
			profit += banda;
		}

		r->setGraph(g);
		r->setProfit(profit);

		requests_.push_back(r);

		fclose(arquivo);
	}

	return true;
}

Graph* ProblemData::getSubstrate()
{
	return this->substrate_;
}

Request* ProblemData::getRequest(int i)
{
	assert(i >= 0 && i < this->requests_.size());

	return this->requests_[i];
}

int ProblemData::numberVns()
{
	return this->number_vns_;
}
