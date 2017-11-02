#include "Graph.h"

Graph::Graph(int n, int m) {
    this->n = n;
    this->m = m;
    
    this->adj = new int*[n];
    for (int i = 0; i < n; i++) {
        this->adj[i] = new int[n];
        for (int j = 0; j < n; j++) {
            this->adj[i][j] = -1;
        }
    }

    this->nodes = std::vector<Node>();
    this->edges = std::vector<Edge>();

}

Graph::~Graph() {
    for (int i = 0; i < this->n; i++) {
        delete [] this->adj[i];
        delete [] this->dist[i];
    }
    delete [] this->adj;
    delete [] this->dist;
}

void Graph::addNode(Node node) {
    this->nodes.push_back(node);
}

void Graph::addEdge(Edge edge) {
    this->edges.push_back(edge);

    this->adj[edge.getOrig()][edge.getDest()] = edge.getId();
    this->adj[edge.getDest()][edge.getOrig()] = edge.getId();
}

int Graph::getN() {
    return this->n;
}

int Graph::getM() {
    return this->m;
}

int Graph::getAdj(int i, int j) {
    return this->adj[i][j];
}

void Graph::setDist(Graph *sub) {

    this->dist = new float*[this->n];
    for (int i = 0; i < this->n; i++) {
        this->dist[i] = new float[sub->getN()];
        for (int j = 0; j < sub->getN(); j++) {
            this->dist[i][j] = sqrt(pow(this->getNodes()[i].getX() - sub->getNodes()[j].getX(), 2)
                + pow(this->getNodes()[i].getY() - sub->getNodes()[j].getY(), 2));
        }
    }
}

float Graph::getDist(int i, int j) {
    return this->dist[i][j];
}

float Graph::getCost(int m) {

    int orig = this->getEdges()[m].getOrig();
    int dest = this->getEdges()[m].getDest();
/*
    float x1 = this->getNodes()[orig].getX();
    float y1 = this->getNodes()[orig].getY();

    float x2 = this->getNodes()[dest].getX();
    float y2 = this->getNodes()[dest].getY();

    float cost = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
*/
    return dist[orig][dest];
}

std::vector<Node> Graph::getNodes() {
    return this->nodes;
}

std::vector<Edge> Graph::getEdges() {
    return this->edges;
}

