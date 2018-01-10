#include "KnapsackProblem.h"
#include "GC.h"

float KnapsackProblem::Solve(int Capacity, float Weight[], float Value[], int N)
{
	//Set f(k,r) := ∞ for k = 1,..., n and r = 0,..., b. Set f(0, 0) := 0.
	//Set g(k) := ∞ for k = 1,..., n.
	for(int k = 0; k <= N; k++)
	{
		for(int r = 0; r <= Capacity; r++)
		{
			F[k][r] = INFINITY;
			Sel[k][r] = false;
		}
		G[k] = INFINITY;
	}
	F[0][0] = 0;

	//For k = 1,..., n
	//	For r = 0,..., b
	//		If f (k − 1,r) < f (k,r)
	//			Set f (k,r) := f (k − 1,r).
	for(int k = 1; k <= N; k++)
	{
		int weightK = (int)Weight[k - 1];
		double valueK = Value[k - 1];
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
				F[k][r + weightK] = F[k - 1][r] + 1 - valueK;
				Sel[k][r + weightK] = true;
			}
		}

		//For r = b − ak + 1,..., b
		//	If f (k − 1,r) + (1 − x∗k ) < g(k)
		//		Set g(k) := f (k − 1,r) + (1 − x∗k ).
		for(int r = Capacity - weightK + 1; r <= Capacity; r++)
		{
			if(F[k-1][r] + 1 - valueK < G[k])
			{
				G[k] = F[k - 1][r] + 1 - weightK;
				Sel[k - 1][r] = true;
			}
		}
	}

	if(G[N - 1] < 1)
	{
		for(int k = 0; k < N; k++)
		{
			for(int r = 0; r <= Capacity; r++)
			{
				cout << Sel[k][r] << " ";
			}
			cout << endl;
		}

		cout << "There is a CI:" << endl;

		// Retrieve Variables
		int row = N;
		int col = Capacity;
		int cap = Capacity;
		while ( cap > 0 ) {
			if ( Sel[row][col] == 1) {
				cap = cap - Weight[row - 1];
				col = cap;
				cout << "Item " << (row - 1) << endl;
			}             
			row = row - 1;
		}

	}
	return F[N][Capacity];
}