
/****************************************************************************
This program uses Gurobi and column generation to solve the Bilevel Clique Interdiction Problem.

Author: Tim Becker
2014-2015

Usage: clique <graph file> <double Defender's budget> <double Attacker's budget> <double RANDOM> <double Edge Probability>  

***************************************************************************/


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


#define inf 10000000
#define epsilon 1e-10
using namespace std;

vector<double> sol;
  
//Defining functions so all can find them
void maximalcliques(int startingnode, int **A, vector<vector<int> > &allcliques,GRBModel &model, vector<GRBConstr> &Constraints, vector<GRBVar> &x, vector<GRBVar> &z, GRBLinExpr &obj, int n, vector<int> &alreadyused);

void singleclique(vector<int> &newclique, vector<int> &candidates, int **A, vector<vector<int> > &allcliques, vector<double> &nodeweights, GRBModel &model, vector<GRBConstr> &Constraints, vector<GRBVar> &x, vector<GRBVar> &z, GRBLinExpr &obj, int n, vector<int> &alreadyused);

double BB(GRBModel &model, string filename, double &best, vector<GRBConstr> &Constraints, vector<GRBVar> &x, vector<GRBVar> &z, GRBLinExpr &obj, int n, vector<double> &cc, double RANDOM, double prob, int yes, vector<vector<int> > allcliques, clock_t start, int **A, vector<int> &alreadyused);




//*****************************Sort a vector***************************************************


vector<int> vectorsort(vector<int> &clique){

  std::sort(clique.begin(), clique.end());

  return clique;



}


//**************************Update New Candidate List*****************************************

//vector<int> NewCandidates(vector<vector<int> > &cliquegroup, vector<int> &candidates){ 
vector<int> NewCandidates(vector<int> &newclique, vector<int> &candidates){  
  int check = 0;
  vector<int> newcandidates;
  

  for(int i = 0; i < candidates.size(); i++){
    check = 0; 
    for(int j = 0; j < newclique.size(); j++){
      if(candidates.at(i) == newclique.at(j)){
	check = 1;
      }

    }
    if(check == 0){
      newcandidates.push_back(candidates.at(i));
    }

  }


  return newcandidates;
  
}

//*********************************Find Nodeweights*********************************************


vector<double> GetNodeweights(vector<GRBConstr> &Constraints, vector<vector<int> > &allcliques, vector<int> &nodes, int n,vector<double> &cc){

  vector<vector<int> >::iterator itera;
  double weight;
  vector<double> nodeweights;
  int k = 1;
 
  for(int i = 0; i < n; i++){  
    k = 1;
    weight = -cc[i]*Constraints[1].get(GRB_DoubleAttr_Pi)-Constraints[2+i].get(GRB_DoubleAttr_Pi)-Constraints[n+2+i].get(GRB_DoubleAttr_Pi);
    for(itera = allcliques.begin(); itera != allcliques.end(); itera++){
      for(int j = 0; j < (*itera).size(); j++){	  
	if(i == (*itera)[j]){
	  weight = weight - Constraints[1+3*n+k].get(GRB_DoubleAttr_Pi);
	}
      }
      k= k+2;
    }
    nodeweights.push_back(weight);
  }
  return nodeweights;
}


//************************************Print Vector(int)*********************************************

void PrintIntVector(vector<int> &vec){

  for(int i = 0; i < vec.size(); i++){
    cout << vec.at(i) << " ";
  }
  cout << endl;
  

}



//************************************Print Vector(double)*********************************************

void PrintDoubleVector(vector<double> &vec){

  for(int i = 0; i < vec.size(); i++){
    cout << vec.at(i) << " ";
  }
  cout << endl;
  

}

//***************************************Find New Cliques*****************************************

bool AddCliques(GRBModel &model, string filename, double &best, vector<GRBConstr> &Constraints, vector<GRBVar> &x, vector<GRBVar> &z, GRBLinExpr &obj, int n, vector<double> &cc, double RANDOM, double prob, int yes, vector<vector<int> > &allcliques, clock_t start, int **A,vector<int> &alreadyused){


  vector<int> candidates;
  vector<double> nodeweights;
  double weight;
  vector<vector<int> >::iterator itera;
  vector<int> nodes;
  int keepgoing = 1;
 
  // Find a clique with negative reduced cost to bring into the model as a new column
  while(keepgoing){
    for(int i = 0; i < n; i++){
      nodes.push_back(i);
    }
    
    nodeweights = GetNodeweights(Constraints,allcliques,nodes,n,cc);
    
    int repeat = 0;

    for(int i = 0; i < n; i++){
      if(nodeweights.at(i) > -1 && nodeweights.at(i) < -epsilon){
	for(int j = 0; j < alreadyused.size(); j++){
	  if(i == alreadyused.at(j)){
	    repeat = 1;
	  }
	}
	if(repeat == 0){
	  candidates.push_back(nodes.at(i));
	}
	repeat = 0;
      }
    }
    

    
    if(candidates.size() == 0){
      break;
    }
    
    vector<int> newclique;
    newclique.push_back(candidates.back());    
    singleclique(newclique,candidates,A,allcliques,nodeweights,model,Constraints,x,z,obj,n,alreadyused);

        
    model.update();
    model.write("test.lp");
    model.optimize();
    
    newclique.clear();
    nodes.clear(); 
    nodeweights.clear();
    candidates.clear();      
    
    
    
  
  }
  
  
  
  nodeweights.clear();    
  nodes.clear();   
  candidates.clear();
  return false;
}


//*************************************Branching********************************************


double BranchAndBound(GRBModel &model, string filename, double &best, vector<GRBConstr> &Constraints, vector<GRBVar> &x, vector<GRBVar> &z, GRBLinExpr &obj, int n, vector<double> &cc, double RANDOM, double prob, int yes, vector<vector<int> > allcliques, clock_t start, int **A,double objective, vector<int> &alreadyused){

  if(yes){
    yes = 0; 
  }
  model.update();
  model.write("test.lp");
  model.optimize();
  

  objective = model.get(GRB_DoubleAttr_ObjVal);
  if(objective < best){
    return objective;
  }

 
  vector<double> branching;

  for(int i = 0; i < 2*n; i++){
    if(i < n){
      branching.push_back(x[i].get(GRB_DoubleAttr_X));
    }
    if(i >= n){
      branching.push_back(z[i-n].get(GRB_DoubleAttr_X));
    }
  }
   
  int k;
  double bound;
  bool flag = 0;
  double improve = inf;
  double attribute; 
  for(int i = 0; i < 2*n; i++){
    if(branching.at(i) > epsilon && branching.at(i) < 1.0-epsilon){

      flag = 1;


      //Most infeasible branching
      if(fabs(branching.at(i)-0.5) < improve){
	improve = fabs(branching.at(i)-0.5);
	k = i;
      }
    }
  }



  if(flag){
    try{
        
      //      cout << "branching on " << k << endl;
      if(k < n){
	x[k].set(GRB_DoubleAttr_UB,0.0);
	objective = BB(model,filename,best,Constraints,x,z,obj,n,cc,RANDOM,prob,yes,allcliques,start,A,alreadyused);
	x[k].set(GRB_DoubleAttr_UB,1.0);
	x[k].set(GRB_DoubleAttr_LB,1.0);
	objective = BB(model,filename,best,Constraints,x,z,obj,n,cc,RANDOM,prob,yes,allcliques,start,A,alreadyused);
	x[k].set(GRB_DoubleAttr_LB,0.0);
      }
      if(k >= n){
	z[k-n].set(GRB_DoubleAttr_UB,0.0);
	objective = BB(model,filename,best,Constraints,x,z,obj,n,cc,RANDOM,prob,yes,allcliques,start,A,alreadyused);
	z[k-n].set(GRB_DoubleAttr_UB,1.0);
	z[k-n].set(GRB_DoubleAttr_LB,1.0);
	objective = BB(model,filename,best,Constraints,x,z,obj,n,cc,RANDOM,prob,yes,allcliques,start,A,alreadyused);
	z[k-n].set(GRB_DoubleAttr_LB,0.0);

      }
    }
     catch(...){cout << "infeasible" << endl;}
  }

 
  if(objective > best){
    //cout << "updating best" << endl;
    best = objective;
    // cout << "current best: " << best << endl;
    sol.clear();
    for(int i = 0; i < 2*n; i++){
      if(i < n){
	sol.push_back(x[i].get(GRB_DoubleAttr_X));
      }
      if(i >= n){
	sol.push_back(z[i-n].get(GRB_DoubleAttr_X));
      } 
    }
  }
  else{
    return objective;
  }
  
  


}


//************************************Number to String Function*******************************

string NumberToString(int number){
  ostringstream ss;
  ss << number;
  return ss.str();
}


//********************************Check for Duplicate Clique************************************
int CheckDuplicateClique(vector<vector<int> > allcliques, vector<int> newclique){
  
  vector<vector<int> >::iterator it;
  int SAME = 0;
  int REPEAT = 0;
  
  //CHECK TO SEE IF CLIQUES HAVE THE SAME SIZE
   for(it = allcliques.begin(); it != allcliques.end(); it++){
    SAME = 0;
    //IF THEY HAVE THE SAME SIZE, CHECK THEIR NODES
     if((*it).size() == newclique.size()){
      for(int i = 0; i < (*it).size(); i++){
	
	//IF EACH NODE SHOWS UP IN THE OTHER CLIQUE, THEN THEY ARE THE SAME
	if((*it)[i] == newclique[i]){
	  SAME++;
	}
      } 
    }
    
    if(SAME == (*it).size()){
      REPEAT = 1;
    }
  }
  return REPEAT;
  
}


//*****************************Add constraint to Gurobi************************************

void addconstraint(int **A, vector<int> &newclique, vector<vector<int> > &allcliques, GRBModel &model, vector<GRBConstr> &Constraints, vector<GRBVar> &x, vector<GRBVar> &z, GRBLinExpr &obj, int n,int duplicate, vector<int> &alreadyused){

  //  cout << "Beginning add constraint to gurobi" << endl;
  vector<int> nodes;

  GRBVar yy = model.addVar(0.0,GRB_INFINITY,0.0, GRB_CONTINUOUS, "y" + NumberToString(allcliques.size()));
  
  model.update();
  model.write("test.lp");
  
  // Add the new constraint to the model    
  GRBLinExpr lx = 0;
      
  for(int i = 0; i < n; i++){
    for(int k = 0; k < newclique.size(); k++){
      if(i == newclique.at(k)){
	lx += x[i];
      } 
    }
  }
  
  int REPEAT = 0;
  
  REPEAT = CheckDuplicateClique(allcliques,newclique);
  
  if(REPEAT == 0){
    
    allcliques.push_back(newclique);
    
    GRBConstr *r = new GRBConstr[1];
    r[0] = model.addConstr(lx - yy, GRB_GREATER_EQUAL, 0);
    Constraints.push_back(r[0]);
    delete[] r;   
    
    
    GRBConstr *b = new GRBConstr[1];
    b[0] = model.addConstr(yy, GRB_LESS_EQUAL,1);
    Constraints.push_back(b[0]);
    delete[] b;  
    GRBLinExpr le = yy; 
    
    //Update the objective function, as another y has been added    
    obj = obj + yy;
    model.setObjective(obj);
    
  }
   else{
     if(duplicate == 0){
       //cout << "TRIED ADDING A DUPLICATE CLIQUE--add more nodes to it" << endl;

       newclique = vectorsort(newclique);
       alreadyused.push_back(newclique.front());
       //cout << "Starting Node: " << newclique.front() << endl;
       maximalcliques(newclique.front(),A,allcliques,model,Constraints,x,z,obj,n,alreadyused);
       return;      
     }
     else{
       // cout << "TRIED ADDING A DUPLICATE MAXIMAL CLIQUE--MOVE ON" << endl;

     }
   }

  nodes.clear();

  return;
}


//*****************************Find Given Clique with current maximum weight node*********************


void updateclique(vector<int> &candidates, vector<int> &clique, int **A){


  //Delete all nodes from candidates which are not adjacent to all members of the clique
  for(int q = 0; q < clique.size(); q++){
    for(int k = 0; k < candidates.size(); k++){
      if(A[clique.at(q)][candidates.at(k)] == 0 || clique.at(q) == candidates.at(k)){
	candidates.erase(candidates.begin()+k);
	k--;
      }
    }
  }
  
  if(candidates.size() == 0){
    return;
  }


  //Choose largest weight node to add to the clique from candidates
  clique.push_back(candidates.back());
  
  if(candidates.size() == 1){
    return;
  }
  updateclique(candidates,clique,A);
  
  
}




//**************************Add Maximal Cliques containing a given smaller clique***************



void maximalcliques(int startingnode, int **A, vector<vector<int> > &allcliques,GRBModel &model, vector<GRBConstr> &Constraints, vector<GRBVar> &x, vector<GRBVar> &z, GRBLinExpr &obj, int n,vector<int> &alreadyused){

  //  cout << "beginning maximalcliques" << endl;
  vector<int> clique;
  Graph g(A,n,startingnode);

  g.MaxCliques(n,startingnode);

  // cin.get();

  for(int i = 0; i < (g.newcliques).size(); i++){
    clique = g.newcliques[i];
    addconstraint(A,clique,allcliques,model,Constraints,x,z,obj,n,1,alreadyused);
    clique.clear();
  }

  model.update();
  model.write("test.lp");
 
  
  return;

}




//*************************************Add one clique at a time****************************************

void singleclique(vector<int> &newclique,vector<int> &candidates, int **A, vector<vector<int> > &allcliques, vector<double> &nodeweights, GRBModel &model, vector<GRBConstr> &Constraints, vector<GRBVar> &x, vector<GRBVar> &z, GRBLinExpr &obj, int n, vector<int> &alreadyused){


  updateclique(candidates,newclique,A);
  
  addconstraint(A,newclique,allcliques,model,Constraints,x,z,obj,n,0,alreadyused);  
  
  model.update();
  model.write("test.lp");


  return;
  
}


//*************************************Begin to Add Cliques**********************************************

double BB(GRBModel &model, string filename, double &best, vector<GRBConstr> &Constraints, vector<GRBVar> &x, vector<GRBVar> &z, GRBLinExpr &obj, int n, vector<double> &cc, double RANDOM, double prob, int yes, vector<vector<int> > allcliques, clock_t start, int **A,vector<int> &alreadyused){

  bool getout;
  model.update();
  model.write("test.lp");
  model.optimize();
  double objective = model.get(GRB_DoubleAttr_ObjVal); 

  getout = AddCliques(model,filename,best,Constraints,x,z,obj,n,cc,RANDOM,prob,yes,allcliques,start,A,alreadyused);

 
  //Branching


  objective = BranchAndBound(model,filename,best,Constraints,x,z,obj,n,cc,RANDOM,prob,yes,allcliques,start,A,objective,alreadyused);

  
  return objective;



}


//******************************Main Function**********************************

int main(int argc, char* argv[]){

  if(argc != 7){
    cout << "Error: Usage is <graph> <Defender's Budget> <Attacker's Budget> <RANDOM> <Edge Probability> <Number of nodes>" << endl;
    return 1;
  }
  
  try{
    // Declare the GRBG environment
    GRBEnv env = GRBEnv();
    GRBModel model= GRBModel(env);
    model.set(GRB_IntAttr_ModelSense, -1);

    double RANDOM = atof(argv[4]);
    
    if(RANDOM !=0 && RANDOM != 1){
      cout << "ERROR: RANDOM MUST BE 0 OR 1" << endl;
    }

    int **A;
    double prob = atof(argv[5]);
    double rdm;
    string filename(argv[1]);
    int n,m;


    
    if(RANDOM == 0){
      ifstream f_in;
      f_in.open(argv[1]);
      
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
    } 
    
    if(RANDOM == 1){      

      n = atof(argv[6]);
      
      A = new int*[n];
      for(int i = 0; i < n; i++){
	A[i] = new int[n];
      }     
      
      for(int i = 0; i < n; i++){
	for(int j = i; j < n; j++){
	  A[i][j] = 0;
	  A[j][i] = 0;
	}
      }
      
      
      srand(time(NULL));
      
      for(int k = 0; k < n; k++){
	for(int j = k; j < n; j++){
	  rdm = ((double) rand() / (RAND_MAX));
	  if(rdm < prob){
	    A[k][j] = 1;
	    A[j][k] = 1;
	  }
	}
      }
      for(int i = 0; i < n; i++){
	A[i][i] = 1;
      }

    }

   

    /*cout << "Adjacency matrix : " << endl;
    
    for(int i = 0; i < n; i++){
      for(int j = 0; j < n; j++){
	cout << A[i][j] << " ";
      }
      cout << endl;
    }
    cin.get();*/


    // Set budget for defender (Q) and attacker (R)

    double Q = atof(argv[2]);
    double R = atof(argv[3]);


   
    // Vectors for Cost of Interdicting (c) and Removing (d) a node

    vector<double> cc;
    vector<double> d;
    double rdm1;
    double rdm2;
    srand(time(NULL));
    

    rdm1 = ((double) rand() / (RAND_MAX))*5;
    rdm2 = ((double) rand() / (RAND_MAX))*5;
    

    d.push_back(rdm1);
    cc.push_back(rdm2);
   

    //Comment out the following lines if costs are given
    for(int i = 1; i < n; i++){
      rdm1 = ((double) rand() / (RAND_MAX))*n;
      rdm2 = ((double) rand() / (RAND_MAX))*n;
      cc.push_back(rdm1);
      d.push_back(rdm2);
     
    }

    //***********************Begin Model**************************

    // Gurobi Variables
    vector<GRBConstr> Constraints;
    vector<GRBVar> x;
    vector<GRBVar> z; 

    model.update();
    
    //Add Variables to the model
    for(int i = 0; i < n; i++){
      GRBVar xx = model.addVar(0.0,GRB_INFINITY, 1.0,GRB_CONTINUOUS, "x" + NumberToString(i));
      //GRBVar xx = model.addVar(0.0,1.0,1.0,GRB_CONTINUOUS, "x" + NumberToString(i));
      x.push_back(xx);
      GRBVar zz = model.addVar(0.0, GRB_INFINITY, 1.0,GRB_CONTINUOUS, "z"+NumberToString(i));
      //GRBVar zz = model.addVar(0.0,1.0,1.0,GRB_CONTINUOUS, "z" + NumberToString(i));
      z.push_back(zz); 
    }

    model.update();
    
    //Gurobi Linear Expressions
    GRBLinExpr pcost;
    GRBLinExpr icost;
    GRBLinExpr *lexpr = new GRBLinExpr[n];
    GRBLinExpr *lexpr1 = new GRBLinExpr[n];
    GRBLinExpr *lexpr2 = new GRBLinExpr[n];
    GRBLinExpr obj = 0;

    //Update Linear Expression to be used for objective
    for(int i = 0; i < n; i++){
      obj += z[i];
    }

    model.setObjective(obj);
    
    //Update Linear Expressions to be used as constraints
    for(int i = 0; i < n; i++){
      pcost += d[i]*z[i];
      icost += cc[i]*x[i];
      lexpr[i] = x[i] + z[i];
      lexpr1[i] = x[i];
      lexpr2[i] = z[i];
    }
 

   
    //Add Constraints
    GRBConstr pc = model.addConstr(pcost, GRB_LESS_EQUAL, Q);
    Constraints.push_back(pc);

    GRBConstr ic = model.addConstr(icost, GRB_LESS_EQUAL, R);
    Constraints.push_back(ic);

    GRBConstr *p = new GRBConstr[n];
    
    for(int i = 0; i < n; i++){
      p[i] = model.addConstr(lexpr[i], GRB_LESS_EQUAL, 1);
      Constraints.push_back(p[i]);
    }

    
    GRBConstr *h = new GRBConstr[n];
    
    for(int i = 0; i < n; i++){
      h[i] = model.addConstr(lexpr1[i], GRB_LESS_EQUAL, 1);
      Constraints.push_back(h[i]);
    }
    
    
    GRBConstr *g = new GRBConstr[n];
    
    for(int i = 0; i < n; i++){
      g[i] = model.addConstr(lexpr2[i], GRB_LESS_EQUAL, 1);
      Constraints.push_back(g[i]);
    }
    
    model.update();
    model.write("test.lp");    
    model.optimize();  
    
    //Solve the MIP
    double best = -inf;
    int yes = 1;
    vector<vector<int> > allcliques;
    vector<int> alreadyused;
    clock_t begin = clock();
    double objective = BB(model, filename, best, Constraints, x, z, obj, n, cc, RANDOM,prob,yes,allcliques,begin, A,alreadyused);

    clock_t end = clock();
    delete[] lexpr;
    delete[] p;
    delete[] h;
    delete[] g;
    delete[] lexpr1;
    delete[] lexpr2;
    alreadyused.clear();
    Constraints.clear();
    
    for(int i = 0; i < allcliques.size(); i++){
      (allcliques[i]).clear();
    }
    allcliques.clear();

    
    for(int i = 0; i < n; i++){
      delete[] A[i];
    }
    delete[] A;
    
    //**********************Post Processing*************************************
    
        
    cout<<"=======================Solution==========================="<<endl;


    // UNCOMMENT IF NEED ALL VALUES TO BE OUTPUT

    /*    cout << "x values " << "z values " << endl;
    for(int i = 0; i < n; i++){
      cout << "x[" << i << "] = " << sol.at(i) << endl;
    }
    for(int i = n; i <2*n; i++){
      cout << "z[" << i-n << "] = " << sol.at(i) << endl;
      }*/
   
    cout<<"Nodes chosen for interdiction:"<<endl;
    
    for(int i = 0; i < n; i++){
        if(sol.at(i) == 1){
	cout<<"Node "<<i+1<<endl;
      }
    }
    
    /* cout <<"Nodes partially chosen for interdiction:"<<endl;
    for(int k = 0; k < n; k++){
        if(sol.at(k) != 0 && sol.at(k) != 1){
	cout <<"Node "<<k<< endl;
      }
    }
    
    cout <<"Nodes partially protected from interdiction:"<<endl;
    for(int k = n; k < 2*n; k++){
       if(sol.at(k) != 0 && sol.at(k) != 1){
	 cout <<"Node "<<k-n<<endl;
      }
      }*/
    
    cout << "Nodes protected from interdiction:" << endl;
    for(int j = n; j < 2*n; j++){
      if(sol.at(j) == 1){	
	cout<<"Node "<<j-n+1<<endl;
      }
    }
    
    //cout << "The objective cost of the interdiction is " << best << endl;
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout <<"Time to solve: "<<elapsed_secs<<"s";
    double elapsed_mins = elapsed_secs/60;
    cout << "(or " << elapsed_mins<<"m)"<<endl;
    
    x.clear();
    z.clear();
    
    
  } catch(GRBException e) {
    cout << "Error code = " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  } catch (...) {
    cout << "Error during optimization" << endl;
  }
  
  
  return 0;
  
}
