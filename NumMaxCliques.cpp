/*******************************************************

Finds number of maximal cliques a given node is contained
within

Tim Becker
9/2015

********************************************************/

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <ostream>
#include <stdio.h>
#include <sstream>
#include <time.h>
#include <string.h>
#include <cstring>
#include <ctime>
#include <vector> //needed by Gurobi                        
#include <string>
#include <algorithm>
#include <list>
#include <map>
#include <ctype.h>
#include <ctime>
#include "gurobi_c++.h"
#include "bronkerbosch.cpp"


int main(int argc, char* argv[]){
  
  if(argc != 3){
    cout << "Error: Usage is <graph> <Starting Node> " << endl;
    return 1;
  }
  
  int **A;
  
  string filename(argv[1]);
  int n,m;
  
  ifstream f_in;
  f_in.open(argv[1]);
  int startingnode = atoi(argv[2]);
  
  if(!f_in.is_open()){
    cerr<<"Can't open file\n";
  }
  
  f_in>>n; //number of vertices			  
  f_in>>m; //number of edges			
  
  
  A = new int*[n];
  for(int i = 0; i < n; i++){
    A[i] = new int[n];
  }
  
  int *a = new int[m];
  int *b = new int[m];
  double weight;
  
  
  for(int i = 0; i < m; i++){
    f_in>>a[i]>>b[i]>>weight;
  }
  
  
  for(int i = 0; i < n-1; i++){
    for(int j = i; j < n; j++){
      A[i][j] = 0;
    }
  }
  
  for(int j = 0; j < m; j++){
    A[a[j]][b[j]] = 1;
    A[b[j]][a[j]] = 1;
  }
  
  for(int i = 0; i < n; i++){
    A[i][i] = 1;
  }
  
  delete []a;
  delete []b;
  
  
  
  Graph g(A,n,startingnode);
  
  g.MaxCliques(n,startingnode);
  
  cout << "Number of Maximal cliques containing " << startingnode << " = " << (g.newcliques).size() << endl;
  
  return 0;
  
}
