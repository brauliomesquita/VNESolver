#include "KnapsackProblem.h"
#include "GC.h"

float KnapsackProblem::Solve(int Capacity, float Weight[], float Value[], int N)
{
	//Set f(k,r) := ∞ for k = 1,..., n and r = 0,..., b. Set f(0, 0) := 0.
	//Set g(k) := ∞ for k = 1,..., n.
	for(int k = 0; k < N; k++)
	{
		for(int r = 0; r < Capacity; r++)
		{
			F[k][r] = INFINITY;
		}
		G[k] = INFINITY;
	}
	F[0][0] = 0;

	//For k = 1,..., n
	//	For r = 0,..., b
	//		If f (k − 1,r) < f (k,r)
	//			Set f (k,r) := f (k − 1,r).
	for(int k = 1; k < N; k++)
	{
		int weightK = (int)Weight[k];
		for(int r = 0; r <= Capacity; r++)
		{
			if(F[k-1][r] < F[k][r])
			{
				F[k][r] = F[k-1][r];
			}
		}

		//For r = 0,..., b − ak
		//	If f (k − 1,r) + (1 − x∗k ) < f (k,r + ak )
		//		Set f (k,r + ak ) := f (k − 1,r) + (1 − x∗k ).
		for(int r = 0; r <= Capacity - weightK; r++)
		{
			if(F[k-1][r] + 1 - Value[k] < F[k][r + weightK])
			{
				F[k][r + weightK] = F[k - 1][r] + 1 - Value[k];
			}
		}

		//For r = b − ak + 1,..., b
		//	If f (k − 1,r) + (1 − x∗k ) < g(k)
		//		Set g(k) := f (k − 1,r) + (1 − x∗k ).
		for(int r = Capacity - weightK + 1; r <= Capacity; r++)
		{
			if(F[k-1][r] + 1 - Value[k] < G[k])
			{
				G[k] = F[k - 1][r] + 1 - weightK;
			}
		}

		if(G[k] < 1)
		{
			cout << "There is a CI." << endl;

			// Retrieve Variables
		}
	}
	return F[N][Capacity];
}