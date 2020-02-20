#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include <cassert>
#include "the3.h"


// void printVector(std::vector<int>& array){

//     std::cout << "size: " << array.size() << std::endl;
//     for (unsigned i = 0; i < array.size(); ++i){
//         std::cout << array[i] << " ";
//     }
//     std::cout << std::endl;
// }



using namespace std;
bool TestTask()
{
    
    // time variables
  clock_t begin;
  clock_t end;
  double duration;
  int expectedTime = 500;
  int true_answer = 3;
  int solution = -1;
  
    // generate solution
  if ((begin = clock ()) == -1) 
      std::cerr << "clock err" << std::endl;
      int n = 6;
      double *scores = new double[6];
      int **edgeList;
      edgeList = new int*[6];
     int edgelist[6][6] = {0, 0, 1, 0, 10,3,
                          0, 0, 0, 0, 0, 0,
                          1, 0, 0, 0, 8, 9,
                          0, 0, 0, 0, 0, 0,
                          10, 0, 8, 0, 0, 0,
                          3, 0, 9, 0, 0, 0};
    
    for (int i = 0; i < 6; ++i) {
      edgeList[i] = new int[6];
    }
    
    for(int i =0 ; i<6;i++){
        for(int j=0; j<6;j++){
            edgeList[i][j] = edgelist[i][j];
        }
    }
      
    solution = Important(n, edgeList, scores);

    for(int i = 0; i<6;i++){
      for(int j=0; j<6;j++){
        std::cout<<edgeList[i][j]<<" ";
      } std::cout<<std::endl;
    }

      std::cout<<"Scores HERE"<<std::endl;
      for(int j=0; j<6;j++){
        std::cout<<scores[j]<<" ";
      } 
    std::cout<<std::endl;
    

    for(int i = 0; i < 6; ++i){
        delete[] edgeList[i];//deletes an inner array of integer;
    }
    
    delete[] scores; 
    
    cout<<"Solutuion:  "<<solution<<endl;
  if ((end = clock ()) == -1) 
      std::cerr << "clock err" << std::endl;
      
  duration = ((double)end-begin) / CLOCKS_PER_SEC * 1000000;
  
  cout << "Elapsed time  " << duration << " vs " << expectedTime << " microsecs" << std::endl;

  return (solution == true_answer);
}





int main()
{
    srandom(time(0));       // no need to initialize second time

  if (TestTask())
    std::cout << "PASSED TASK" << std::endl << std::endl;
  else
    std::cout << "FAILED TASK" << std::endl << std::endl;
  

  return 0;
}
