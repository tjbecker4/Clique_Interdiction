/**********************************************************************
Updated version of Ben McClosky's maximum k-plex algorithm based on 
Ostergard's maximum clique algorithm. Finds a maximum weighted clique.

Author: Tim Becker
Summer 2014

**********************************************************************/



#include "Graph.h"
#define inf 10000000
#define epsilon 1e-10
#define max(i,j) (i<=j) ?j:i
#define min(i,j) (i>=j) ?j:i
using namespace std;

void quicksort(int *array, mygraph G, int top, int bottom);
int partition(int *array, mygraph G, int top, int bottom);
pair<vector<double>,vector<int> > sort(vector<double> &delta, vector<int> &nodes);

Graph::Graph(){
  lbound = -inf;  
  found=0;
  list<int> Qmax;
}
Graph::~Graph(){
}




/********************************************
input: command line argument
1) open and read data file
2) create graph objects G and CG (graph and complement graph)
3) find upper and lower bounds on independence
4) call SOLVE function for exact solution
*******************************************/
//list<int> Graph::Load_and_Solve(string filename,vector<double> weights,vector<int> nodes, double RANDOM, double prob, int **A){
list<int> Graph::Load_and_Solve(vector<double> weights, vector<int> nodes, int **A){
  //graph values
  lbound = -inf;	
  int gnodes = 50;
  int gedges;
  int *gend1=NULL,*gend2=NULL;


  //lists to store vertices and cliques
  list<int> S;
  list<int> Q;
  vector<int>::iterator itr;
  list<int>::iterator lit;
  
  //Initialize solution vector
  list<int> Solution;


  //*********************** READ IN A GIVEN GRAPH ************************
  /*
  if(RANDOM == 0){
    //read graphs in from DIMACS formatted files
    ifstream f_in(filename.c_str());
    if(!f_in){
      cerr<<"can't open file "<< filename<<endl<<endl;
      Solution = Qmax;
      return Solution;
    }
    
    f_in>>gnodes;
    f_in>>gedges;
    
    gend1 = new int[gedges];
    gend2 = new int[gedges];
    //continue reading; store complement graph as an adjacency matrix A
    
    A = new int*[gnodes];
    
    for(int i = 0; i < gnodes; i++){
      A[i] = new int[gnodes];
    }

    
    for(int i = 0; i < gnodes; i++){
      for(int j = i; j < gnodes; j++){
	A[i][j] = 1;
	A[j][i] = 1;
      }
    }
    
    int discard;
    int i=0;
    while ((i<gedges)){
      f_in>>gend1[i];	//arrays used to build
      f_in>>gend2[i];	//graph object
      f_in>>discard;
      int a=min(gend1[i],gend2[i]);
      int b=max(gend1[i],gend2[i]);
      A[a][b-a-1]=0;
      i++;
    }
    
    //create graph object to store original graph G
    Create_Graph(gnodes, gedges, gend1, gend2, G);
    delete []gend1;
    delete []gend2;
    gend1 = NULL;
    gend2 = NULL;
    
    }*/

  
  double *C;      //C[i] stores weight of largest clique in S_i={i,...,end} 
  C=new double[gnodes];  
  
  for(int i=0;i<gnodes;i++){
    C[i]=0;  //init pruning vector
  }	 
  
  //Begin timer
  clock_t start;
  start=clock();
  

  // NEW SORTING SECTION ************************************************

  /*pair<vector<double>, vector<int> > Result;
 
  Result = sort(weights,nodes);
  weights = Result.first;
  nodes = Result.second;*/
  cout << "Nodes.size() = " << nodes.size() << endl;

  cout << "Nodeweights: ";
  
  for(int i = 0; i < weights.size(); i++){
    cout << weights[i] << " ";
  }
  cout << endl;
  
  
  cout << "Nodes: ";
  
  for(int i = 0; i < nodes.size(); i++){
    cout << nodes[i] << " ";
  }
  cout << endl;

  /* for(int i = 0; i < weights.size(); i++){
    if(weights[i] < 0){
      weights[i] = 0;
    }
  }
  cout << "NEW NODEWEIGHTS: ";
  for(int i = 0; i < weights.size(); i++){
    cout << weights[i] << " ";
  }
  cout << endl;*/ 

  map<int,double> weightmap;
  for(int i = 0; i < nodes.size(); i++){
    weightmap[nodes[i]] = weights[i];
  }

  vector<int>::reverse_iterator rit = nodes.rbegin();
  vector<int>::reverse_iterator riter = nodes.rbegin();

  //this loop corresponds to function new() in Ostergard's paper
  for(rit = nodes.rbegin(); rit != nodes.rend(); ++rit){
    S.clear();

    for(riter = nodes.rbegin(); riter != rit; ++riter){
      S.push_back(*riter);
    }
    found = 0;
    Q.clear();
    Q.push_back(*rit);

    SOLVE(A,S,start,C,weights,weightmap,Q);

    Solution = Qmax; //update current maximum weighted clique
    C[*rit] = lbound; //update pruning vector

  } 

  /****************************************
  //this loop corresponds to function new() in Ostergard's paper
  for(int j=gnodes-1; j>=0; j--){

    S.clear();  		
    for(int i=j+1;i<gnodes;i++){
      S.push_back(i);
    }
    found=0;       	//found = false 
    Q.clear();
    Q.push_back(j);
    
  
    SOLVE(A,S,start,C,weights);
    
    Solution = Qmax; //update current maximum weighted clique
    C[j]=lbound; //update pruning vector
  
    Sol = make_pair(Solution,lbound);
    }
  *************************************/

  if(Solution.size() == 0){
    cout << "Did not find a clique" << endl;
  }

  //free memory and return solution


 
  delete []C;
  C=NULL;
  //delete []P;
  //P=NULL;
  Qmax.clear();
  Q.clear();
  weights.clear();
  nodes.clear();
  return Solution;
  Solution.clear();

  
}//end load_data function


/*******************************************************************************
recursive function to find max clique
A=adjacency matrix of complement graph CG
U=candidate list -- nodes that can be added to current clique (Q)
*******************************************************************************/
void Graph::SOLVE(int **A, list<int> U,clock_t start,double *C, vector<double> weights, map<int,double> weightmap,list<int> Q){
  
  clock_t end;
  list<int>::iterator it,iter;
  double Qweight = 0;  
  double Qmaxweight = 0;
  double Uweight = 0;
  int i;
  list<int> U_p;	   		//define new candidate set U_p
  
  
  if(U.empty()){
    //  cout << "FINAL Q: ";
    for(it=Q.begin();it!=Q.end();it++){
      Qweight += weightmap[*it];
      // cout << *it << " ";
    }
    //cout << endl;
    if(Qweight>=lbound){  //update incumbent
      Qmaxweight = Qweight;
      Qmax = Q;	
      /* cout << "QMAX: ";
      for(it = Qmax.begin(); it != Qmax.end(); it++){
	cout << *it << " ";
      }	
      cout << endl;*/
      lbound=Qmaxweight;
      found=1;		//found=true
      Q.clear();
      // cout << "U is empty and cliqueweight is: " << lbound << endl;
    }
    return;
  } 	
  
  while(!U.empty()){		//loop until candidate set is empty
    Uweight = 0;
    Qweight = 0;    
    
    // cout << "U: ";
    for(it = U.begin(); it != U.end(); it++){
      Uweight += weightmap[*it];
      //cout << *it << " ";
    }
    //cout << endl;
    


    //check time limit
    end=clock();
    if((double)(end - start)/CLOCKS_PER_SEC>3600){
      cout<< "Time's up" << endl;
      return;
    }
    
    /*    cout << "SIZE OF Q: " << Q.size() << endl;
    
    cout << "Q: ";
    for(it = Q.begin(); it != Q.end(); it++){
      cout << *it << " ";
    }
    cout << endl;*/
    
    
    //Get weight for current clique 
    for(it = Q.begin(); it != Q.end(); it++){
      Qweight += weightmap[*it]; 
    }
      /*   cout << "QWeight = " << Qweight << endl;
    cout << "UWeight = " << Uweight << endl;
    cout << "lbound = " << lbound << endl;*/

    //USE FOR POSITIVE WEIGHTED CLIQUES
    if(Qweight + Uweight < lbound){
      //  cout << "UWEIGHT RETURN" << endl;
      return;
    }    
    
    //find min index in U; try to prune
    
    int p=inf;
    for(it=U.begin();it!=U.end();it++){
      if(Qweight + weightmap[*it] >= lbound){ //USE FOR NEGATIVE WEIGHTED CLIQUES
	p=min(p,*it);
      }
    }
    //    cout << "p = " << p << endl;
    
    //PRUNE
    if(p == inf){
      U_p.clear();
      U.remove(p);
      return;
    }
    
    // cout << "C[p] = " << C[p] << endl;
    if(Qweight+C[p]<lbound){
      // cout << "C[p] RETURN" << endl;
      return;  //prune
    } 
    
    
   
    U.remove(p);
    
    //Add p to the current clique    
    Q.push_back(p);
    //cout << "size of Q: " << Q.size() << endl;

    for(iter=U.begin(); iter!=U.end(); iter++){
      int a=min(*iter,p);
      int b=max(*iter,p);
      if(!A[a][b-a-1]){
	U_p.push_back(*iter); //define U_p (new candidate set);
      } 
    }
    


    //cout << "size of U_p = " << U_p.size() << endl;
    
    SOLVE(A,U_p,start,C,weights,weightmap,Q);          //recursive call of solve
    
    Q.remove(p);		//we've found largest clique with p, now remove it
    U_p.clear();
    
    if(found==1){
      return;
    }	  
  }//while
}


/**************************************************************
a function to populate data into actual graph variables
returns a graph structure
***************************************************************/
void Graph::Create_Graph(int gnodes, int gedges, int *gend1, int *gend2, 
			mygraph &curr_graph){
  
  int i;
  curr_graph.mxdeg = 0;
  curr_graph.mindeg = 99999999;
  
  curr_graph.nodes=gnodes;   //number of nodes
  curr_graph.edges=gedges;   //number of edges
  
  curr_graph.nlist=new mynode[curr_graph.nodes]; //array of nodes
  curr_graph.elist=new myedge[curr_graph.edges]; //array of edges
  
  
  //initialize everything to 0
  for(i=0; i<curr_graph.nodes; i++){
    curr_graph.nlist[i].degree = 0;
    curr_graph.nlist[i].mark = 0;
    curr_graph.nlist[i].adj = (int *)NULL;
    
  }
  
  //computes the degree of each node and sets up the data of the edge
  for(i=0; i < curr_graph.edges; i++){
    curr_graph.nlist[gend1[i]].degree++;
    if(gend1[i] != gend2[i]){
      curr_graph.nlist[gend2[i]].degree++;
    }
    
    curr_graph.elist[i].end1 = gend1[i];
    curr_graph.elist[i].end2 = gend2[i];
  }
  
  //sets up the adjacency of each node
  for(i=0; i<curr_graph.edges; i++){
    int u = curr_graph.elist[i].end1;
    int v = curr_graph.elist[i].end2;
    
    if(curr_graph.nlist[u].adj == NULL){
      curr_graph.nlist[u].adj = new int [curr_graph.nlist[u].degree];
    }
    
    if(curr_graph.nlist[v].adj == NULL){
      curr_graph.nlist[v].adj = new int [curr_graph.nlist[v].degree];
    }
    
    curr_graph.nlist[u].adj[curr_graph.nlist[u].mark] = v;
    curr_graph.nlist[u].mark++;
    if(u != v){
      curr_graph.nlist[v].adj[curr_graph.nlist[v].mark] = u;
      curr_graph.nlist[v].mark++;
    }
  }
  for(i=0; i<curr_graph.nodes; i++){
    curr_graph.mxdeg=max(curr_graph.mxdeg, curr_graph.nlist[i].degree);
    curr_graph.mindeg=min(curr_graph.mindeg, curr_graph.nlist[i].degree);
  }
  return;
}

/*************************************************
 prints a Graph
*************************************************/
int Graph::Print_Graph(mygraph current){
  for(int i=0;i<current.edges;i++){
    cout <<current.elist[i].end1 <<" ";
    cout <<current.elist[i].end2 <<" ";
    cout <<endl;
  }
  return 0;
}


/*************************************************
 free memory associated with graph storage
*************************************************/
void Graph::clearGraph(mygraph &g){
  int i;
  if(g.nodes){
    for(i=0; i<g.nodes; i++){
      if(g.nlist[i].degree >0) delete []g.nlist[i].adj;
      g.nlist[i].adj=NULL;
      g.nlist[i].degree=0;
      g.nlist[i].mark = 0;
    }
  }
  
  if(g.nlist!=NULL)delete []g.nlist;
  if(g.elist!=NULL)delete []g.elist;
  g.nlist=NULL;
  g.elist=NULL;
  g.nodes=0;
  g.edges=0;
  g.mxdeg = 0;
  
}

/****************************************************
Partition and QuickSort; recursive sorting alg
****************************************************/
int partition(int *array, mygraph curr_graph, int top, int bottom){
  int x = curr_graph.nlist[array[top]].degree;
  int i = top - 1;
  int j = bottom + 1;
  int temp;
  while(1){
    do{
      j--;
    }while(x>curr_graph.nlist[array[j]].degree);
    
    do{
      i++;
    }while(x<curr_graph.nlist[array[i]].degree);
    
    if(i<j){
      temp = array[i];    			  //switch elements at positions i and j
      array[i] = array[j];
      array[j] = temp;
    }
    else {return j;}
  }
}
void quicksort(int *array, mygraph curr_graph, int top, int bottom){
  int middle;
  if(top<bottom){
    middle = partition(array,curr_graph, top, bottom);
    quicksort(array,curr_graph, top, middle);   		//sort top partition
    quicksort(array,curr_graph, middle+1, bottom); //sort bottom partition
  }
  return;
}


//****************************************************
pair<vector<double>,vector<int> > sort(vector<double> &delta, vector<int> &nodes){
  double temp1 = 0;
  int temp2 = 0;
  int n = nodes.size();
   
  for(int i = 0; i < n-1; i++){
    for(int j = i+1; j < n; j++){
      
      if(delta[j] < delta[i]){
	
        temp1 = delta[j];
        delta[j] = delta[i];
        delta[i] = temp1;
	
        temp2 = nodes[j];
        nodes[j] = nodes[i];
        nodes[i] = temp2;
      }
    }
  }
  return pair<vector<double>,vector<int> >(delta,nodes);
}
//*************************************************



