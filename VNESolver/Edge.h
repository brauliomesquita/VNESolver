#ifndef EDGE_H
#define EDGE_H

class Edge {
    int id, orig, dest;
    float bw, delay;

  public:

  	Edge();
	Edge (int id, int o, int d, float b, float del);
     int getId();    
     int getOrig();    
     int getDest();    
     float getBW();    
     float getDelay();
};

#endif