#include "KnapsackProblem.h"

KnapsackProblem::KnapsackProblem(void)
{

}


KnapsackProblem::~KnapsackProblem(void)
{

}

float KnapsackProblem::Solve(int Capacity, float Weight[], float Value[], int N)
{
	int i, w;

	for (i = 0; i <= N; i++){
		for (w = 0; w <= Capacity; w++){
			if (i == 0 || w == 0){
				K[i][w] = 0;
			} else if (Weight[i-1] <= w) {
				K[i][w] = max(Value[i-1] + K[i-1][(int)floor(w - Weight[i-1])], K[i-1][w]);
			} else {
				K[i][w] = K[i-1][w];
			}
		}
	}

	return K[N][Capacity];
}