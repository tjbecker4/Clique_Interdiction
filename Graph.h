/**************************************************************************
* header file for class Graph,                                            *   
**************************************************************************/
#ifndef Class_Graph
#define Class_Graph
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <fstream>
#include <vector>
#include <string>
#include <time.h>
#include <list>
#include <map>
#include <algorithm>
#include <iterator>

using namespace std;

//data structure to hold data for nodes
struct mynode{
  int degree,     	//node degree
    mark,       	//used to define *adj
    colL,
    cSet;       	//tracks component membership
  int* adj;       	//the array of adjacent nodes
};

//data structure to hold data for edges
struct myedge{
  int end1,     	//one end of the edge
    end2;       //the other end   
};

//data structure for graphs
struct mygraph{
  int nodes,mxdeg,edges,mindeg;  		//graph invariants
  mynode *nlist;						//list of nodes
  myedge *elist;						//list of edges
};

//the graph class
class Graph{
  
 public:
  Graph();
  ~Graph();
  
  int NODES,EDGES,OPT,K,BBN;
  double lbound;
 
  //function to load graph data, run heuristics, and call SOLVE
  //  list<int> Load_and_Solve(string filename,vector<double> weights,vector<int> nodes,double RANDOM, double prob, int **A);
  list<int> Load_and_Solve(vector<double> weights, vector<int> nodes, int **A);

 private:
  
  mygraph G;   //graph structure
  int found;
  
  //Qmax stores incumbent; Q stores current kplex
  list<int> Q,Qmax;
  
  //recursive algorithm to solve kplex
  void SOLVE(int **A, list<int> R, clock_t start,double* C, vector<double> weights, map<int,double> weightmap,list<int> Q);
  
  //these function create and handle the graph objects
  void Create_Graph(int gnodes, int gedges, int *gend1, int *gend2,
		   mygraph &curr_graph); 
  int Print_Graph(mygraph current);
  
  void clearGraph(mygraph &g);

};

#endif //class Graph
