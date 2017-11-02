#ifndef NODE_H
#define NODE_H

class Node {
    
    int id;
    float x, y, cpu;
    
    public:
    Node(int id, float x, float y, float cpu);
    Node();
    int getId();
    float getX();
    float getY();
    float getCPU();

};

#endif