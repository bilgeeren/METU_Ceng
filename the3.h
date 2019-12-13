#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Test
{
public:
	void calculateScores(int n, int**& edgeList, double*& scores);
	int calculateDisconnectedGraphs(int n, int**& edgeList);
	int Important (int n, int**& edgeList, double*& scores);

};
