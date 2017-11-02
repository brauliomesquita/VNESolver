#ifndef MAPLINK_H
#define MAPLINK_H

#include "Edge.h"

class MappingLink {
	Edge phys;
	float BW;

	public:
		MappingLink(Edge l);
		void setPhysEdge(Edge p);
		Edge getPhysEdge();
		float getBW();
		void setBW(float bw);
};

#endif /* GC_H */

