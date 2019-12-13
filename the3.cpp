#include "the3.h"
#include <vector>
#include <iostream>

// You  can define extra functions here



// INPUT :
//            n         : number of nodes in the graph
//            edgeList  : edges in the graph
//            scores    : importance scores
// return value : 
//                 number of disconnected components
using namespace std;

void Test::calculateScores(int n, int**& edgeList, double*& scores) {

	for(int i = 0; i<n; i++){
		scores[i] = 0.0f;
	}
	for(int v = 0; v < n; v++) {
		for(int s = 0; s < n; s++){
			for(int t = 0; t <n ; t++) {
				if( s == t ) {
					scores[v] += (double(edgeList[s][v]+edgeList[v][t])/double(1));
				}
				else if( s == v or t == v or edgeList[s][t] == 0 ) {
					continue;
				}
				else {
					//cout<<edgeList[s][t]<<" "<<edgeList[s][v]<<" "<<edgeList[v][t]<<endl;
					scores[v] += (double(edgeList[s][v]+edgeList[v][t])/double(edgeList[s][t]));
				}

			}
		}
	}

}

int Test::calculateDisconnectedGraphs(int n, int**& edgeList) {

	std::vector<bool> isVisited;
	for(int i = 0; i<n;i++) {
		isVisited.push_back(false);
	}
	int numOfDisconnectedGraphs = 0;
	for(int i = 0; i<n;i++) {
		if( isVisited[i] == false) {
			isVisited[i] = true;
			numOfDisconnectedGraphs++;
			for(int j = i; j<n;j++) {
				if(edgeList[i][j] > 0) isVisited[j] = true;
			}	
		} 

	}
	return numOfDisconnectedGraphs;	
}

int Test::Important (int n, int**& edgeList, double*& scores)
{  

	for (int s = 0; s < n; s++)  
    {  
        for (int t = 0; t < n; t++)  
        {  
            for (int c = 0; c < n; c++)  
            {  
            	if((edgeList[t][s] == 0 and t != s) or (edgeList[s][c] == 0 and s != c)) continue;
            	else if(edgeList[t][c] == 0 and t!=c) {
            		 edgeList[t][c] = edgeList[t][s] + edgeList[s][c]; 
            	}
                else if ( edgeList[t][s] + edgeList[s][c] < edgeList[t][c])  
                    edgeList[t][c] = edgeList[t][s] + edgeList[s][c];  
            }  
        }  
    }  
	calculateScores(n,edgeList,scores);
    return calculateDisconnectedGraphs(n,edgeList);
}
