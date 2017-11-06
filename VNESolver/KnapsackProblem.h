#ifndef KNAPSACK_H
#define KNAPSACK_H

#include <cmath>

#include "Utility.h"

class KnapsackProblem
{
public:
	KnapsackProblem();
	~KnapsackProblem();

	float Solve(int Capacity, float Weight[], float Value[], int N);

	float K[1000][1000];
};

#endif