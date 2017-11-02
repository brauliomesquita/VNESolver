#include "Column.h"

Column::Column(int v, int kl){
	this->v = v;
	this->kl = kl;

	this->k = this->l = -1;
	this->lowerBound = 0;
	this->upperBound = 1;
	this->custoFO = 0;
}

std::vector<Edge> Column::getEdges(){
	return this->edges;
}

void Column::addEdge(Edge e){
	this->edges.push_back(e);
}