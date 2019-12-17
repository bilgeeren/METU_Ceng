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

void printVector(std::vector<int> array){
    for (unsigned i = 0; i < array.size(); ++i){
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
}

void calculateScores(int n, std::vector<std::vector<int>>& edgeList, double*& scores) {

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

int calculateDisconnectedGraphs(int n, std::vector<std::vector<int>>& edgeList) {

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

int Important (int n, int**& edgeList, double*& scores)
{  
	std::vector<std::vector<int>> edgeVec;
	for(int i = 0; i < n; i++){
		std::vector<int> vec;
		edgeVec.push_back(vec);
		for (int j = 0 ; j <n; j++){
			edgeVec[i].push_back(edgeList[i][j]);
		}
	}

	for (int s = 0; s < n; s++)  
    {  
        for (int t = 0; t < n; t++)  
        {  
            for (int c = 0; c < n; c++)  
            {  
            	if((edgeVec[t][s] == 0 and t != s) or (edgeVec[s][c] == 0 and s != c)) continue;
            	else if(edgeVec[t][c] == 0 and t!=c) {
            		 edgeVec[t][c] = edgeVec[t][s] + edgeVec[s][c]; 
            	}
                else if ( edgeVec[t][s] + edgeVec[s][c] < edgeVec[t][c])  
                    edgeVec[t][c] = edgeVec[t][s] + edgeVec[s][c];  
            }  
        }  
    }  
//	for(int i = 0; i < n; i++){
//		printVector(edgeVec[i]);
//	}
	//cout<<"----------------------------"<<endl;

	calculateScores(n,edgeVec,scores);
    return calculateDisconnectedGraphs(n,edgeVec);
}
