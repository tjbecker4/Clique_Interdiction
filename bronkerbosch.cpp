/*
 *  Bron & Kerbosch Algorithm to find all Maximal Cliques
 *  containing a given node
 *
 *  Reference: Algorithm 457
 *  Finding All Cliques of an Undirected Graph[H]
 *  Communications of the ACM, September 1973, Volume 16, Number 9
 *
 *  Created by Cynthia Wood on 6/?/2012
 *  Adapted by Tim Becker-Spring 2015
 *
 */

#include "modcppstd.hh"

class Operations{
  int n;
  
public:
  int size;
  int *comp;
  Operations(int maxSize);
  ~Operations();
  void CompAlloc(int);
  void Add(int);
  void Delete();
  void Remove(int);
  void OutputClique();
  void AddClique(vector<vector<int> > &newcliques);
};

Operations::Operations(int maxSize){
  n = maxSize;
  // we allocate the data when the object is being created
  comp = new int[n];
  size = 0;
  
}

Operations::~Operations(){
  delete[] comp;
}

void Operations::Add(int AddSel){
  comp[size++] = AddSel;
  
}

void Operations::Delete(void){
  size--;
}

void Operations::Remove(int i){
  
  int j;	
  int index = 0;
  
  for (j = 0; j < size; j++)
    if(comp[j] != i)
      comp[index++] = comp[j];
  size--;
}

void Operations::OutputClique(){
  cerr<<"size: "<<size<<endl;
  for(int i = 0; i < size; i++)
    if(i < size-1)
      cerr<<"   "<<comp[i];
    else{
      cerr<<"   "<<comp[i]<<endl;
    }
}

void Operations::AddClique(vector<vector<int> > &newcliques){

  vector<int> clique;

  for(int i = 0; i < size; i++){
    clique.push_back(comp[i]);
  }
  newcliques.push_back(clique);

}

class Graph{
  int n;
  int **C;
  int *Neighbors;
  int count1; 	
  Operations *compsub;
  
public:
  Graph(int **A, int n, int startingnode);
  // ~Graph();
  vector<vector<int> > newcliques;
  void MaxCliques(int n, int startingnode);
  int size(void);
  void OutputMatrix(void);
  void PrintDot(int prefix, int *shownNodes, int shownNodesCount);
   
private:
  void extension(int *old, int ne, int ce, int startingnode);
  bool inSet(int node, int *set, int setSize);
  
};

Graph::Graph(int **A, int n, int startingnode){

  //Create matrix
  C = new int*[n];
  for( int i = 0; i < n;i++)
    C[i] = new int[n];
  
  for(int i = 0; i < n; i++){
    for(int j = 0; j < n; j++) C[i][j] = A[i][j];
  }
      
 

  int count = 0;
  for(int i = 0; i < n; i++){
    if(C[i][startingnode]){
      count++;  
    }
  }

  Neighbors = new int[count];
  count1 = 1;
  Neighbors[0] = startingnode;
  for(int i = 0; i < n; i++){
    if(C[i][startingnode] && i != startingnode){
      Neighbors[count1++] = i;
    }
  }

  // cout << "NUMBER OF NEIGHBORS INCLUDING GIVEN NODE: " << count1 << endl;
 
  compsub = new Operations(n);
  //  compsub->Add(startingnode);

}

/*Graph::~Graph(){
  
  for(int i = 0; i < n; i++)
    delete[] C[i];
  delete[] C;
  delete compsub;
  delete Neighbors;
  }*/

void Graph::MaxCliques(int n, int startingnode){
  /*

  for(int i = 0; i < count1; i++){
    cout << Neighbors[i];
  }
  cout << endl;*/

  //  cout << "count1 = " << count1 << endl;

  extension(Neighbors, 0, count1, startingnode);
  
}

int Graph::size(){
  return n;
}

bool Graph::inSet(int node, int set[], int setSize) {
  for (int i = 0; i < setSize; i++) {
    if (set[i] == node) {
      return 1;
    }
  }
  return 0;
}

void Graph::PrintDot(int prefix, int shownNodes[], int shownNodesCount) {
  cout << "subgraph A_";
  cout << prefix;
  cout << " {\n";
  for(int i = 0; i < n; i++) {
    for(int j = i; j < n; j++){
      if(C[i][j] && i != j && inSet(i, shownNodes, shownNodesCount)) {
	cout << prefix;
	cout << "_";
	cout << i;
	cout << " -- ";
	cout << prefix;
	cout << "_";
	cout << j;
	cout << "\n";
      }
    }
  }
  cout << "}\n";
  
}

//Extension operator to find all the configurations of the compsub
void Graph::extension(int old[], int ne, int candidates, int startingnode){
  
  int numberOfDisconnections = 0, fixp = 0, s = 0, minnod = candidates;
  
  //Determine each counter value and look for a minimum
  
   for(int i = 0; i < candidates && minnod != 0; i++){
    int pos = 0;
    //cerr<<"I'm here after determining each counter value"<<endl;
    
    int p = old[i];
    int count = 0;
    
    //Count Disconnections, we want to select one with the smallest number of disconnections
    for(int j = ne; j < candidates && count < minnod; j++){
      if(!C[startingnode][old[j]]){
	count++;
      }
      
      if(!C[p][old[j]]){
	count++;
	//Save position of potential candidate
	pos = j;
      }      
    }
    
    //cerr<<"count: "<<count<<endl;
    //Test new minimum
    if(count < minnod){
      fixp = p;
      minnod = count;
      if(i < ne){
	s = pos;
      }
      else{
	s = i;
	//Preincr
	numberOfDisconnections = 1;
      }
    }	
    
   }
   
  
  //Bactrackcycle
  int *New = new int[candidates];
  for(numberOfDisconnections = minnod + numberOfDisconnections; numberOfDisconnections >= 1; numberOfDisconnections--){
    //cerr<<"I'm starting the backtrackcycle"<<endl;
    
    //Interchange
    int p = old[s];
    old[s] = old[ne];
    int sel = old[ne] = p;
    
    //Fill new set not
    int newne = 0;
    for(int i = 0; i < ne; i++)
      if(C[sel][old[i]])
	New[newne++] = old[i];
    
    //cerr<<"I'm about to fill new set cand"<<endl;
    //Fill new set cand
    int newCandidates = newne;
    for(int i = ne+1 ; i < candidates; i++){
      if(C[sel][old[i]]){
	New[newCandidates++] = old[i];
      }
    }                         
    
    /*cerr<<"New: ";
      for( i = 0; i < candidate;i++)
      cerr<<" "<<New[i];
      cerr<<endl;*/
    
    //cerr<<"I'm here about to add to compsub"<<endl;
    //Add to compsub 
    compsub->Add(sel);
    
    
    if(newCandidates == 0){
      //compsub->OutputClique();
      compsub->AddClique(newcliques);
    }
    
    else if(newne < newCandidates){
      //cerr<<"I'm about to call the extension operator again"<<endl;
      extension(New, newne, newCandidates,startingnode);
    }
    
    //cerr<<"I'm here about to delete from compsub"<<endl;
    //Remove from compsub
    compsub->Delete();
    
    //Add to not
    ne++;
    if(numberOfDisconnections > 1)
      //Select candidate disconnected to the fixed point
      //Look for candidate
      for(s = ne; C[fixp][old[s]]; s++);	
    //End Backtrackcycle
  }
  
  delete[] New;		
}

/*int Graph::main2(int **A, int n, int startingnode){
  
  // Graph g(A,n,startingnode);
  //cout << "graph A{\n";
  g.MaxCliques(n,startingnode);
  //cout << "}\n";
  
  return 0;
  }*/
