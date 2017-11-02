#include "Node.h"
    
Node::Node(int id, float x, float y, float cpu){
    this->id = id;
    this->x = x;
    this->y = y;
    this->cpu = cpu;
}

Node::Node(){
}

int Node::getId(){
    return this->id;
}

float Node::getX(){
    return this->x;
}

float Node::getY(){
    return this->y;
}

float Node::getCPU(){
    return this->cpu;
}
