#ifndef KNAPSACK_H
#define KNAPSACK_H

#include <cmath>

#include "Utility.h"

class KnapsackProblem
{
public:
	float Solve(int Capacity, float Weight[], float Value[], int N);

	float F[1000][1000];
	float G[1000];
};

#endif